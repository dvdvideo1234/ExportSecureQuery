#ifndef SKIP_H_
  #include "mytypes.h"
  #include "mydefine.h"
  #define SKIP_H_

  typedef class Skip
  {
    private:
      u32 Size;
      s8 Data[SKIP_SIZE][OTHER_LEN];
    public:
      u32 getSize(){ return Size; }
      u8 isHere(s8* Str);
      u8 isHere(s8* Str, u8 Ch);
      s8 *getItem(u32 iD);
      s8 *getTop();
      class Skip &setItem(s8 *Str);
      Skip();
     ~Skip(){};
  } cSkip;

  Skip::Skip()
  {
    Size = 0;
  }

  u8 Skip::isHere(s8* Str)
  {
    u32 uI;
    u8 uR = 0x00;
    for(uI = 0; uI < SKIP_SIZE; uI++)
    {
      if(strcmp(Data[uI], Str) == 0)
      {
        uR = 0xFF;
        break;
      }
    }
    return uR;
  }

  cSkip &Skip::setItem(s8 *Str)
  {
    if(Str != NULL)
    {
      memcpy(Data[Size], Str, OTHER_LEN);
      StrCppTrimBoth(Data[Size], 10);
      StrCppTrimBoth(Data[Size], 13);
      StrCppTrimBoth(Data[Size], ' ');
      if(strlen(Data[Size]) > 0)
      {
        if(Size >= 0 && Size < SKIP_SIZE)
        {
          StrCppCapitalizeLatin(Data[Size++]);
        }
        else
        {
          printf("\nSkip container is FULL!\n");
        }
      }
    }
    return *this;
  }

  s8 *cSkip::getItem(u32 iD)
  {
    if(iD >= 0 && iD <= SKIP_SIZE)
    {
      return Data[iD];
    }
    return NULL;
  }

  s8 *cSkip::getTop()
  {
    return getItem(Size-1);
  }
#endif
