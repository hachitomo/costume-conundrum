#include "dicer_internal.h"

struct dicer dicer={0};

/* Compile in memory. Determine type and dispatch.
 */
 
static int dicer_compile(struct sr_encoder *dst,const void *src,int srcc) {
  
  /* Extract the last two components of the input path; those unambiguously tell us the type.
   * (base) might be interesting, it contains rid, name, and suffix, but for now I think we don't need.
   */
  const char *base=dicer.srcpath,*lastdir=0;
  int basec=0,lastdirc=0,pathp=0;
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
  
  // image: verbatim
  if ((lastdirc==5)&&!memcmp(lastdir,"image",5)) return sr_encode_raw(dst,src,srcc);
  
  // tilesheet: compile
  if ((lastdirc==9)&&!memcmp(lastdir,"tilesheet",9)) return dicer_compile_tilesheet(dst,src,srcc);
  
  // decalsheet: compile
  if ((lastdirc==10)&&!memcmp(lastdir,"decalsheet",10)) return dicer_compile_decalsheet(dst,src,srcc);
  
  // map: compile
  if ((lastdirc==3)&&!memcmp(lastdir,"map",3)) return dicer_compile_map(dst,src,srcc);
  
  // song: verbatim
  if ((lastdirc==4)&&!memcmp(lastdir,"song",4)) return sr_encode_raw(dst,src,srcc);
  
  // sound: verbatim
  if ((lastdirc==5)&&!memcmp(lastdir,"sound",5)) return sr_encode_raw(dst,src,srcc);
  
  // Anything else: verbatim but issue a warning.
  fprintf(stderr,"%s:WARNING: Unexpected resource type '%.*s'. Emitting verbatim. Please update %s:%d\n",dicer.srcpath,lastdirc,lastdir,__FILE__,__LINE__);
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
  
  // Acquire input.
  void *src=0;
  int srcc=file_read(&src,dicer.srcpath);
  if (srcc<0) {
    fprintf(stderr,"%s: Failed to read file.\n",dicer.srcpath);
    return 1;
  }
  
  // Digest input. Might be a verbatim copy, or we might do some kind of type-aware compilation.
  struct sr_encoder bin={0};
  int err=dicer_compile(&bin,src,srcc);
  if (err<0) {
    if (err!=-2) fprintf(stderr,"%s: Unspecified error digesting resource.\n",dicer.srcpath);
    return 1;
  }
  
  // Represent as C.
  struct sr_encoder code={0};
  if ((err=dicer_to_c(&code,bin.v,bin.c))<0) {
    if (err!=-2) fprintf(stderr,"%s: Unspecified error wrapping %d bytes compiled data in a C file.\n",dicer.srcpath,bin.c);
    return 1;
  }
  
  // Write output.
  if (file_write(dicer.dstpath,code.v,code.c)<0) {
    fprintf(stderr,"%s: Failed to write %d-byte C file.\n",dicer.dstpath,code.c);
    return 1;
  }
  
  return 0;
}
