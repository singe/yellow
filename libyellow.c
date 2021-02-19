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

    Edit the list of binary names you want to alert on under "alert_list".

  Usage:
    If anyone executes one of your binaries, you'll get a Canary notification.
**/

#include "canary32.h"
#define SSIZE_MAX = 64

int main_wrapper()
{
  int alert_size = 2;
  char *alert_list[] = {
    "id",
    "nc"
  };

  //read name of invoked command from proc
  //it isn't always being passed from init_array
  FILE *fp;
  fp=fopen("/proc/self/cmdline","r");
  if (fp) {
    char *line = NULL;
    size_t linecap = 0;
    getline(&line, &linecap, fp);

    for (int i=0; i < alert_size; i++) {
      if(strncmp(line, alert_list[i], 64) == 0 ) {
        yellow(line);
        break;
      }
    }
  }

  return 0;
}

__attribute__((section(".init_array"))) static void *p_constructor = &main_wrapper;
