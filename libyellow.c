/**
  A simple library for triggering a canary token.
  By @singe

  Compile it as a shared library:
    gcc -shared -fPIC libyellow.c canary32.c -o libyellow.so

  Setup:
    Register your own DNS canary token from Thinkst at https://canarytokens.org/
    and put it in the TOKEN environment variable.

    Put the library somewhere you're comfortable with, and put it in
    LD_PRELOAD. For example:
      cp libyellow.so /usr/lib
      echo /usr/lib/libyellow.so >> /etc/ld.so.preload

  Usage:
    If anyone executes one of your binaries, you'll get a Canary notification.
**/

#include "canary32.h"


//int main_wrapper(int argc, char *argv[], char *env[])
int main_wrapper()
{
  FILE *fp;
  fp=fopen("/proc/self/cmdline","r");
  if (fp) {
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    getline(&line, &linecap, fp);
    yellow(line);
  }
  return 0;
}

__attribute__((section(".init_array"))) static void *p_constructor = &main_wrapper;
