#ifndef DICER_INTERNAL_H
#define DICER_INTERNAL_H

#include "serial.h"
#include "image.h"
#include "fs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

extern struct dicer {
  const char *exename;
  const char *srcpath;
  const char *dstpath;
} dicer;

int dicer_compile_tilesheet(struct sr_encoder *dst,const void *src,int srcc);
int dicer_compile_decalsheet(struct sr_encoder *dst,const void *src,int srcc);
int dicer_compile_map(struct sr_encoder *dst,const void *src,int srcc);

int dicer_to_c(struct sr_encoder *dst,const void *src,int srcc);

#endif
