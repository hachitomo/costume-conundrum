#include "image_internal.h"

/* Measure.
 */
 
int image_measure(int *w,int *h,const void *src,int srcc) {
  if (!w||!h) return -1;
  if (!src||(srcc<0)) return -1;
  #define _(tag) if (image_##tag##_measure(w,h,src,srcc)>=0) { \
    if ((*w<1)||(*w>IMAGE_SIZE_LIMIT)||(*h<1)||(*h>IMAGE_SIZE_LIMIT)) return -1; \
    return 0; \
  }
  IMAGE_FORMAT_FOR_EACH
  #undef _
  return -1;
}

/* Decode.
 */
 
int image_decode(void *dst,int dsta,const void *src,int srcc) {
  if (!dst||(dsta<0)) return -1;
  if (!src||(srcc<0)) return -1;
  #define _(tag) if (image_##tag##_decode(dst,dsta,src,srcc)>=0) return 0;
  IMAGE_FORMAT_FOR_EACH
  #undef _
  return -1;
}
