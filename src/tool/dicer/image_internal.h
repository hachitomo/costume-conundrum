#ifndef IMAGE_INTERNAL_H
#define IMAGE_INTERNAL_H

#include "image.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/* List all possible formats here.
 * Then add a block below either declaring the 'measure' and 'decode' functions, or stubbing them to return <0.
 * We will only support formats that most web browsers do; nothing custom.
 * Maybe only PNG.
 */
#define IMAGE_FORMAT_FOR_EACH \
  _(png)

#define IMAGE_ENABLE_png 1
#if IMAGE_ENABLE_png
  int image_png_measure(int *w,int *h,const uint8_t *src,int srcc);
  int image_png_decode(void *dst,int dsta,const uint8_t *src,int srcc);
#else
  static inline int image_png_measure(int *w,int *h,const uint8_t *src,int srcc) { return -1; }
  static inline int image_png_decode(void *dst,int dsta,const uint8_t *src,int srcc) { return -1; }
#endif

#endif
