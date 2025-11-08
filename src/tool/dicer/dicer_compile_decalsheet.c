#include "dicer_internal.h"

/* Compile decalsheet from text.
 */
 
int dicer_compile_decalsheet(struct sr_encoder *dst,const void *src,int srcc) {
  struct decal { int id,x,y,w,h; } decalv[256]={0};
  struct sr_decoder decoder={.v=src,.c=srcc};
  const char *line;
  int linec,lineno=1;
  for (;(linec=sr_decode_line(&line,&decoder))>0;lineno++) {
    while (linec&&((unsigned char)line[linec-1]<=0x20)) linec--;
    while (linec&&((unsigned char)line[0]<=0x20)) { linec--; line++; }
    if (!linec||(line[0]=='#')) continue;
    
    // Every line that isn't blank or comment is: ID X Y W H [EXTRA...]
    int linep=0,id,x,y,w,h;
    const char *idsrc=line+linep;
    int idsrcc=0;
    while ((linep<linec)&&((unsigned char)line[linep++]>0x20)) idsrcc++;
    if (sym_get(&id,"NS_decal_",9,idsrc,idsrcc)<0) {
      fprintf(stderr,"%s:%d: Expected integer or 'NS_decal_' symbol, found '%.*s'\n",dicer.srcpath,lineno,idsrcc,idsrc);
      return -2;
    }
    #define INTTOKEN(name) { \
      while ((linep<linec)&&((unsigned char)line[linep]<=0x20)) linep++; \
      const char *token=line+linep; \
      int tokenc=0; \
      while ((linep<linec)&&((unsigned char)line[linep++]>0x20)) tokenc++; \
      if (sr_int_eval(&name,token,tokenc)<2) { \
        fprintf(stderr,"%s:%d: Expected integer for '%s', found '%.*s'\n",dicer.srcpath,lineno,#name,tokenc,token); \
        return -2; \
      } \
    }
    INTTOKEN(x)
    INTTOKEN(y)
    INTTOKEN(w)
    INTTOKEN(h)
    #undef INTTOKEN
    
    // id must be in 0..255, it's also the storage index.
    if ((id<0)||(id>0xff)) {
      fprintf(stderr,"%s:%d: Invalid decal id %d, must be in 0..255.\n",dicer.srcpath,lineno,id);
      return -2;
    }
    struct decal *decal=decalv+id;
    decal->id=id;
    
    // Anything goes for (x,y,w,h). We don't know the image bounds so no sense trying to validate.
    decal->x=x;
    decal->y=y;
    decal->w=w;
    decal->h=h;
  }
  
  /* With the model acquired, write it out as C.
   * Don't overthink this. Yeah, we could write it sparsely, but who cares. Write a bunch of zeroes.
   */
  if (sr_encode_raw(dst,"#include \"game/data.h\"\n",-1)<0) return -1;
  if (sr_encode_fmt(dst,"const struct decal %.*s[256]={\n",dicer.objnamec,dicer.objname)<0) return -1;
  const struct decal *decal=decalv;
  int i=256;
  for (;i-->0;decal++) {
    if (sr_encode_fmt(dst,"  {%d,%d,%d,%d,%d},\n",decal->id,decal->x,decal->y,decal->w,decal->h)<0) return -1;
  }
  if (sr_encode_raw(dst,"};\n",3)<0) return -1;
  return 1; // >0 because it's C text and not raw data.
}
