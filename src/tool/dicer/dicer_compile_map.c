#include "dicer_internal.h"

/* Compile map from text.
 * Map starts with a hex dump of the cell data, from which dimensions can be inferred.
 * Then a blank line, then a loose command list.
 * TODO Should we record the dimensions? I lean heavily Yes! But how?
 * TODO Should we record specific commands? ''
 */
 
int dicer_compile_map(struct sr_encoder *dst,const void *src,int srcc) {

  int mapw=0,maph=0,cella=8192;
  uint8_t *cellv=malloc(cella); // Leaks on errors, whatever
  if (!cellv) return -1;
  
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
  
  // TODO Can continue reading lines off (decoder) for the command list.
  
  if (sr_encode_raw(dst,cellv,mapw*maph)<0) return -1;
  free(cellv);
  
  return 0;
}
