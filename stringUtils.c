#include "stringUtils.h"

static char mdechex[17] = "0123456789ABCDEF";

int dec2hex (unsigned int dec, char *hex, int fill) {
	if (hex == NULL) {
    return -1;
	}
  int size = 0;
  int bakdec = dec;
	for (;;) {
    size++;
		dec >>= 4;
		if (dec == 0) {
			break;
		}
	}
  dec = bakdec;
  int i = 0;
  int s = fill-size;
	for (;;) {
    hex[s+size-i-1] = mdechex[dec & 0xF];
    i++;
		dec >>= 4;
		if (dec == 0) {
			break;
		}
	}
  for (i = 0; i < fill-size; ++i) {
    hex[i] = '0';
  }
  hex[fill] = '\0';
  return size;
}
