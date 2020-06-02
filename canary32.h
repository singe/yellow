#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_HOSTNAME 254
#define BASE32_INPUT 5

size_t cyoBase32EncodeA(char* dest, const void* src, size_t srcBytes);
