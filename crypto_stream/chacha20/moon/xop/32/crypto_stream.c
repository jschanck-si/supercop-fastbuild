#include "crypto_stream.h"
#include <stddef.h>

extern void chacha_xop(const unsigned char *k, const unsigned char *n, const unsigned char *in, unsigned char *out, size_t inlen, size_t rounds);

int crypto_stream_xor(unsigned char *out, const unsigned char *in, unsigned long long inlen, const unsigned char *n, const unsigned char *k) {
	chacha_xop(k, n, in, out, (size_t)inlen, 20);
}

int crypto_stream(unsigned char *out, unsigned long long outlen, const unsigned char *n, const unsigned char *k) {
	chacha_xop(k, n, NULL, out, (size_t)outlen, 20);
}
