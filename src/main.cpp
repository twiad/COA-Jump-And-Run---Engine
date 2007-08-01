
#include "Dependencies.h"

#include "MainApplication.h"

#ifdef __APPLE__
  #undef main
#endif

int main(int p_argc, char const** p_argv)
{
    CoABlaster::MainApplication::go();
    return 0;
}

