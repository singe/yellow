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

#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include "canary32.h"

#define TOKEN "TOKEN"
#define FILENAMESIZE 500

int yellow(int argc, char **argv, char **env)
{
  char *token = getenv(TOKEN);
  if (token != NULL) {
    struct addrinfo *res;
    char hostname[MAX_HOSTNAME], fqdn[FILENAMESIZE];
    int len = cyoBase32EncodeA(hostname, argv[0], strlen(argv[0]));
    // Trigger our canary token
    if (len == 0)
      getaddrinfo(token,NULL,NULL,&res);
    else {
      snprintf(fqdn, FILENAMESIZE, "%s.%s", hostname, token);
      getaddrinfo(fqdn,NULL,NULL,&res);
    }
    freeaddrinfo(res);
  }
  return 0;
}

__attribute__((section(".init_array"))) static void *foo_constructor = &yellow;
