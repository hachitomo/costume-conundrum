#include "dicer_internal.h"

/* Compile decalsheet from text.
 */
 
int dicer_compile_decalsheet(struct sr_encoder *dst,const void *src,int srcc) {
  // We don't need these yet. When we do, we'll probably want to emit an organized model and not just raw serial data.
  // Whatever, do that when the need arises.
  // For now, it can't be empty, so emit a zero.
  return sr_encode_u8(dst,0);
}
