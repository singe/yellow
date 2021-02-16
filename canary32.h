#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <netdb.h>

// Name of the environment variable containing the token
#define TOKEN "TOKEN"
#define MAX_FQDN 1004 //RFC1035
#define MAX_HOSTNAME 253 //RFC952, RFC1123 and RFC1035
#define BASE32_INPUT 5

int yellow(char* text);
size_t cyoBase32EncodeA(char* dest, const void* src, size_t srcBytes);
