/* image.h
 * General image decoder.
 */
 
#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

struct sr_encoder;

// Size limit in pixels per axis. Yields a maximum size of 64 MB.
#define IMAGE_SIZE_LIMIT 4096
 
int image_measure(int *w,int *h,const void *src,int srcc);
int image_decode(void *dst,int dsta,const void *src,int srcc);

/* We only produce PNG.
 * We examine the image in detail and aggressively optimize.
 * (dst) may be partially populated on errors.
 */
int image_encode(struct sr_encoder *dst,const void *rgba,int rgbac,int w,int h);

#endif
