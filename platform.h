#ifndef PLATFORM_H_
  #define PLATFORM_H_

  #ifdef _WIN32
      #include <windows.h>

      void sleep(unsigned milliseconds)
      {
          Sleep(milliseconds);
      }
  #else
      #include <unistd.h>

      void sleep(unsigned milliseconds)
      {
          usleep(milliseconds * 1000); // takes microseconds
      }
  #endif

#endif
