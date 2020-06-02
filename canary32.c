#include "canary32.h"

// Modified from https://sourceforge.net/projects/cyoencode/

static const char* const BASE32_TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=";

size_t cyoBase32EncodeA(char* dest, const void* src, size_t srcBytes)
{
  if (srcBytes > 153)
    return 0; // Hostname max is 253, which means input can't be more than 153 I think
  if (dest && src)
  {
    unsigned char* pSrc = (unsigned char*)src;
    char* pDest = dest;
    size_t dwSrcSize = srcBytes;
    size_t dwDestSize = 0;
    size_t dwBlockSize;
    unsigned char n[8];

    while (dwSrcSize >= 1)
    {
      /* Encode inputs */
      dwBlockSize = (dwSrcSize < BASE32_INPUT ? dwSrcSize : BASE32_INPUT);
      n[0] = n[1] = n[2] = n[3] = n[4] = n[5] = n[6] = n[7] = 0;
      switch (dwBlockSize)
      {
      case 5:
        n[7] = (pSrc[4] & 0x1f);
        n[6] = ((pSrc[4] & 0xe0) >> 5);
      case 4:
        n[6] |= ((pSrc[3] & 0x03) << 3);
        n[5] = ((pSrc[3] & 0x7c) >> 2);
        n[4] = ((pSrc[3] & 0x80) >> 7);
      case 3:
        n[4] |= ((pSrc[2] & 0x0f) << 1);
        n[3] = ((pSrc[2] & 0xf0) >> 4);
      case 2:
        n[3] |= ((pSrc[1] & 0x01) << 4);
        n[2] = ((pSrc[1] & 0x3e) >> 1);
        n[1] = ((pSrc[1] & 0xc0) >> 6);
      case 1:
        n[1] |= ((pSrc[0] & 0x07) << 2);
        n[0] = ((pSrc[0] & 0xf8) >> 3);
        break;

      default:
        assert(0);
      }
      pSrc += dwBlockSize;
      dwSrcSize -= dwBlockSize;

      /* Validate */
      for (int i=0; i < 8; i++)
        assert(n[i] <= 31);

      /* Padding */
      switch (dwBlockSize)
      {
      case 1: n[2] = n[3] = 32;
      case 2: n[4] = 32;
      case 3: n[5] = n[6] = 32;
      case 4: n[7] = 32;
      case 5:
        break;

      default:
        assert(0);
      }

      /* 8 outputs */
      for (int i=0; i < 8; i++) {
        if (n[i] == 32) break;
        dwDestSize++;
        if (dwDestSize % 64 == 0) {
          *pDest++ = '.';
          dwDestSize++;
        }
        *pDest++ = BASE32_TABLE[n[i]];
      }

    }
    *pDest++ = '\x0'; /*append terminator*/
    //sranddev();
    //int i = snprintf(dest, MAX_HOSTNAME, "%s.G%02d", dest, rand()%99);
    srand(time(NULL));
    char foo[MAX_HOSTNAME];
    int i = snprintf(foo, MAX_HOSTNAME, "%s.G%02d", dest, rand()%99);
    i = snprintf(dest, MAX_HOSTNAME, "%s", foo);
    if (i >= MAX_HOSTNAME)
      return 0; //encoded value exceeds hostname max
    else
      return i;
  }
  else
    return 0; /*ERROR - null pointer*/
}
