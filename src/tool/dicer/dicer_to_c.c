#include "dicer_internal.h"

/* Wrap arbitrary data in a C file, in memory.
 */
 
int dicer_to_c(struct sr_encoder *dst,const void *src,int srcc) {
  
  sr_encode_fmt(dst,"const int %.*s_length=%d;\n",dicer.objnamec,dicer.objname,srcc);
  sr_encode_fmt(dst,"const unsigned char %.*s[]={\n",dicer.objnamec,dicer.objname);
  const uint8_t *SRC=src;
  int srcp=0;
  for (;srcp<srcc;srcp++) {
    sr_encode_fmt(dst,"%d,",SRC[srcp]);
    if (srcp%100==99) sr_encode_u8(dst,0x0a); // Newline every 100 bytes because long lines are ugly.
  }
  sr_encode_raw(dst,"};\n",3);
  
  return sr_encoder_assert(dst);
}
