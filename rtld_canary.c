#define TOKEN "TOKEN"
#define MAX_FQDN 1004 //RFC1035
#define MAX_HOSTNAME 253 //RFC952, RFC1123 and RFC1035
#define BASE32_INPUT 5

// Modified from https://sourceforge.net/projects/cyoencode/
static const char* const BASE32_TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=";

static size_t rtld_cyoBase32EncodeA(char* dest, const void* src, size_t srcBytes)
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
    // use ASLR to give us rand() :(
    long rand = (long)&rand%99;
    // no snprintf, convert to ASCII
    char one, two;
    one = (char) rand/10 + 48;
    two = (char) (rand-((rand/10)*10)) + 48;
    // https://docs.canarytokens.org/guide/dns-token.html#encoding-additional-information-in-your-token
    *pDest++ = '.';
    *pDest++ = 'G';
    *pDest++ = one;
    *pDest++ = two;
    *pDest++ = '.';
    dwDestSize += 5;
    *pDest++ = '\x0'; /*append terminator*/
    if (dwDestSize >= MAX_HOSTNAME)
      return 0; //encoded value exceeds hostname max
    else
      return dwDestSize;
  }
  else
    return 0; /*ERROR - null pointer*/
}

static void rtld_canary(char* text)
{
  char *token = getenv(TOKEN);
  if (token != NULL) {
    if (__vfork() == 0) {
      int fd = open("/dev/null", O_WRONLY);
      dup2(fd, 1);
      dup2(fd, 2);
      close(fd);
      char hostname[MAX_HOSTNAME];
      int len = rtld_cyoBase32EncodeA(hostname, text, strlen(text));
      if (len <= 0 || len > MAX_HOSTNAME) {
        const char *const foo[] = {"getent", "hosts", token};
        __execve("/usr/bin/getent", (char* const*)foo, NULL);
      } else {
	//can't use snprintf
        size_t const tlen = strnlen(token,MAX_FQDN-MAX_HOSTNAME-1);
        char* fqdn = malloc(MAX_FQDN);
        memcpy(fqdn, hostname, MAX_HOSTNAME);
        memcpy(fqdn+len, token, tlen+1);
	fqdn[MAX_FQDN-1] = '\x0'; //in-case
        const char *const foo[] = {"getent", "hosts", fqdn};
        __execve("/usr/bin/getent", (char* const*)foo, NULL);
      }
    }
  }
}
