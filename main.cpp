/*$T main.cpp GC 1.140 08/30/19 09:24:27 */

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <malloc.h>
#include <iostream>

#include "main.h"
#include "common.h"
#include "mytypes.h"
#include "skip.h"
#include "platform.h"

using namespace std;

s8    Buff[BUFF_LEN];
cSkip sSkip = cSkip();

/*
 =======================================================================================================================
 =======================================================================================================================
 */

//s32 main(void)
s32 main(s32 argc, s8 *argv[])
{
  // s32 argc = 2;
  // s8 argv[2][400];
  // strcpy(argv[0], "C:\\Users\\ddobromirov\\Documents\\CodeBlocks-Projs\\ExportSecureQuery\\bin\\Debug\\ExportSecureQuery.exe");
  // strcpy(argv[1], "C:\\Users\\ddobromirov\\Documents\\CodeBlocks-Projs\\ExportSecureQuery\\bin\\Debug\\in.sql");

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  FILE  *I, *O, *L, *P, *S;
  s8    TableName[OTHER_LEN] = { 0 };
  s8    *FieldName[OTHER_LEN] = { 0 };
  s8    *ValueName[OTHER_LEN] = { 0 };
  s8    FieldSkip[OTHER_LEN] = { 0 };
  s8    TEMPBUFF[OTHER_LEN] = { 0 };
  u8    bFirstInsert = 0xFF;
  u8    bIsNull = 0x00;
  u32   SkipNulls = 0;
  s32   iArg = 0;
  u32   i = 0;  /* Buffer */
  u32   t = 0;  /* Table */
  u32   f = 0;  /* Field */
  u32   v = 0;  /* Value */
  u32   Cnt = 0;
  u32   Cnf = 0;
  u32   Dump = 0;
  u32   Deal = 0;
  u32   RowNum = 0;
  u32   iLines = 0;
  u32   ParamCnt = 0;
  u32   ParamRel = 0;
  u32   FieldLen = 0;
  f32   fCompleted = 0;
  u32   FuncIndent = 0;
  u32   StringIndent = 0;
  u32   LongFieldLen = 0;
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


  if(argc < 2)
  {
    printf("\nNot enough input arguments!");
    printf("\nThis program is used to convert\nexported SQL lines into secure inserts.");
    printf("\nTry drag-dropping an *.sql source\nfile into the exe...\n\n");
    printf("\nInput file should be in >DOS< format as exported from PL/SQL Dev.");
    printf
    (
      "\nSymbol behind the numbers (lines):\n %c: insert\n %c: values\n Empty -> not matched\n\n",
      MATCH_INSERT,
      MATCH_VALUES
    );
    system("PAUSE");
    return EXIT_SUCCESS;
  }

  if(StrCppEndsLike(argv[1],QUERY_EXT))
  {
    /* INITIALIZATION */
    sprintf(TEMPBUFF,"mkdir %s",SETTING_FOLD);
    system(TEMPBUFF);

    /* Open IN-OUT stream */
    I = fopen(argv[1],"rb");
    printf(">[%s]\n",argv[1]);
    strncpy(TEMPBUFF,argv[1],OTHER_LEN);
    strncpy(strstr(TEMPBUFF, QUERY_EXT),OFILE_SUFF,OTHER_LEN);
    strcat(TEMPBUFF,QUERY_EXT);
    O = fopen(TEMPBUFF,"wb");
    printf("<[%s]\n",TEMPBUFF);

    /* Open parameters */
    strncpy(TEMPBUFF,SETTING_FOLD,OTHER_LEN);
    strcat(TEMPBUFF,"\\");
    strcat(TEMPBUFF,PARAM_NAME);
    strcat(TEMPBUFF,FTEXT_EXT);
    P = fopen(TEMPBUFF,"rb");
    printf("Open parameters [%s]\n",TEMPBUFF);
    if(I != NULL)
    {
      if(O != NULL)
      {
        if(P != NULL)
        {
          /* PARAMETERS */
          iLines = NewLinesFile(I);
          fscanf(P,"MaxColumnID:%u\n",&ParamCnt);  /* The count of colons to be exported. The first N. */
          fscanf(P,"SysLogging:%u\n",&Dump);      /* Flag for enabling logging file stream */
          fscanf(P,"ReadDelay:%u\n",&Deal);       /* Delay flag for the console output display */
          fscanf(P,"SkipNulls:%u\n",&SkipNulls);  /* Skip NULL values in the query ( reduce output length ) */
          fscanf(P,"FieldSkip:%s\n",FieldSkip);   /* Relative directory path to column skip definition */
          fclose(P);
          printf("Params = {%u,%u,%u,%u,%s}\n",ParamCnt,Dump,Deal,SkipNulls,FieldSkip);
          if(Dump)
          {
            strncpy(TEMPBUFF,SETTING_FOLD,OTHER_LEN);
            strcat(TEMPBUFF,"\\");
            strcat(TEMPBUFF,LOGGS_NAME);
            strcat(TEMPBUFF,FTEXT_EXT);
            L = fopen(TEMPBUFF,"wb");
            Dump = ((L != NULL) && Dump);
            printf("Open logs [%u][%s]\n",Dump,TEMPBUFF);
          }

          if(Dump)
          {
            fprintf(L,  "InputLines :[%u]",iLines);
            fprintf(L,"\nMaxColumnID:[%u]",ParamCnt);
            fprintf(L,"\nSysLogging :[%u]",Dump);
            fprintf(L,"\nReadDelay  :[%u]",Deal);
            fprintf(L,"\nSkipNulls  :[%u]",SkipNulls);
            fprintf(L,"\nFieldSkip  :[%s]",FieldSkip);
            if(PRINT_ARGS)
            {
              for(iArg = 0; iArg < argc; iArg++) fprintf(L,"\n[%2d]:<%s>",iArg,argv[iArg]);
            }
          }

          if(ParamCnt == 0)
          {
            fprintf(O,"Max column ID cannot be zero !");
            fprintf(O,"\nParams = {%u,%u,%u,%u,%s}",ParamCnt,Dump,Deal,SkipNulls,FieldSkip);
            fprintf(O,"\nCheck parameters in [%s\\%s%s] !",SETTING_FOLD,PARAM_NAME,FTEXT_EXT);
            fprintf(O,"\nKeep in mind that strings are evaluated to 0s.");
            fclose(I);
            fclose(O);
            if(Dump)
            {
              fprintf(L,"\nMax column ID cannot be zero !");
              fprintf(L,"\nCheck parameters in [%s\\%s%s] !",SETTING_FOLD,PARAM_NAME,FTEXT_EXT);
              fclose(L);
            }

            return EXIT_SUCCESS;
          }

          printf("Processing...\n");
          while(!feof(I))
          {
            Delay(Deal);
            fgets(Buff,BUFF_LEN,I);
            if(Deal)
            {
              fCompleted = (f32) RowNum / (f32) iLines;
              if((RowNum % 6) == 0 && (RowNum != 0))
              {
                printf("  %8.4f%%\n",fCompleted * 100.0);
              }

              printf("%10u",++RowNum);
            }

            StrCppTrimBoth(Buff,' ');
            if(!strncmp(Buff,"insert into ",12))
            {
              i = 0;
              if(Dump)
              {
                fprintf(L,"\n\nGot -> Insert");
              }

              while(i < 12)
              {
                if(CharIsSmallLatinLetter(Buff[i]))
                {
                  Buff[i] = CharToggleCapitalizeLatin(Buff[i]);
                }

                i++;
              }

              if(bFirstInsert)
              {
                if(Dump)
                {
                  fprintf(L,"\nInitializing columns...");
                }

                Cnf = 0;
                while((Buff[Cnf] != '\0') && (Cnf < BUFF_LEN))
                {
                  if(Buff[Cnf] == ',')
                  {
                    ParamRel++;
                  }

                  Cnf++;
                }

                Cnf = 0;
                ParamRel++;
                if(Dump)
                {
                  fprintf(L,"\nMax parameters {%u >= %u} ?",ParamRel,ParamCnt);
                }

                if(ParamCnt > ParamRel)
                {
                  if(Dump)
                  {
                    fprintf(L,"\nCurrent parameter count ( %u ) !",ParamCnt);
                    fprintf(L,"\nParameter count is between 1:%u !",ParamRel);
                  }

                  ParamCnt = ParamRel;
                }

                Cnt = 0;
                while(Cnt < ParamCnt)
                {
                  ValueName[Cnt] = (s8 *) malloc(OTHER_LEN * sizeof(u8));
                  FieldName[Cnt] = (s8 *) malloc(OTHER_LEN * sizeof(u8));
                  if((ValueName[Cnt] == NULL) || (FieldName[Cnt] == NULL))
                  {
                    if(Dump)
                    {
                      fprintf(L,"\nFailed to alloc RAM!");
                    }

                    Cnf = 0;
                    while(Cnf < Cnt)
                    {
                      free(ValueName[Cnf]);
                      free(FieldName[Cnf]);
                      Cnf++;
                    }

                    return EXIT_SUCCESS;
                  }

                  Cnt++;
                }

                if(Dump)
                {
                  fprintf(L,"\nMemory allocation success !\n");
                }

                while(Buff[i] != '(')
                {
                  TableName[t] = Buff[i];
                  t++;
                  i++;
                }

                TableName[t] = '\0';
                StrCppTrimBoth(TableName,' ');

                /* Use the table name as skipped columns list */
                if(strlen(FieldSkip) == 0)
                {
                  strncpy(FieldSkip,SETTING_FOLD,OTHER_LEN);
                  strncat(FieldSkip,"\\",OTHER_LEN-1);
                  strncat(FieldSkip,TableName,OTHER_LEN-1);
                }

                if(strlen(FieldSkip) > 0)
                {
                  S = fopen(strcat(FieldSkip, FTEXT_EXT),"rb");
                  if(S != NULL)
                  {
                    if(Dump)
                    {
                      fprintf(L,"\nSkipped columns [%s]...",FieldSkip);
                    }

                    i = 0;
                    while(!feof(S) && i <= OTHER_LEN)
                    {
                      fgets(FieldSkip,OTHER_LEN,S);
                      sSkip.setItem(FieldSkip);
                      if(Dump)
                      {
                        fprintf(L,"\nRead:%u<%s>",i,FieldSkip);
                        fprintf(L,"\nSkip:%u<%s>",sSkip.getSize(),sSkip.getTop());
                      }

                      i++;
                    }

                    i = 0;
                    fclose(S);
                  }
                  else
                  {
                    if(Dump)
                    {
                      fprintf(L,"\nSkipped missing [%s]...",FieldSkip);
                    }
                  }
                }
                else
                {
                  if(Dump)
                  {
                    fprintf(L,"\nTable unavailable!");
                  }

                  fprintf(O,"\nTable unavailable!");
                  fclose(O);
                  fclose(I);
                }

                if(Dump)
                {
                  fprintf(L,"\nTable: >%s<",TableName);
                }

                while(Buff[i] != '(')
                {
                  i++;
                }

                i++;
                Cnt = 0;
                while(Cnt < ParamCnt)
                {
                  memset(FieldName[Cnt],0,OTHER_LEN);
                  while(Buff[i] != ',' && Buff[i] != '\0')
                  {
                    FieldName[Cnt][f] = Buff[i];
                    f++;
                    i++;
                  }

                  FieldName[Cnt][f] = '\0';
                  StrCppTrimBoth(FieldName[Cnt],10);
                  StrCppTrimBoth(FieldName[Cnt],13);
                  StrCppTrimBoth(FieldName[Cnt],' ');
                  StrCppTrimBoth(FieldName[Cnt],',');
                  StrCppTrimBoth(FieldName[Cnt],')');
                  StrCppCapitalizeLatin(FieldName[Cnt]);
                  if(Dump)
                  {
                    fprintf(L,"\nField: >%s<",FieldName[Cnt]);
                  }

                  if(strlen(FieldName[Cnt]) >= LongFieldLen)
                  {
                    LongFieldLen = strlen(FieldName[Cnt]);
                  }

                  f = 0;
                  i++;
                  Cnt++;
                }

                bFirstInsert = 0x00;
                if(Dump)
                {
                  fprintf(L,"\nInitializing columns done !");
                }
              }

              if(Dump)
              {
                fprintf(L,"\nIndex = %u\nBuff: >>%s<<\n",i,Buff);
              }

              i = 12;

              /* Put the fields depending of the length ... */
              memcpy(&Buff[i],&TableName[0],strlen(TableName));
              i += strlen(TableName);
              Buff[i++] = '(';
              for(Cnt = 0; (Cnt < ParamCnt) && (i < BUFF_LEN); Cnt++)
              {
                sprintf(TEMPBUFF,"%c:%s",MATCH_INSERT,FieldName[Cnt]);
                StrCppCapitalizeLatin(TEMPBUFF);
                if(!sSkip.isHere(TEMPBUFF))
                {
                  memcpy(&Buff[i],FieldName[Cnt],strlen(FieldName[Cnt]));
                  i += strlen(FieldName[Cnt]);
                  Buff[i++] = ',';
                  Buff[i++] = ' ';
                }
              }
              Buff[i--] = '\0';
              Buff[i--] = '\n';
              Buff[i--] = ')';
              fputs(Buff,O);
              if(Deal)
              {
                printf("%c",MATCH_INSERT);
              }

              if(Dump)
              {
                fprintf(L,"\nWritten to file!");
              }

              i = 0;
            }
            else if(!strncmp(Buff,"values ",7))
            {
              FuncIndent = 0;
              StringIndent = 0;
              if(Dump)
              {
                fprintf(L,"\n\nGot -> Values");
              }

              memmove(Buff,"SELECT  ",8);
              Cnt = 0;
              i = 7;
              while(Cnt < ParamCnt)
              {
                memset(ValueName[Cnt],0,OTHER_LEN);
                if(Dump)
                {
                  fprintf(L,"\n>>%s<<",Buff);
                }

                while(Buff[i] != '\0')
                {
                  /* When string mode is activated it is processing a string */
                  if(Buff[i] == '\'' && !StringIndent)
                  {
                    StringIndent = 1;
                  }
                  else if(Buff[i] == '\'' && StringIndent)
                  {
                    StringIndent = 0;
                  }     /* Process functions when string mode is not active */

                  if(StringIndent == 0)
                  {     /* Function cannot begin or end while inside a string */
                    if(Buff[i] == '(' && (CharIsAlphaNum(Buff[i - 1]) || Buff[i - 1] == ' '))
                    {
                      FuncIndent++;
                    }
                    else if(Buff[i] == ')')
                    {
                      if(FuncIndent == 0)
                      { /* End of the line is terminated by a bracket when state is not a function */
                        Buff[i] = '\n';
                        Buff[i + 1] = '\0';
                        if(Dump) fprintf(L," -> End !");
                      }
                      else
                      { /* End is not reached yet and the current state is function */
                        FuncIndent--;
                      }
                    }
                  }

                  if(Dump)
                  {
                    fprintf
                    (
                      L,
                      "\nF[%u] S[%u] found >>%c%c<<",
                      FuncIndent,
                      StringIndent,
                      Buff[i - 1],
                      Buff[i]
                    );
                  }

                  if(Buff[i] == ',' && FuncIndent == 0 && StringIndent == 0)
                  {     /* Break the values on a comma. If is not a function argument or string */
                    if(Dump) fprintf(L," -> Break !");
                    break;
                  }

                  ValueName[Cnt][v] = Buff[i];
                  v++;
                  i++;
                }

                ValueName[Cnt][v] = '\0';
                StrCppTrimBoth(ValueName[Cnt],13);
                StrCppTrimBoth(ValueName[Cnt],10);
                StrCppTrimBoth(ValueName[Cnt],';');
                StrCppTrimBoth(ValueName[Cnt],',');
                StrCppTrimBoth(ValueName[Cnt],' ');
                if(Dump)
                {
                  fprintf(L,"\nValue: >%s<",ValueName[Cnt]);
                }

                v = 0;
                i++;
                Cnt++;
              }

              /* 01234567 Generating a Line >SELECT < */
              i = 7;
              for(Cnt = 0; (Cnt < ParamCnt) && (i < BUFF_LEN); Cnt++)
              {
                sprintf(TEMPBUFF,"%c:%s",MATCH_VALUES,FieldName[Cnt]);
                StrCppCapitalizeLatin(TEMPBUFF);
                if(!sSkip.isHere(TEMPBUFF))
                {
                  memcpy(&Buff[i],ValueName[Cnt],strlen(ValueName[Cnt]));
                  i += strlen(ValueName[Cnt]);
                  Buff[i++] = ',';
                  Buff[i++] = ' ';
                }
              }

              Buff[i--] = '\0';
              Buff[i--] = '\0';
              Buff[i--] = '\n';
              fputs(Buff,O);
              if(Dump)
              {
                fprintf(L,"\n\nBuff: >>%s<<\n",Buff);
                fprintf(L,"\nWritten to file !!");
              }

              if(Deal)
              {
                printf("%c",MATCH_VALUES);
              }

              fprintf(O,"FROM DUAL");
              fprintf(O,"\nWHERE NOT EXISTS ( SELECT 1");
              fprintf(O,"\n%s FROM %s",WHC_INDENT,TableName);
              Cnt = 0;
              while(Cnt < ParamCnt)
              {
                sprintf(TEMPBUFF,"%c:%s",MATCH_WHERE,FieldName[Cnt]);
                StrCppCapitalizeLatin(TEMPBUFF);
                bIsNull = StrCppIsNullSQL(ValueName[Cnt]);
                if(!sSkip.isHere(TEMPBUFF) && !(SkipNulls && bIsNull))
                {
                  memset(Buff,0,BUFF_LEN);
                  FieldLen = 1 + LongFieldLen - strlen(FieldName[Cnt]);
                  if(bIsNull)
                  {
                    memcpy(&Buff[0],"IS NULL",7);
                    Buff[7] = '\0';
                  }
                  else
                  {
                    memcpy(&Buff[0],"= ",2);
                    memcpy(&Buff[2],ValueName[Cnt],strlen(ValueName[Cnt]));
                    Buff[strlen(ValueName[Cnt]) + 2] = '\0';
                  }

                  if(Cnt == 0)
                  {
                    fprintf(O,"\n%sWHERE %s",WHC_INDENT,FieldName[Cnt]);
                  }
                  else
                  {
                    fprintf(O,"\n%s  AND %s",WHC_INDENT,FieldName[Cnt]);
                  }

                  while(FieldLen--)
                  {
                    fprintf(O," ");
                  }

                  fprintf(O,"%s",Buff);
                }

                Cnt++;
              }

              fprintf(O," );\n\n");
              if(Dump)
              {
                fprintf(L,"\nSQL Query Exported !");
              }
            }
            else
            {
              if(Dump)
              {
                StrCppTrimBoth(Buff,10);
                StrCppTrimBoth(Buff,13);
                fprintf(L,"\nSkipped line: <%s>",Buff);
              }

              if(Deal)
              {
                printf("%c",MATCH_NOITEM);
              }
            }
          }

          Cnf = 0;
          while(Cnf < ParamCnt)
          {
            if(ValueName[Cnf] != NULL)
            {
              free(ValueName[Cnf]);
            }

            if(FieldName[Cnf] != NULL)
            {
              free(FieldName[Cnf]);
            }

            Cnf++;
          }

          if(Dump)
          {
            fprintf(L,"\nMemory freed !");
          }
        }
        else
        {
          printf("\nFailed to open parameters!!");
          printf("\nGestating capsule ...");
          P = fopen(TEMPBUFF,"wb");
          if(P != NULL)
          {
            fprintf(P,"MaxColumnID:999\n");
            fprintf(P,"SysLogging:1\n");
            fprintf(P,"ReadDelay:15\n");
            fprintf(P,"SkipNulls:0\n");
            fprintf(P,"FieldSkip:\n");
            fprintf(O,"Parameters capsule have been gestated!\n");
            fprintf(O,"Location reference: %s\\%s%s\n\n",SETTING_FOLD,PARAM_NAME,FTEXT_EXT);
            fprintf(O,"Drag the input file below over the executable!\n");
            fprintf(O,"Inpit: %s\n",argv[1]);

            fclose(P);
            fclose(I);
            fclose(O);
          }

          sleep(GESTATING_TM);
          return EXIT_SUCCESS;
        }
      }
      else
      {
        printf("\nFailed to open output!!");
      }
    }
    else
    {
      printf("\nFailed to open input!!");
    }

    fclose(I);
    fclose(O);
    if(Dump)
    {
      fprintf(L,"\nFiles Closed !\n");
      fclose(L);
    }

    printf("\nFiles closed ! \nQueries exported, press a key to exit the app !");
  }
  else
  {
    printf
    (
      "\nI can't eat such <*%s> query format it makes my tummy feel bad !",
      (char *) (&argv[1][strlen(argv[1]) - 4])
    );
  }

  printf("\n");
  system("PAUSE");
  return EXIT_SUCCESS;
}
