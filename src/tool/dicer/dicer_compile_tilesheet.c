#include "dicer_internal.h"

/* Compile tilesheet from text.
 * Tilesheets are line-oriented text.
 * One line containing the table's name, then 16 lines each containing 32 hex digits.
 * The only table we care about is "physics".
 */
 
int dicer_compile_tilesheet(struct sr_encoder *dst,const void *src,int srcc) {
  struct sr_decoder decoder={.v=src,.c=srcc};
  const char *line;
  int linec,lineno=1,reading_physics=0,tablep=0;
  uint8_t table[256]={0};
  for (;(linec=sr_decode_line(&line,&decoder))>0;lineno++) {
    while (linec&&((unsigned char)line[linec-1]<=0x20)) linec--;
    while (linec&&((unsigned char)line[0]<=0x20)) { linec--; line++; }
    if (!linec||(line[0]=='#')) continue;
    
    if (reading_physics) {
      if (tablep>=256) break; // End of physics table, and no need to read any further.
      if (linec!=32) {
        fprintf(stderr,"%s:%d: Expected 32 hex digits\n",dicer.srcpath,lineno);
        return -2;
      }
      int i=16;
      for (;i-->0;tablep++,line+=2) {
        int hi=sr_digit_eval(line[0]);
        int lo=sr_digit_eval(line[1]);
        if ((hi<0)||(hi>15)||(lo<0)||(lo>15)) {
          fprintf(stderr,"%s:%d: Expected hex byte, found '%.2s'\n",dicer.srcpath,lineno,line);
          return -2;
        }
        table[tablep]=(hi<<4)|lo;
      }
      
    } else if ((linec==7)&&!memcmp(line,"physics",7)) {
      reading_physics=1;
    }
  }
  // Neither of these conditions strictly needs to be fatal, but they are highly suggestive of some wrong that the user should know about.
  if (!reading_physics) {
    fprintf(stderr,"%s: Tilesheet does not contain a 'physics' table\n",dicer.srcpath);
    return -2;
  }
  if (tablep<256) {
    fprintf(stderr,"%s: Expected 256 entries in physics table, found %d\n",dicer.srcpath,tablep);
    return -2;
  }
  return sr_encode_raw(dst,table,256);
}
