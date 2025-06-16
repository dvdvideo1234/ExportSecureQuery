#ifndef BUFF_H_
  #include "mytypes.h"
  #include "mydefine.h"
  #define BUFF_H_

  typedef class Buff
  {
    private:
      u32 IDX; /// points to the zero character
      s8 Data[BUFF_LEN]; /// Storage container
    public:
      s8 *getPos(u32 iDX);
      s8 *getPos(s8* Str);
      class Buff &putChar(s8 Ch);
      class Buff &putStr(s8* Str);
      Buff();
     ~Buff(){};
  } cBuff;

  Buff::Buff()
  {
    IDX = 0;
    memset(Data, 0, BUFF_LEN);
  }

  s8 *Buff::getPos(u32 iDX)
  {
    if(iDX >= 0 && iDX <= BUFF_LEN)
    {
      return &(Data[IDX]);
    }
    return NULL;
  }

  s8 *Buff::getPos(s8* Str)
  {
    if(Str != NULL)
    {
      return strstr(Str, Data);
    }
    return NULL;
  }

  cBuff &Buff::putChar(s8 Ch)
  {
    Data[IDX++] = Ch;
    return *this;
  }

  cBuff &Buff::putStr(s8* Str)
  {
    if(Str != NULL)
    {
      u32 iL = (strlen(Str) + 1);
      memmove(&(Data[IDX]), Str, iL);
    }
    return *this;
  }
#endif
