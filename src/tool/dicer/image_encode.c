/* image_encode.c
 * PNG encoder.
 * Input is always 32-bit RGBA. We'll select an appropriate output format.
 */
 
#include "image_internal.h"
#include "serial.h"
#include <zlib.h>

/* Filters.
 * (dst,src,prv) all have length (stride).
 * You must not call with (filterid>1) if (prv) null.
 */
 
static int image_paeth(int a,int b,int c) {
  int p=(a+b)-c;
  int pa=p-a; if (pa<0) pa=-pa;
  int pb=p-b; if (pb<0) pb=-pb;
  int pc=p-c; if (pc<0) pc=-pc;
  if ((pa<=pb)&&(pa<=pc)) return a;
  if (pb<=pc) return b;
  return c;
}
 
static void image_perform_filter(int filterid,uint8_t *dst,const uint8_t *src,const uint8_t *prv,int stride,int xstride) {
  switch (filterid) {
  
    case 0: { // NONE
        memcpy(dst,src,stride);
      } break;
      
    case 1: { // SUB
        memcpy(dst,src,xstride);
        int i=xstride;
        for (;i<stride;i++) dst[i]=src[i]-src[i-xstride];
      } break;
      
    case 2: { // UP
        int i=0;
        for (;i<stride;i++) dst[i]=src[i]-prv[i];
      } break;
      
    case 3: { // AVG
        int i=0;
        for (;i<xstride;i++) dst[i]=src[i]-(prv[i]>>1);
        for (;i<stride;i++) dst[i]=src[i]-((prv[i]+src[i-xstride])>>1);
      } break;
      
    case 4: { // PAETH
        int i=0;
        for (;i<xstride;i++) dst[i]=src[i]-prv[i];
        for (;i<stride;i++) dst[i]=src[i]-image_paeth(src[i-xstride],prv[i],prv[i-xstride]);
      } break;
  }
}

/* Filter one row of raw image.
 * (prv) may be null.
 * Otherwise length of (src) and (prv) is (stride).
 * Length of (dst) is (1+stride).
 */
 
static int image_count_zeroes(const uint8_t *v,int c) {
  int zeroc=0;
  for (;c-->0;v++) if (!*v) zeroc++;
  return zeroc;
}
 
static void image_filter_row(uint8_t *dst,const uint8_t *src,const uint8_t *prv,int stride,int xstride) {

  // No previous row, use NONE or SUB. Otherwise, all 5 filters are available.
  // As luck would have it, NONE and SUB are the first two filters. So we only need to record how many.
  int optionc=prv?5:2;
  
  // Try each filter, and record our favorite.
  // We'll use a simple heuristic: More zeroes is better.
  int bestid=0,bestscore=0,i=0;
  for (;i<optionc;i++) {
    image_perform_filter(i,dst+1,src,prv,stride,xstride);
    int zeroc=image_count_zeroes(dst+1,stride);
    if (zeroc>bestscore) {
      bestid=i;
      bestscore=zeroc;
    }
  }
  
  // If the best was not the last, reperform it.
  dst[0]=bestid;
  if (bestid!=optionc-1) image_perform_filter(bestid,dst+1,src,prv,stride,xstride);
}

/* Encode IDAT body.
 */
 
static int image_encode_idat(struct sr_encoder *dst,const uint8_t *src,int w,int h,int stride,int depth,int colortype) {

  int xstride;
  switch (colortype) {
    case 0: xstride=depth; break;
    case 2: xstride=depth*3; break;
    case 3: xstride=depth; break;
    case 4: xstride=depth*2; break;
    case 6: xstride=depth*4; break;
    default: return -1;
  }
  if ((xstride>>=3)<1) xstride=1;
  
  const uint8_t *pv=0;
  int y=0;
  uint8_t *rowbuf=malloc(1+stride);
  if (!rowbuf) return -1;
  
  z_stream z={0};
  if (deflateInit(&z,Z_BEST_COMPRESSION)<0) {
    free(rowbuf);
    return -1;
  }
  
  int err=0;
  for (;y<h;y++,src+=stride) {
    image_filter_row(rowbuf,src,pv,stride,xstride);
    z.next_in=(Bytef*)rowbuf;
    z.avail_in=1+stride;
    while (z.avail_in>0) {
      if (!z.avail_out) {
        if ((err=sr_encoder_require(dst,4096))<0) goto _done_;
        z.next_out=(Bytef*)dst->v+dst->c;
        z.avail_out=dst->a-dst->c;
      }
      int toa=z.total_out;
      if ((err=deflate(&z,Z_NO_FLUSH))<0) goto _done_;
      int addc=z.total_out-toa;
      dst->c+=addc;
    }
    pv=src;
  }
  
  for (;;) {
    if (!z.avail_out) {
      if ((err=sr_encoder_require(dst,4096))<0) goto _done_;
      z.next_out=(Bytef*)dst->v+dst->c;
      z.avail_out=dst->a-dst->c;
    }
    int toa=z.total_out;
    if ((err=deflate(&z,Z_FINISH))<0) goto _done_;
    int addc=z.total_out-toa;
    dst->c+=addc;
    if (err==Z_STREAM_END) break;
  }
  
 _done_:;
  free(rowbuf);
  deflateEnd(&z);
  return err;
}

/* Calculate CRC and append it.
 * Caller provides the nominal chunk length; there must be a 4-byte ID preceding it.
 */
 
static int image_append_crc(struct sr_encoder *dst,int len) {
  if ((len<0)||(len>=0x40000000)) return -1;
  len+=4;
  if (len>dst->c) return -1;
  uint32_t crc=crc32(crc32(0,0,0),(char*)dst->v+dst->c-len,len);
  return sr_encode_intbe(dst,crc,4);
}

/* Encode prepared pixels and color table to a new PNG file.
 */
 
static int image_encode_png(
  struct sr_encoder *dst,
  int depth,int colortype,
  int w,int h,int stride,
  const uint8_t *pixels,
  const uint8_t *plte,int pltec, // (c) in entries, not bytes
  const uint8_t *trns,int trnsc
) {
  
  sr_encode_raw(dst,"\x89PNG\r\n\x1a\n",8);
  sr_encode_intbe(dst,13,4);
  sr_encode_raw(dst,"IHDR",4);
  sr_encode_intbe(dst,w,4);
  sr_encode_intbe(dst,h,4);
  sr_encode_u8(dst,depth);
  sr_encode_u8(dst,colortype);
  sr_encode_u8(dst,0); // compression
  sr_encode_u8(dst,0); // filter
  sr_encode_u8(dst,0); // interlace
  image_append_crc(dst,13);
  
  if (pltec) {
    sr_encode_intbe(dst,pltec*3,4);
    sr_encode_raw(dst,"PLTE",4);
    sr_encode_raw(dst,plte,pltec*3);
    image_append_crc(dst,pltec*3);
  }
  if (trns) {
    sr_encode_intbe(dst,trnsc,4);
    sr_encode_raw(dst,"tRNS",4);
    sr_encode_raw(dst,trns,trnsc);
    image_append_crc(dst,trnsc);
  }
  
  int lenp=dst->c;
  sr_encode_raw(dst,"\0\0\0\0IDAT",8);
  if (image_encode_idat(dst,pixels,w,h,stride,depth,colortype)<0) return -1;
  int len=dst->c-lenp-8;
  if (len<0) return -1;
  ((uint8_t*)(dst->v))[lenp]=len>>24;
  ((uint8_t*)(dst->v))[lenp+1]=len>>16;
  ((uint8_t*)(dst->v))[lenp+2]=len>>8;
  ((uint8_t*)(dst->v))[lenp+3]=len;
  image_append_crc(dst,len);
  
  sr_encode_raw(dst,"\0\0\0\0IEND",8);
  image_append_crc(dst,0);
  return sr_encoder_assert(dst);
}

/* Stats for raw image.
 */
 
struct image_stats {
  uint32_t colorv[256];
  int colorc; // Only the first 256 get recorded, so this stops counting at 257. Different chroma with alpha zero count only once.
  int transparentc; // How many alpha==0 pixels.
  int opaquec; // How many alpha==1 pixels.
  int intermediatec; // How many pixels with alpha not zero or one. Pixels differing only by alpha *do* count as different colors too.
  int chromac; // How many non-exact-gray pixels.
};

static int image_stats_colorv_search(const struct image_stats *stats,uint32_t color) {
  int lo=0,hi=stats->colorc;
  while (lo<hi) {
    int ck=(lo+hi)>>1;
    uint32_t q=stats->colorv[ck];
         if (color<q) hi=ck;
    else if (color>q) lo=ck+1;
    else return ck;
  }
  return -lo-1;
}

static void image_stats_gather(struct image_stats *stats,const uint8_t *src,int srcc/* pixels, not bytes */) {
  for (;srcc-->0;src+=4) {
  
    uint32_t color;
    if (!src[3]) {
      color=0;
      stats->transparentc++;
    } else {
      if (src[3]==0xff) stats->opaquec++;
      else stats->intermediatec++;
      color=(src[0]<<24)|(src[1]<<16)|(src[2]<<8)|src[3];
      if ((src[0]!=src[1])||(src[1]!=src[2])) stats->chromac++;
    }
    
    int p=image_stats_colorv_search(stats,color);
    if (p<0) {
      if (stats->colorc>=256) {
        stats->colorc=257;
      } else {
        p=-p-1;
        memmove(stats->colorv+p+1,stats->colorv+p,sizeof(uint32_t)*(stats->colorc-p));
        stats->colorc++;
        stats->colorv[p]=color;
      }
    }
  }
}

/* Convert from RGBA to some other PNG-legal format.
 * We don't support 16-bit channels.
 */
 
static int plte_search(const uint8_t *plte,const uint8_t *trns,int pltec,const uint8_t *rgba) {
  int ix=0;
  for (;ix<pltec;ix++,plte+=3,trns++) {
    if (plte[0]!=rgba[0]) continue;
    if (plte[1]!=rgba[1]) continue;
    if (plte[2]!=rgba[2]) continue;
    if (trns[0]!=rgba[3]) continue;
    return ix;
  }
  return -1;
}
 
static void image_convert_for_png(
  uint8_t *plte,int *pltec, // 768 bytes. We fill (*pltec) with the count 0..256.
  uint8_t *trns,int *trnsc, // 256 bytes. We fill (*trnsc) with the count 0..256 which is also the size.
  uint8_t *dstrow,int dststride,
  int depth,int colortype,
  const uint8_t *src, // RGBA with minimum stride.
  int w,int h
) {

  // Indexed is kind of special.
  if (colortype==3) {
    int limit=1<<depth;
    int yi=h;
    uint8_t basemask=limit-1;
    for (;yi-->0;dstrow+=dststride) {
      int xi=w;
      uint8_t *dstp=dstrow;
      int shift=8-depth;
      for (;xi-->0;src+=4) {
        int ix=plte_search(plte,trns,*pltec,src);
        if (ix<0) {
          if ((*pltec)<limit) {
            int pltep=(*pltec)*3;
            plte[pltep++]=src[0];
            plte[pltep++]=src[1];
            plte[pltep++]=src[2];
            trns[*pltec]=src[3];
            ix=(*pltec)++;
          } else {
            ix=0;
          }
        }
        (*dstp)=((*dstp)&~(basemask<<shift))|(ix<<shift);
        if ((shift-=depth)<0) {
          shift+=8;
          dstp++;
        }
      }
    }
    *trnsc=*pltec;
    while (*trnsc&&(trns[(*trnsc)-1]==0xff)) (*trnsc)--;
    return;
  }
  
  // Opaque Gray can also work bitwise; it looks somewhat like Indexed.
  if (colortype==0) {
    int yi=h;
    uint8_t basemask=(1<<depth)-1;
    int reduce=8-depth;
    for (;yi-->0;dstrow+=dststride) {
      int xi=w;
      uint8_t *dstp=dstrow;
      int shift=8-depth;
      for (;xi-->0;src+=4) {
        int v=((src[0]+src[1]+src[2])/3)>>reduce;
        (*dstp)=((*dstp)&~(basemask<<shift))|(v<<shift);
        if ((shift-=depth)<0) {
          shift+=8;
          dstp++;
        }
      }
    }
    return;
  }
  
  // Gray+Alpha?
  if ((colortype==4)&&(depth==8)) {
    int yi=h;
    for (;yi-->0;dstrow+=dststride) {
      int xi=w;
      uint8_t *dstp=dstrow;
      for (;xi-->0;dstp+=2,src+=4) {
        dstp[0]=(src[0]+src[1]+src[2])/3;
        dstp[1]=src[3];
      }
    }
    return;
  }
  
  // RGB?
  if ((colortype==2)&&(depth==8)) {
    int yi=h;
    for (;yi-->0;dstrow+=dststride) {
      int xi=w;
      uint8_t *dstp=dstrow;
      for (;xi-->0;dstp+=3,src+=4) {
        dstp[0]=src[0];
        dstp[1]=src[1];
        dstp[2]=src[2];
      }
    }
    return;
  }
  
  // RGBA?
  if ((colortype==6)&&(depth==8)) {
    int cpc=w<<2;
    int yi=h;
    for (;yi-->0;dstrow+=dststride,src+=cpc) {
      memcpy(dstrow,src,cpc);
    }
    return;
  }
  
  // If we land here it's an error.
}

/* Encode image, main entry point.
 */
 
int image_encode(struct sr_encoder *dst,const void *rgba,int rgbac,int w,int h) {
  if (!dst||!rgba||(w<1)||(w>IMAGE_SIZE_LIMIT)||(h<1)||(h>IMAGE_SIZE_LIMIT)) return -1;
  int stride=w<<2;
  if (rgbac<stride*h) return -1;
  
  /* Analyze image, then select pixel format from this histogram.
   */
  struct image_stats stats={0};
  image_stats_gather(&stats,rgba,w*h);
  uint8_t depth,colortype;
  // If there's more than 256 colors, we can only use RGB or RGBA.
  // Our decoder only allows tRNS for Indexed color, so that simplifies things.
  if (stats.colorc>256) {
    depth=8;
    colortype=(stats.transparentc||stats.intermediatec)?6:2;
  // If there's anything non-gray, we must use Indexed.
  } else if (stats.chromac) {
    colortype=3;
    if (stats.colorc<=2) depth=1;
    else if (stats.colorc<=4) depth=2;
    else if (stats.colorc<=16) depth=4;
    else depth=8;
  // If transparency is in play, use Indexed again, but fall back on Gray+Alpha if there's too many colors.
  } else if (stats.transparentc||stats.intermediatec) {
    if (stats.colorc<=2) { depth=1; colortype=3; }
    else if (stats.colorc<=4) { depth=2; colortype=3; }
    else if (stats.colorc<=16) { depth=4; colortype=3; }
    else if (stats.colorc<=256) { depth=8; colortype=3; }
    else { depth=8; colortype=4; }
  // And finally, opaque Gray.
  } else {
    colortype=0;
    if (stats.colorc<=2) depth=1;
    else if (stats.colorc<=4) depth=2;
    else if (stats.colorc<=16) depth=4;
    else depth=8;
  }
  
  /* If we selected RGBA, proceed directly to PNG encode.
   * Otherwise, allocate another pixel buffer, convert into it, and then encode.
   */
  if ((depth==8)&&(colortype==6)) return image_encode_png(dst,depth,colortype,w,h,w<<2,rgba,0,0,0,0);
  int chanc;
  switch (colortype) {
    case 0: chanc=1; break;
    case 2: chanc=3; break;
    case 3: chanc=1; break;
    case 4: chanc=2; break;
    case 6: chanc=4; break;
    default: return -1;
  }
  int pixelsize=depth*chanc;
  int dststride=(w*pixelsize+7)>>3;
  int pixelslen=dststride*h;
  uint8_t *pixels=calloc(1,pixelslen);
  if (!pixels) return -1;
  uint8_t palstore[1024]={0};
  uint8_t *plte=palstore,*trns=palstore+768;
  int pltec=0,trnsc=0;
  image_convert_for_png(plte,&pltec,trns,&trnsc,pixels,dststride,depth,colortype,rgba,w,h);
  int err=image_encode_png(dst,depth,colortype,w,h,dststride,pixels,plte,pltec,trns,trnsc);
  free(pixels);
  return err;
}
