#include "dicer_internal.h"

struct dicer dicer={0};

/* Compile in memory. Determine type and dispatch.
 * Returns >0 if we've produced C text ready to roll in verbatim.
 * Or 0 if we produce binary data that needs to get wrapped generically.
 */
 
static int dicer_compile(struct sr_encoder *dst,const void *src,int srcc,const char *type,int typec) {
  
  // image: verbatim
  if ((typec==5)&&!memcmp(type,"image",5)) return sr_encode_raw(dst,src,srcc);
  
  // tilesheet: compile
  if ((typec==9)&&!memcmp(type,"tilesheet",9)) return dicer_compile_tilesheet(dst,src,srcc);
  
  // decalsheet: Compile. This will produce C text, not serial data, but that's not our problem.
  if ((typec==10)&&!memcmp(type,"decalsheet",10)) return dicer_compile_decalsheet(dst,src,srcc);
  
  // map: compile
  if ((typec==3)&&!memcmp(type,"map",3)) return dicer_compile_map(dst,src,srcc);
  
  // song: verbatim
  if ((typec==4)&&!memcmp(type,"song",4)) return sr_encode_raw(dst,src,srcc);
  
  // sound: verbatim
  if ((typec==5)&&!memcmp(type,"sound",5)) return sr_encode_raw(dst,src,srcc);
  
  // Anything else: verbatim but issue a warning.
  fprintf(stderr,"%s:WARNING: Unexpected resource type '%.*s'. Emitting verbatim. Please update %s:%d\n",dicer.srcpath,typec,type,__FILE__,__LINE__);
  return sr_encode_raw(dst,src,srcc);
}

/* Main.
 */
 
int main(int argc,char **argv) {

  // Read command line.
  dicer.exename=argv[0];
  int argi=1; for (;argi<argc;argi++) {
    const char *arg=argv[argi];
    if (!memcmp(arg,"-o",2)) {
      if (dicer.dstpath) {
        fprintf(stderr,"%s: Multiple output paths\n",dicer.exename);
        return 1;
      }
      dicer.dstpath=arg+2;
    } else if (arg[0]=='-') {
      fprintf(stderr,"%s: Unexpected option '%s'\n",dicer.exename,arg);
      return 1;
    } else if (dicer.srcpath) {
      fprintf(stderr,"%s: Multiple input paths\n",dicer.exename);
      return 1;
    } else {
      dicer.srcpath=arg;
    }
  }
  if (!dicer.dstpath||!dicer.srcpath) {
    fprintf(stderr,"Usage: %s -oOUTPUT INPUT\n",dicer.exename);
    return 1;
  }

  /* Determine the object's name.
   * This is generic but there will be exceptions.
   */
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
    dicer.objnamec=snprintf(dicer.objname,sizeof(dicer.objname),"map");
  } else {
    int basep=0;
    while ((basep<basec)&&(base[basep]>='0')&&(base[basep]<='9')) basep++; // skip rid; not meaningful to us.
    if ((basep<basec)&&(base[basep]=='-')) {
      basep++;
      const char *stem=base+basep;
      int stemc=0;
      while ((basep<basec)&&(base[basep]!='.')) { basep++; stemc++; }
      dicer.objnamec=snprintf(dicer.objname,sizeof(dicer.objname),"%.*s_%.*s",lastdirc,lastdir,stemc,stem);
    } else {
      fprintf(stderr,"%s: Malformed resource name\n",dicer.srcpath);
      return 1;
    }
  }
  if ((dicer.objnamec<1)||(dicer.objnamec>=sizeof(dicer.objname))) {
    fprintf(stderr,"%s: Failed to acquire object name. Is it too long?\n",dicer.srcpath);
    return 1;
  }
  
  // Acquire input.
  void *src=0;
  int srcc=file_read(&src,dicer.srcpath);
  if (srcc<0) {
    fprintf(stderr,"%s: Failed to read file.\n",dicer.srcpath);
    return 1;
  }
  
  // Digest input. Might be a verbatim copy, or we might do some kind of type-aware compilation.
  // Success may be 0 for binary data that needs generic wrapping, or >0 for finished C text.
  struct sr_encoder bin={0};
  int istext=dicer_compile(&bin,src,srcc,lastdir,lastdirc);
  if (istext<0) {
    if (istext!=-2) fprintf(stderr,"%s: Unspecified error digesting resource.\n",dicer.srcpath);
    return 1;
  }
  
  // Represent as C, if the compiler didn't.
  struct sr_encoder code={0};
  if (istext) {
    code.v=bin.v;
    code.c=bin.c;
    bin.v=0;
  } else {
    int err=dicer_to_c(&code,bin.v,bin.c);
    if (err<0) {
      if (err!=-2) fprintf(stderr,"%s: Unspecified error wrapping %d bytes compiled data in a C file.\n",dicer.srcpath,bin.c);
      return 1;
    }
  }
  
  // Write output.
  if (file_write(dicer.dstpath,code.v,code.c)<0) {
    fprintf(stderr,"%s: Failed to write %d-byte C file.\n",dicer.dstpath,code.c);
    return 1;
  }
  
  return 0;
}
