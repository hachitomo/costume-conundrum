#ifndef DICER_INTERNAL_H
#define DICER_INTERNAL_H

#include "serial.h"
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
  char objname[64];
  int objnamec;
  
  struct sym {
    char *k;
    int kc,v;
  } *symv;
  int symc,syma;
  int sym_loaded;
} dicer;

/* Compilers will typically produce binary data and return zero.
 * Optionally, they may compose the C text themselves and return >0.
 * decalsheet does this, and map probably will soon.
 */
int dicer_compile_tilesheet(struct sr_encoder *dst,const void *src,int srcc);
int dicer_compile_decalsheet(struct sr_encoder *dst,const void *src,int srcc);
int dicer_compile_map(struct sr_encoder *dst,const void *src,int srcc);

int dicer_to_c(struct sr_encoder *dst,const void *src,int srcc);

/* Look up any integer from shared_symbols.h
 * Integer values only.
 */
int sym_get(int *v,const char *pfx,int pfxc,const char *k,int kc);

#endif
