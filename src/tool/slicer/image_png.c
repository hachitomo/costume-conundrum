/* image_png.c
 * Quickie PNG decoder.
 * We do not support interlacing.
 * We force output to RGBA regardless of encoded format.
 * Not currently supporting tRNS for colortypes other than Indexed, tho we easily could.
 */

#include "image_internal.h"
#include <zlib.h>

/* Measure.
 */

int image_png_measure(int *w,int *h,const uint8_t *src,int srcc) {
  if ((srcc<8)||memcmp(src,"\x89PNG\r\n\x1a\n",8)) return -1;
  int srcp=8;
  for (;;) {
    if (srcp>srcc-8) break;
    int chunklen=(src[srcp]<<24)|(src[srcp+1]<<16)|(src[srcp+2]<<8)|src[srcp+3];
    srcp+=4;
    const uint8_t *chunkid=src+srcp;
    srcp+=4;
    if ((chunklen<0)||(srcp>srcc-chunklen)) break;
    if (!memcmp(chunkid,"IHDR",4)) {
      if (chunklen<8) break;
      *w=(src[srcp]<<24)|(src[srcp+1]<<16)|(src[srcp+2]<<8)|src[srcp+3];
      *h=(src[srcp+4]<<24)|(src[srcp+5]<<16)|(src[srcp+6]<<8)|src[srcp+7];
      return 0;
    }
    srcp+=chunklen;
    srcp+=4;
  }
  return -1;
}

/* Structured decoder.
 */
 
struct png_decoder {
  const uint8_t *src;
  int srcc;
  int srcp;
  
  // Special chunks located before decoding pixels. Must appear before the first IDAT.
  const uint8_t *ihdr; int ihdrc;
  const uint8_t *plte; int pltec;
  const uint8_t *trns; int trnsc;
  
  // From IHDR. We validate but don't record (compression,filter,interlace).
  int w,h;
  uint8_t depth,colortype;
  
  int chanc; // 1,2,3,4, from (colortype)
  int pixelsize; // bits, (chanc*depth). 1,2,4,8,16,24,32,48,64
  int xstride; // bytes, distance for filtering.
  int stride; // bytes, for the natural image
  int stride32; // bytes, for the RGBA output, always exactly (w*4).
  uint8_t *row,*prv; // Row buffers. Length of both is (1+stride).
  void (*cvtrow)(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder);
  
  uint8_t *dst; // Provided by caller.
  int dsta; // ''
  int dstp;
  int dsty;
  
  z_stream z;
  int zinit;
};

static void png_decoder_cleanup(struct png_decoder *decoder) {
  if (decoder->row) free(decoder->row);
  if (decoder->prv) free(decoder->prv);
  if (decoder->zinit) inflateEnd(&decoder->z);
}

/* Read and skip next chunk from the decoder.
 * Doesn't change (decoder) except (srcp).
 * Doesn't validate CRCs.
 * Fails at EOF.
 */
 
struct png_chunk {
  const uint8_t *id;
  const uint8_t *v;
  int c;
};

static int png_decode_chunk(struct png_chunk *chunk,struct png_decoder *decoder) {
  if (decoder->srcp>decoder->srcc-8) return -1;
  const uint8_t *v=decoder->src+decoder->srcp;
  chunk->c=(v[0]<<24)|(v[1]<<16)|(v[2]<<8)|v[3];
  chunk->id=v+4;
  decoder->srcp+=8;
  if (chunk->c<0) return -1;
  if (decoder->srcp>decoder->srcc-4-chunk->c) return -1;
  chunk->v=v+8;
  decoder->srcp+=chunk->c;
  decoder->srcp+=4;
  return 0;
}

/* Row converters.
 */
 
static void png_cvtrow_y1(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  uint8_t mask=0x80;
  int xi=decoder->w;
  for (;xi-->0;dst+=4) {
    if ((*src)&mask) memset(dst,0xff,4);
    else memcpy(dst,"\0\0\0\xff",4);
    if (mask==1) { mask=0x80; src++; }
    else mask>>=1;
  }
}

static void png_cvtrow_y2(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  int shift=6;
  int xi=decoder->w;
  for (;xi-->0;dst+=4) {
    int y=((*src)>>shift)&3;
    if (shift) shift-=2;
    else { src++; shift=6; }
    y|=y<<2;
    y|=y<<4;
    dst[0]=dst[1]=dst[2]=y;
    dst[3]=0xff;
  }
}

static void png_cvtrow_y4(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  int shift=4;
  int xi=decoder->w;
  for (;xi-->0;dst+=4) {
    int y=((*src)>>shift)&15;
    if (shift) shift=0;
    else { src++; shift=4; }
    y|=y<<4;
    dst[0]=dst[1]=dst[2]=y;
    dst[3]=0xff;
  }
}

static void png_cvtrow_y8(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  int xi=decoder->w;
  for (;xi-->0;dst+=4,src++) {
    dst[0]=dst[1]=dst[2]=*src;
    dst[3]=0xff;
  }
}

static void png_cvtrow_y16(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  int xi=decoder->w;
  for (;xi-->0;dst+=4,src+=2) {
    dst[0]=dst[1]=dst[2]=*src;
    dst[3]=0xff;
  }
}

static void png_cvtrow_rgb8(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  int xi=decoder->w;
  for (;xi-->0;dst+=4,src+=3) {
    dst[0]=src[0];
    dst[1]=src[1];
    dst[2]=src[2];
    dst[3]=0xff;
  }
}

static void png_cvtrow_rgb16(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  int xi=decoder->w;
  for (;xi-->0;dst+=4,src+=6) {
    dst[0]=src[0];
    dst[1]=src[2];
    dst[2]=src[3];
    dst[3]=0xff;
  }
}

static void png_cvtrow_i1(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  uint8_t mask=0x80;
  int xi=decoder->w;
  for (;xi-->0;dst+=4) {
    if ((*src)&mask) {
      if (decoder->pltec>=6) memcpy(dst,decoder->plte+3,3);
      else memset(dst,0xff,3);
      if (decoder->trnsc>=2) dst[3]=decoder->trns[1];
      else dst[3]=0xff;
    } else {
      if (decoder->pltec>=3) memcpy(dst,decoder->plte,3);
      else memset(dst,0,3);
      if (decoder->trnsc>=1) dst[3]=decoder->trns[0];
      else dst[3]=0xff;
    }
    if (mask==1) { mask=0x80; src++; }
    else mask>>=1;
  }
}

static void png_cvtrow_i2(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  int shift=6;
  int xi=decoder->w;
  for (;xi-->0;dst+=4) {
    int i=((*src)>>shift)&3;
    if (shift) shift-=2;
    else { src++; shift=6; }
    int p=i*3;
    if (p>decoder->pltec-3) {
      dst[0]=dst[1]=dst[2]=i|(i<<2)|(i<<4)|(i<<6);
    } else {
      memcpy(dst,decoder->plte+p,3);
    }
    if (i<decoder->trnsc) dst[3]=decoder->trns[i];
    else dst[3]=0xff;
  }
}

static void png_cvtrow_i4(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  int shift=4;
  int xi=decoder->w;
  for (;xi-->0;dst+=4) {
    int i=((*src)>>shift)&15;
    if (shift) shift=0;
    else { src++; shift=4; }
    int p=i*3;
    if (p>decoder->pltec-3) {
      dst[0]=dst[1]=dst[2]=i|(i<<4);
    } else {
      memcpy(dst,decoder->plte+p,3);
    }
    if (i<decoder->trnsc) dst[3]=decoder->trns[i];
    else dst[3]=0xff;
  }
}

static void png_cvtrow_i8(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  int xi=decoder->w;
  for (;xi-->0;dst+=4,src++) {
    int i=*src;
    int p=i*3;
    if (p>decoder->pltec-3) {
      dst[0]=dst[1]=dst[2]=i;
    } else {
      memcpy(dst,decoder->plte+p,3);
    }
    if (i<decoder->trnsc) dst[3]=decoder->trns[i];
    else dst[3]=0xff;
  }
}

static void png_cvtrow_ya8(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  int xi=decoder->w;
  for (;xi-->0;dst+=4,src+=2) {
    dst[0]=dst[1]=dst[2]=src[0];
    dst[3]=src[1];
  }
}

static void png_cvtrow_ya16(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  int xi=decoder->w;
  for (;xi-->0;dst+=4,src+=4) {
    dst[0]=dst[1]=dst[2]=src[0];
    dst[3]=src[2];
  }
}

static void png_cvtrow_rgba8(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  memcpy(dst,src,decoder->stride);
}

static void png_cvtrow_rgba16(uint8_t *dst,const uint8_t *src,struct png_decoder *decoder) {
  int xi=decoder->w;
  for (;xi-->0;dst+=4,src+=8) {
    dst[0]=src[0];
    dst[1]=src[2];
    dst[2]=src[4];
    dst[3]=src[6];
  }
}

/* Set (decoder->rowcvt) or fail.
 */
 
static int png_select_row_converter(struct png_decoder *decoder) {
  switch (decoder->colortype) {
    case 0: switch (decoder->depth) {
        case 1: decoder->cvtrow=png_cvtrow_y1; return 0;
        case 2: decoder->cvtrow=png_cvtrow_y2; return 0;
        case 4: decoder->cvtrow=png_cvtrow_y4; return 0;
        case 8: decoder->cvtrow=png_cvtrow_y8; return 0;
        case 16: decoder->cvtrow=png_cvtrow_y16; return 0;
      } break;
    case 2: switch (decoder->depth) {
        case 8: decoder->cvtrow=png_cvtrow_rgb8; return 0;
        case 16: decoder->cvtrow=png_cvtrow_rgb16; return 0;
      } break;
    case 3: switch (decoder->depth) {
        // 'i' degrades gracefully into 'y'. If the PLTE was missing altogether (a spec violation), eliminate the middle man.
        case 1: decoder->cvtrow=decoder->plte?png_cvtrow_i1:png_cvtrow_y1; return 0;
        case 2: decoder->cvtrow=decoder->plte?png_cvtrow_i2:png_cvtrow_y2; return 0;
        case 4: decoder->cvtrow=decoder->plte?png_cvtrow_i4:png_cvtrow_y4; return 0;
        case 8: decoder->cvtrow=decoder->plte?png_cvtrow_i8:png_cvtrow_y8; return 0;
      } break;
    case 4: switch (decoder->depth) {
        case 8: decoder->cvtrow=png_cvtrow_ya8; return 0;
        case 16: decoder->cvtrow=png_cvtrow_ya16; return 0;
      } break;
    case 6: switch (decoder->depth) {
        case 8: decoder->cvtrow=png_cvtrow_rgba8; return 0;
        case 16: decoder->cvtrow=png_cvtrow_rgba16; return 0;
      } break;
  }
  return -1;
}

/* Unfilter one row in place.
 */
 
static void png_unfilter_SUB(uint8_t *row,const uint8_t *prv,int c,int xstride) {
  row+=xstride;
  c-=xstride;
  for (;c-->0;row++) (*row)+=row[-xstride];
}

static void png_unfilter_UP(uint8_t *row,const uint8_t *prv,int c,int xstride) {
  for (;c-->0;row++,prv++) (*row)+=(*prv);
}

static void png_unfilter_AVG(uint8_t *row,const uint8_t *prv,int c,int xstride) {
  int i=xstride;
  for (;i-->0;row++,prv++) (*row)+=(*prv)>>1;
  c-=xstride;
  for (;c-->0;row++,prv++) (*row)+=((*prv)+row[-xstride])>>1;
}

static uint8_t png_paeth(uint8_t a,uint8_t b,uint8_t c) {
  int p=a+b-c;
  int pa=a-p; if (pa<0) pa=-pa;
  int pb=b-p; if (pb<0) pb=-pb;
  int pc=c-p; if (pc<0) pc=-pc;
  if ((pa<=pb)&&(pa<=pc)) return a;
  if (pb<=pc) return b;
  return c;
}

static void png_unfilter_PAETH(uint8_t *row,const uint8_t *prv,int c,int xstride) {
  int i=xstride;
  for (;i-->0;row++,prv++) (*row)+=(*prv);
  c-=xstride;
  for (;c-->0;row++,prv++) (*row)+=png_paeth(row[-xstride],prv[0],prv[-xstride]);
}

/* Apply filter, convert, copy to image, and swap row buffers.
 */
 
static int png_receive_row(struct png_decoder *decoder) {
  if (decoder->dsty>=decoder->h) return 0;
  switch (decoder->row[0]) {
    case 0: break;
    case 1: png_unfilter_SUB(decoder->row+1,decoder->prv+1,decoder->stride,decoder->xstride); break;
    case 2: png_unfilter_UP(decoder->row+1,decoder->prv+1,decoder->stride,decoder->xstride); break;
    case 3: png_unfilter_AVG(decoder->row+1,decoder->prv+1,decoder->stride,decoder->xstride); break;
    case 4: png_unfilter_PAETH(decoder->row+1,decoder->prv+1,decoder->stride,decoder->xstride); break;
    default: return -1;
  }
  decoder->cvtrow(decoder->dst+decoder->dstp,decoder->row+1,decoder);
  decoder->dstp+=decoder->stride32;
  decoder->dsty++;
  void *tmp=decoder->row;
  decoder->row=decoder->prv;
  decoder->prv=tmp;
  return 0;
}

/* Decode one IDAT chunk.
 */
 
static int png_decode_idat(struct png_decoder *decoder,const uint8_t *src,int srcc) {
  decoder->z.next_in=(Bytef*)src;
  decoder->z.avail_in=srcc;
  while (decoder->z.avail_in>0) {
    if (decoder->z.avail_out<=0) {
      if (png_receive_row(decoder)<0) return -1;
      decoder->z.next_out=(Bytef*)decoder->row;
      decoder->z.avail_out=1+decoder->stride;
    }
    if (inflate(&decoder->z,Z_NO_FLUSH)<0) return -1;
  }
  return 0;
}

/* End of file. Drain decompressor.
 */
 
static int png_decode_drain(struct png_decoder *decoder) {
  while (decoder->dsty<decoder->h) {
    if (decoder->z.avail_out<=0) {
      if (png_receive_row(decoder)<0) return -1;
      decoder->z.next_out=(Bytef*)decoder->row;
      decoder->z.avail_out=1+decoder->stride;
    }
    int err=inflate(&decoder->z,Z_FINISH);
    if (err==Z_STREAM_END) break;
    if (err<0) return -1;
  }
  return 0;
}

/* Decode, main.
 */
 
static int png_decode_inner(struct png_decoder *decoder) {

  // Validate signature.
  if (decoder->srcc<8) return -1;
  if (memcmp(decoder->src,"\x89PNG\r\n\x1a\n",8)) return -1;
  decoder->srcp=8;
  
  // Locate pre-pixel chunks.
  for (;;) {
    int srcp0=decoder->srcp;
    struct png_chunk chunk={0};
    if (png_decode_chunk(&chunk,decoder)<0) break;
    if (!memcmp(chunk.id,"IDAT",4)) {
      decoder->srcp=srcp0; // unread
      break;
    }
    if (!decoder->ihdr&&!memcmp(chunk.id,"IHDR",4)) {
      decoder->ihdr=chunk.v;
      decoder->ihdrc=chunk.c;
    } else if (!decoder->plte&&!memcmp(chunk.id,"PLTE",4)) {
      decoder->plte=chunk.v;
      decoder->pltec=chunk.c;
    } else if (!decoder->trns&&!memcmp(chunk.id,"tRNS",4)) {
      decoder->trns=chunk.v;
      decoder->trnsc=chunk.c;
    }
    if (!memcmp(chunk.id,"IEND",4)) break;
  }
  
  // Decode IHDR, validate, measure, etc.
  if (decoder->ihdrc<13) return -1;
  decoder->w=(decoder->ihdr[0]<<24)|(decoder->ihdr[1]<<16)|(decoder->ihdr[2]<<8)|decoder->ihdr[3];
  decoder->h=(decoder->ihdr[4]<<24)|(decoder->ihdr[5]<<16)|(decoder->ihdr[6]<<8)|decoder->ihdr[7];
  decoder->depth=decoder->ihdr[8];
  decoder->colortype=decoder->ihdr[9];
  if (decoder->ihdr[10]||decoder->ihdr[11]||decoder->ihdr[12]) {
    // [12]==1 means Adam7 interlacing, which is mandated by the spec, but we're not going to support it.
    // [10] and [11] are only allowed to be zero.
    return -1;
  }
  if ((decoder->w<1)||(decoder->w>IMAGE_SIZE_LIMIT)) return -1;
  if ((decoder->h<1)||(decoder->h>IMAGE_SIZE_LIMIT)) return -1;
  switch (decoder->colortype) {
    case 0: decoder->chanc=1; break;
    case 2: decoder->chanc=3; break;
    case 3: decoder->chanc=1; break;
    case 4: decoder->chanc=2; break;
    case 6: decoder->chanc=4; break;
    default: return -1;
  }
  decoder->pixelsize=decoder->depth*decoder->chanc;
  switch (decoder->pixelsize) {
    case 1: case 2: case 4: case 8: case 16: case 24: case 32: case 48: case 64: break;
    default: return -1;
  }
  if (!(decoder->xstride=decoder->pixelsize>>3)) decoder->xstride=1;
  decoder->stride=(decoder->pixelsize*decoder->w+7)>>3;
  decoder->stride32=decoder->w<<2;
  int dstc=decoder->stride32*decoder->h;
  if (dstc>decoder->dsta) return -1;
  memset(decoder->dst,0,dstc);
  decoder->dstp=0;
  
  // Select row converter.
  if (png_select_row_converter(decoder)<0) return -1;
  
  // Allocate row buffers.
  if (!(decoder->row=calloc(1,1+decoder->stride))) return -1;
  if (!(decoder->prv=calloc(1,1+decoder->stride))) return -1;
  
  // Create zlib context.
  if (inflateInit(&decoder->z)<0) return -1;
  decoder->zinit=1;
  decoder->z.next_out=(Bytef*)decoder->row;
  decoder->z.avail_out=1+decoder->stride;
  
  // Proceed thru the file, processing each IDAT as we find them.
  for (;;) {
    struct png_chunk chunk={0};
    if (png_decode_chunk(&chunk,decoder)<0) break;
    if (!memcmp(chunk.id,"IEND",4)) break;
    if (memcmp(chunk.id,"IDAT",4)) continue;
    if (png_decode_idat(decoder,chunk.v,chunk.c)<0) return -1;
  }
  if (png_decode_drain(decoder)<0) return -1;
  
  // We've zeroed the pixels initially. Don't call short data an error (even though the spec does, if memory serves).
  return dstc;
}
 
int image_png_decode(void *dst,int dsta,const uint8_t *src,int srcc) {
  struct png_decoder decoder={.src=src,.srcc=srcc,.dst=dst,.dsta=dsta};
  int err=png_decode_inner(&decoder);
  png_decoder_cleanup(&decoder);
  return err;
}
