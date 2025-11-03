#include "dicer_internal.h"

/* Wrap arbitrary data in a C file, in memory.
 */
 
int dicer_to_c(struct sr_encoder *dst,const void *src,int srcc) {

  /* First determine the object's name.
   * This is generic but there will be exceptions.
   */
  char objname[64];
  int objnamec=0;
  const char *base=dicer.srcpath,*lastdir=0,*stem=0;
  int basec=0,lastdirc=0,pathp=0,stemc=0;
  for (;dicer.srcpath[pathp];pathp++) {
    if (dicer.srcpath[pathp]=='/') {
      lastdir=base;
      lastdirc=basec;
      base=dicer.srcpath+pathp+1;
      basec=0;
    } else {
      basec++;
    }
  }
  if (!lastdir) return -1;
  if ((lastdirc==3)&&!memcmp(lastdir,"map",3)&&(basec==1)&&(base[0]=='1')) {
    objnamec=snprintf(objname,sizeof(objname),"map");
  } else {
    int basep=0;
    while ((basep<basec)&&(base[basep]>='0')&&(base[basep]<='9')) basep++; // skip rid; not meaningful to us.
    if ((basep<basec)&&(base[basep]=='-')) {
      basep++;
      const char *stem=base+basep;
      int stemc=0;
      while ((basep<basec)&&(base[basep]!='.')) { basep++; stemc++; }
      objnamec=snprintf(objname,sizeof(objname),"%.*s_%.*s",lastdirc,lastdir,stemc,stem);
    } else {
      fprintf(stderr,"%s: Malformed resource name\n",dicer.srcpath);
      return -2;
    }
  }
  if ((objnamec<1)||(objnamec>=sizeof(objname))) {
    fprintf(stderr,"%s: Failed to acquire object name. Is it too long?\n",dicer.srcpath);
    return -2;
  }
  
  sr_encode_fmt(dst,"const int %.*s_length=%d;\n",objnamec,objname,srcc);
  sr_encode_fmt(dst,"const unsigned char %.*s[]={\n",objnamec,objname);
  const uint8_t *SRC=src;
  int srcp=0;
  for (;srcp<srcc;srcp++) {
    sr_encode_fmt(dst,"%d,",SRC[srcp]);
    if (srcp%100==99) sr_encode_u8(dst,0x0a); // Newline every 100 bytes because long lines are ugly.
  }
  sr_encode_raw(dst,"};\n",3);
  
  return sr_encoder_assert(dst);
}
