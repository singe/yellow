/**
  A simple binary wrapper for triggering a canary token.
  By @singe

  Setup:
    Register your own DNS canary token from Thinkst at https://canarytokens.org/
    and put it in the TOKEN environment variable.

    Move the legit binary to use the extension you defined below, then either
    copy this binary to that or use a symlink. For example:
      cp yellow /usr/bin
      cd /usr/bin
      mv id id.canary
      ln -s yellow id

  Compiling:
    gcc -o yellow yellow.c canary32.c

  Usage:
    If anyone executes one of your binaries, you'll get a Canary notification.
**/

#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "canary32.h"

// Name of the environment variable with the token
#define TOKEN "TOKEN"

// Use whatever extension you like
#define EXTENSION ".canary"

// 200 bytes should be enough for anyone - Not Bill Gates
#define FILENAMESIZE 500

int main(int argc, char *argv[], char *environ[])
{
  char *token = getenv(TOKEN);
  if (token != NULL) {
    struct addrinfo *res;
    char hostname[MAX_HOSTNAME], fqdn[FILENAMESIZE];
    int len = cyoBase32EncodeA(hostname, argv[0], strlen(argv[0])); 
    // Trigger our canary token
    if (len == 0)
      getaddrinfo(token,NULL,NULL,&res);
      //printf("%s\n",token);
    else {
      snprintf(fqdn, FILENAMESIZE, "%s.%s", hostname, token);
      getaddrinfo(fqdn,NULL,NULL,&res);
      //printf("%s\n",fqdn);
    }
    freeaddrinfo(res);
  }

  // Append our extension to the called filename to get the real one
  char realexe[FILENAMESIZE];
  snprintf(realexe, FILENAMESIZE, "%s%s", argv[0], EXTENSION);

  // PATH lookup the exec and replace this process with it
  // Preserving the passed args and environment
  execvp(realexe, argv);

  return 0;
}
