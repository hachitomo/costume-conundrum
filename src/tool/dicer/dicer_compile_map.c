#include "dicer_internal.h"
#include "game/shared_symbols.h"

/* Evaluate command components.
 */
 
static int eval_position(int *x,int *y,const char *src,int srcc) {
  if (srcc<1) return -1;
  if (src[0]!='@') return -1;
  int srcp=1;
  *x=0;
  if ((srcp>=srcc)||(src[srcp]<'0')||(src[srcp]>'9')) return -1;
  while ((srcp<srcc)&&(src[srcp]>='0')&&(src[srcp]<='9')) {
    (*x)*=10;
    (*x)+=src[srcp++]-'0';
  }
  if (srcp>=srcc) return -1;
  if (src[srcp++]!=',') return -1;
  *y=0;
  if ((srcp>=srcc)||(src[srcp]<'0')||(src[srcp]>'9')) return -1;
  while ((srcp<srcc)&&(src[srcp]>='0')&&(src[srcp]<='9')) {
    (*y)*=10;
    (*y)+=src[srcp++]-'0';
  }
  if (srcp<srcc) return -1;
  return 0;
}

static int eval_sprite_name(int *dst,const char *src,int srcc) {
  if ((srcc==4)&&!memcmp(src,"hero",4)) { *dst=1; return 0; }
  if ((srcc==5)&&!memcmp(src,"ghost",5)) { *dst=2; return 0; }
  if ((srcc==8)&&!memcmp(src,"princess",8)) { *dst=3; return 0; }
  if ((srcc==7)&&!memcmp(src,"pumpkin",7)) { *dst=4; return 0; }
  if ((srcc==5)&&!memcmp(src,"robot",5)) { *dst=5; return 0; }
  if ((srcc==5)&&!memcmp(src,"clown",5)) { *dst=6; return 0; }
  return -1;
}

/* Compile map from text.
 * Map starts with a hex dump of the cell data, from which dimensions can be inferred.
 * Then a blank line, then a loose command list.
 * There can only be one map in the game, per design, so these populate some global symbols.
 */
 
int dicer_compile_map(struct sr_encoder *dst,const void *src,int srcc) {

  int mapw=0,maph=0,cella=8192;
  uint8_t *cellv=malloc(cella); // Leaks on errors, whatever
  if (!cellv) return -1;
  
  // Read the cells image, and determine dimensions.
  struct sr_decoder decoder={.v=src,.c=srcc};
  const char *line;
  int linec,lineno=1;
  for (;(linec=sr_decode_line(&line,&decoder))>0;lineno++) {
    while (linec&&((unsigned char)line[linec-1]<=0x20)) linec--;
    while (linec&&((unsigned char)line[0]<=0x20)) { linec--; line++; }
    if (!linec||(line[0]=='#')) break; // Stop at the first blank line or comment. Blanks are not permitted during or before the cells image.
    
    // If we haven't established width yet, this line establishes it.
    if (!mapw) {
      if (linec&1) {
        fprintf(stderr,"%s:%d: Invalid row length %d\n",dicer.srcpath,lineno,linec);
        return -2;
      }
      mapw=linec>>1;
    // Otherwise, it must agree with the established width.
    } else if (linec!=mapw<<1) {
      fprintf(stderr,"%s:%d: Line length %d disagrees with prior lines.\n",dicer.srcpath,lineno,linec);
      return -2;
    }
    
    // Grow the cell buffer if needed.
    int na=mapw*(maph+1);
    if (na>cella) {
      if (na>INT_MAX-1024) return -1; // huh?
      na=(na+1024)&~1023;
      void *nv=realloc(cellv,na);
      if (!nv) return -1;
      cellv=nv;
      cella=na;
    }
    
    // Evaluate line directly into cells image.
    uint8_t *dst=cellv+(mapw*maph);
    int i=mapw;
    for (;i-->0;dst++,line+=2) {
      int hi=sr_digit_eval(line[0]);
      int lo=sr_digit_eval(line[1]);
      if ((hi<0)||(hi>15)||(lo<0)||(lo>15)) {
        fprintf(stderr,"%s:%d: Expected hex byte, found '%.2s'\n",dicer.srcpath,lineno,line);
        return -2;
      }
      *dst=(hi<<4)|lo;
    }
    
    maph++;
  }
  if ((mapw<1)||(maph<1)) {
    fprintf(stderr,"%s:%d: Expected cells image\n",dicer.srcpath,lineno);
  }
  
  // Read the rest of the map looking for POI commands.
  struct poi { int x,y,cmd,argv[4]; } *poiv=0;
  int poic=0,poia=0;
  for (;(linec=sr_decode_line(&line,&decoder))>0;lineno++) {
    while (linec&&((unsigned char)line[linec-1]<=0x20)) linec--;
    while (linec&&((unsigned char)line[0]<=0x20)) { linec--; line++; }
    if (!linec||(line[0]=='#')) continue;
    
    int linep=0,tokenc;
    const char *token;
    #define NEXTTOKEN { \
      while ((linep<linec)&&((unsigned char)line[linep]<=0x20)) linep++; \
      token=line+linep; \
      tokenc=0; \
      while ((linep<linec)&&((unsigned char)line[linep++]>0x20)) tokenc++; \
    }
    #define MOREPOI if (poic>=poia) { \
      int na=poia+16; \
      if (na>INT_MAX/sizeof(struct poi)) return -1; \
      void *nv=realloc(poiv,sizeof(struct poi)*na); \
      poiv=nv; \
      poia=na; \
    }
    NEXTTOKEN
    
    if ((tokenc==6)&&!memcmp(token,"sprite",6)) { // sprite @X,Y NAME XXX unused
      MOREPOI
      struct poi *poi=poiv+poic++;
      memset(poi,0,sizeof(struct poi));
      NEXTTOKEN
      if (eval_position(&poi->x,&poi->y,token,tokenc)<0) {
        fprintf(stderr,"%s:%d: Expected '@X,Y', found '%.*s'\n",dicer.srcpath,lineno,tokenc,token);
        return -2;
      }
      NEXTTOKEN
      if (eval_sprite_name(poi->argv+0,token,tokenc)<0) {
        fprintf(stderr,"%s:%d: Expected sprite name, found '%.*s'\n",dicer.srcpath,lineno,tokenc,token);
        return -2;
      }
    }
    
    #define POSONLY(tag) if ((tokenc==sizeof(#tag)-1)&&!memcmp(token,#tag,tokenc)) { \
      MOREPOI \
      struct poi *poi=poiv+poic++; \
      memset(poi,0,sizeof(struct poi)); \
      poi->cmd=CMD_map_##tag; \
      NEXTTOKEN \
      if (eval_position(&poi->x,&poi->y,token,tokenc)<0) { \
        fprintf(stderr,"%s:%d: Expected '@X,Y', found '%.*s'\n",dicer.srcpath,lineno,tokenc,token); \
        return -2; \
      } \
    }
    else POSONLY(hero)
    else POSONLY(ghost)
    else POSONLY(princess)
    else POSONLY(pumpkin)
    else POSONLY(robot)
    else POSONLY(clown)
    else POSONLY(lightbear)
    else POSONLY(cat)
    else POSONLY(jack)
    else POSONLY(pumpkinhat)
    #undef POSONLY
      
    else {
      // Unknown commands are fine, ignore them.
    }
    #undef NEXTTOKEN
    #undef MOREPOI
  }
  
  // Emit C text.
  if (sr_encode_raw(dst,"#include \"game/data.h\"\n",-1)<0) return -1;
  if (sr_encode_fmt(dst,"const int map_w=%d;\n",mapw)<0) return -1;
  if (sr_encode_fmt(dst,"const int map_h=%d;\n",maph)<0) return -1;
  if (sr_encode_fmt(dst,"const int map_poic=%d;\n",poic)<0) return -1;
  if (sr_encode_fmt(dst,"const unsigned char map[]={\n")<0) return -1;
  int i=mapw*maph;
  const uint8_t *p=cellv;
  for (;i-->0;p++) {
    if (sr_encode_fmt(dst,"%d,",*p)<0) return -1;
    if (i%100==99) sr_encode_u8(dst,0x0a);
  }
  if (sr_encode_raw(dst,"};\n",3)<0) return -1;
  if (sr_encode_fmt(dst,"const struct map_poi map_poiv[]={\n")<0) return -1;
  const struct poi *poi=poiv;
  for (i=poic;i-->0;poi++) {
    if (sr_encode_fmt(dst,"  {%d,%d,%d,{%d,%d,%d,%d}},\n",poi->x,poi->y,poi->cmd,poi->argv[0],poi->argv[1],poi->argv[2],poi->argv[3])<0) return -1;
  }
  if (sr_encode_raw(dst,"};\n",3)<0) return -1;
  
  free(cellv);
  if (poiv) free(poiv);
  return 1;
}
