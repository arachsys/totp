#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

const char *base32 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

int main(void) {
  char *line, *secret;
  int64_t offset;
  size_t keysize, length, linesize;
  uint8_t digits, *hmac, *key, msg[8];
  uint32_t bits, code, count;
  uint64_t clock, interval;
  unsigned int hmacsize;
  const EVP_MD *digest;

  key = NULL, keysize = 0;
  line = NULL, linesize = 0;

  while (getline(&line, &linesize, stdin) >= 0) {
    secret = line + strspn(line, "\t ");

    if (!strncmp(secret, "sha1:", strlen("sha1:")))
      digest = EVP_sha1(), secret += strlen("sha1:");
    else if (!strncmp(secret, "sha256:", strlen("sha256:")))
      digest = EVP_sha256(), secret += strlen("sha256:");
    else if (!strncmp(secret, "sha512:", strlen("sha512:")))
      digest = EVP_sha512(), secret += strlen("sha512:");
    else
      digest = EVP_sha1();

    for (count = bits = length = 0; *secret; secret++) {
      if (!strchr(base32, *secret))
        break;
      bits = (bits << 5) | (strchr(base32, *secret) - base32);
      count += 5;

      while (count >= 8) {
        while (length >= keysize) {
          keysize = keysize ? keysize << 1 : 64;
          if (!(key = realloc(key, keysize)))
            return 1;
        }
        count -= 8, key[length++] = bits >> count;
      }
    }

    digits = *secret == ':' ? strtoul(secret + 1, &secret, 10) : 6;
    interval = *secret == ':' ? strtoull(secret + 1, &secret, 10) : 30;
    offset = *secret == ':' ? strtoll(secret + 1, &secret, 10) : 0;

    if (digits > 0 && interval > 0 && length > 0) {
      clock = (time(NULL) - offset) / interval;
      for (count = 0; count < 8; count++)
        msg[7 - count] = clock >> 8*count;
      hmac = HMAC(digest, key, length, msg, sizeof(msg), NULL, &hmacsize);

      for (code = count = 0; count < 4; count++)
        code += hmac[(hmac[hmacsize - 1] & 0x0f) + 3 - count] << 8*count;
      code &= 0x7fffffff;

      if (digits == 6)
        printf("%06u%s", code % 1000000, secret);
      else if (digits == 7)
        printf("%07u%s", code % 10000000, secret);
      else if (digits == 8)
        printf("%08u%s", code % 100000000, secret);
      fflush(stdout);
    }
  }

  free(key);
  free(line);
  return EXIT_SUCCESS;
}
