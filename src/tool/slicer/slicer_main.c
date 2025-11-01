#include "slicer_internal.h"

static const char *imagedir="src/data/image";
static const char *decalsheetdir="src/data/decalsheet";
static const char *dstdir="src/data/sliced";

// Input images, the way Egg TOC would name them.
#define RID_image_sky 1
#define RID_image_terrain 2
#define RID_image_sprites 3

/* Compose output path for one sliced or verbatim image.
 */
 
static int compose_dstpath(char *dst,int dsta,int rid,int decalid) {
  const char *base=0;
  
  switch (rid) {
    case RID_image_sky: base="sky"; break;
    case RID_image_terrain: base="terrain"; break;
    case RID_image_sprites: switch (decalid) {
        case 1: base="dot-walk-1"; break;
        case 2: base="dot-walk-2"; break;
        case 3: base="dot-walk-3"; break;
        case 4: base="dot-walk-4"; break;
        case 5: base="dot-walk-5"; break;
        case 6: base="dot-walk-6"; break;
        case 7: base="dot-walk-7"; break;
        case 8: base="dot-walk-8"; break;
        case 9: base="dot-idle-1"; break;
        case 10: base="dot-idle-2"; break;
        case 11: base="dot-arm"; break;
      } break;
  }
  
  if (!base) {
    fprintf(stderr,"No defined output path for image:%d, decal %d [%s:%d]\n",rid,decalid,__FILE__,__LINE__);
    return -2;
  }
  return snprintf(dst,dsta,"%s/%s.png",dstdir,base);
}

/* Rid from path.
 */
 
static int rid_from_path(const char *path) {
  const char *base=path;
  int basec=0,pathp=0;
  for (;path[pathp];pathp++) {
    if (path[pathp]=='/') {
      base=path+pathp+1;
      basec=0;
    } else {
      basec++;
    }
  }
  if ((basec<1)||(base[0]<'0')||(base[0]>'9')) return 0;
  int rid=0,basep=0;
  for (;basep<basec;basep++) {
    if (base[basep]=='-') break;
    if (base[basep]=='.') break;
    if ((base[basep]<'0')||(base[basep]>'9')) return 0;
    rid*=10;
    rid+=base[basep]-'0';
  }
  return rid;
}

/* Decalsheet path for image path.
 * Succeeds only if it exists (or is supposed to exist).
 * These are hard-coded tho technically we could look them up on demand.
 */
 
static int decalsheet_path_for_image_path(char *dst,int dsta,const char *src) {
  int rid=rid_from_path(src);
  switch (rid) {
    case RID_image_sprites: return snprintf(dst,dsta,"%s/%d-sprites",decalsheetdir,RID_image_sprites);
  }
  return 0;
}

/* Select output path and copy image verbatim -- no decoding.
 */
 
static int copy_image_verbatim(const char *path) {
  int rid=rid_from_path(path);
  char dstpath[1024];
  int dstpathc=compose_dstpath(dstpath,sizeof(dstpath),rid,0);
  if ((dstpathc<1)||(dstpathc>=sizeof(dstpath))) return -1;
  void *serial=0;
  int serialc=file_read(&serial,path);
  if (serialc<0) {
    fprintf(stderr,"%s: Failed to read file.\n",path);
    return -2;
  }
  if (file_write(dstpath,serial,serialc)<0) {
    fprintf(stderr,"%s: Failed to write file, %d bytes.\n",dstpath,serialc);
    free(serial);
    return -2;
  }
  return 0;
}

/* Read a decalsheet text file and parse into an array of decals.
 * (decalv) must have at least 256 slots.
 */
 
struct decal {
  int id;
  int x,y,w,h;
};
 
static int decalsheet_decode(struct decal *decalv,const char *path) {
  int decalc=0;
  char *serial=0;
  int serialc=file_read(&serial,path);
  if (serialc<0) {
    fprintf(stderr,"%s: Failed to read file.\n",path);
    return -2;
  }
  struct sr_decoder decoder={.v=serial,.c=serialc};
  const char *line;
  int linec,lineno=1;
  for (;(linec=sr_decode_line(&line,&decoder))>0;lineno++) {
    while (line&&((unsigned char)line[linec-1]<=0x20)) linec--;
    while (linec&&((unsigned char)line[0]<=0x20)) { linec--; line++; }
    if (!linec||(line[0]=='#')) continue; // Egg editor won't produce blank lines or comments, but whatever, allow them.
    if (decalc>=256) {
      fprintf(stderr,"%s:%d: Too many decals, limit 256.\n",path,lineno);
      free(serial);
      return -2;
    }
    struct decal *decal=decalv+decalc++;
    int linep=0,err;
    #define INTTOKEN(name) { \
      while ((linep<linec)&&((unsigned char)line[linep]<=0x20)) linep++; \
      const char *token=line+linep; \
      int tokenc=0; \
      while ((linep<linec)&&((unsigned char)line[linep++]>0x20)) tokenc++; \
      if (sr_int_eval(&decal->name,token,tokenc)<2) { \
        fprintf(stderr,"%s:%d: Failed to evaluate token '%.*s' as integer for field '%s'\n",path,lineno,tokenc,token,#name); \
        free(serial); \
        return -2; \
      } \
    }
    INTTOKEN(id)
    INTTOKEN(x)
    INTTOKEN(y)
    INTTOKEN(w)
    INTTOKEN(h)
    #undef INTTOKEN
  }
  free(serial);
  return decalc;
}

/* Slice and write one decal image.
 */
 
static int emit_decal(const char *srcpath,const uint32_t *pixels,int imgw,int imgh,const struct decal *decal) {
  if ((decal->x<0)||(decal->y<0)||(decal->w<1)||(decal->h<1)||(decal->x>imgw-decal->w)||(decal->y>imgh-decal->h)) {
    fprintf(stderr,"%s: Invalid bounds (%d,%d,%d,%d) for decal id %d, in %dx%d image.\n",srcpath,decal->x,decal->y,decal->w,decal->h,decal->id,imgw,imgh);
    return -2;
  }
  int rid=rid_from_path(srcpath);
  char dstpath[1024];
  int dstpathc=compose_dstpath(dstpath,sizeof(dstpath),rid,decal->id);
  if ((dstpathc<1)||(dstpathc>=sizeof(dstpath))) {
    fprintf(stderr,"%s: Failed to compose output path for decal %d\n",srcpath,decal->id);
    return -2;
  }
  uint32_t *cropped=calloc(decal->w*decal->h,4);
  if (!cropped) return -1;
  const uint32_t *srcrow=pixels+decal->y*imgw+decal->x;
  uint32_t *dstrow=cropped;
  int cpc=decal->w<<2;
  int i=decal->h;
  for (;i-->0;dstrow+=decal->w,srcrow+=imgw) {
    memcpy(dstrow,srcrow,cpc);
  }
  struct sr_encoder png={0};
  if (image_encode(&png,cropped,decal->w*decal->h*4,decal->w,decal->h)<0) {
    fprintf(stderr,"%s: Unspecified error encoding decal %d @%d,%d,%d,%d\n",srcpath,decal->id,decal->x,decal->y,decal->w,decal->h);
    free(cropped);
    sr_encoder_cleanup(&png);
    return -2;
  }
  int err=file_write(dstpath,png.v,png.c);
  sr_encoder_cleanup(&png);
  free(cropped);
  if (err<0) {
    fprintf(stderr,"%s: Failed to write PNG file, %d bytes\n",dstpath,png.c);
    return -2;
  }
  fprintf(stderr,"%s: From %s:%d@%d,%d,%d,%d\n",dstpath,srcpath,decal->id,decal->x,decal->y,decal->w,decal->h);
  return 0;
}

/* For each image resource.
 */
 
static int cb_image(const char *path,const char *base,char ftype,void *userdata) {

  // Acquire its decalsheet's path. If there isn't one, copy verbatim.
  char decalsheetpath[1024];
  int decalsheetpathc=decalsheet_path_for_image_path(decalsheetpath,sizeof(decalsheetpath),path);
  if ((decalsheetpathc<1)||(decalsheetpathc>=sizeof(decalsheetpath))) {
    int err=copy_image_verbatim(path);
    if (err<0) {
      if (err!=-2) fprintf(stderr,"%s: Failed to copy file.\n",path);
      return -2;
    }
    fprintf(stderr,"%s: Verbatim.\n",path);
    return 0;
  }
  
  // Acquire decalsheet text and decode to a straightforward model.
  struct decal decalv[256];
  int decalc=decalsheet_decode(decalv,decalsheetpath);
  if (decalc<0) {
    if (decalc!=-2) fprintf(stderr,"%s: Unspecified error decoding decalsheet.\n",decalsheetpath);
    return -2;
  }
  if (!decalc) {
    fprintf(stderr,"%s:WARNING: Decalsheet expected but empty. Dropping image.\n",decalsheetpath);
    return 0;
  }
  
  // Acquire the image as flat RGBA.
  void *serial=0;
  int serialc=file_read(&serial,path);
  if (serialc<0) {
    fprintf(stderr,"%s: Failed to read file\n",path);
    return -2;
  }
  // Using the Egg2 image decoder, which is a little weird. It has no model object, just a "measure" function and a "decode" that always produces RGBA.
  int w=0,h=0;
  if (image_measure(&w,&h,serial,serialc)<0) {
    fprintf(stderr,"%s: Failed to measure image.\n",path);
    free(serial);
    return -2;
  }
  int stride=w<<2;
  uint32_t *pixels=calloc(stride,h);
  if (!pixels) {
    fprintf(stderr,"%s: Alloc failed (%dx%d)\n",path,w,h);
    free(serial);
    return -2;
  }
  if (image_decode(pixels,stride*h,serial,serialc)<0) {
    fprintf(stderr,"%s: Decode failed (%dx%d; %d bytes).\n",path,w,h,serialc);
    free(serial);
    free(pixels);
    return -2;
  }
  free(serial);
  
  // Emit an image file for each decal.
  const struct decal *decal=decalv;
  int i=decalc;
  for (;i-->0;decal++) {
    int err=emit_decal(path,pixels,w,h,decal);
    if (err<0) {
      if (err!=-2) fprintf(stderr,"%s: Error emitting decal %d @%d,%d,%d,%d\n",path,decal->id,decal->x,decal->y,decal->w,decal->h);
      free(pixels);
      return -2;
    }
  }
  
  free(pixels);
  return 0;
}

/* Main.
 */
 
int main(int argc,char **argv) {
  if (argc!=1) {
    fprintf(stderr,"Usage: %s\n",argv[0]);
    fprintf(stderr,"All options are hard-coded, see %s.\n",__FILE__);
    return 1;
  }
  if (dir_mkdirp(dstdir)<0) {
    fprintf(stderr,"%s: Failed to make directory\n",dstdir);
    return 1;
  }
  int err=dir_read(imagedir,cb_image,0);
  if (err<0) {
    if (err!=-2) fprintf(stderr,"%s: Unspecified error\n",argv[0]);
    return 1;
  }
  return 0;
}
