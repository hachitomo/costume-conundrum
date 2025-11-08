#include "dicer_internal.h"

/* Register one symbol.
 */
 
static int sym_add(const char *k,int kc,int v) {
  if (dicer.symc>=dicer.syma) {
    int na=dicer.syma+64;
    if (na>INT_MAX/sizeof(struct sym)) return -1;
    void *nv=realloc(dicer.symv,sizeof(struct sym)*na);
    if (!nv) return -1;
    dicer.symv=nv;
    dicer.syma=na;
  }
  char *nk=malloc(kc+1);
  if (!nk) return -1;
  memcpy(nk,k,kc);
  nk[kc]=0;
  struct sym *sym=dicer.symv+dicer.symc++;
  sym->k=nk;
  sym->kc=kc;
  sym->v=v;
  return 0;
}

/* Load symbols from provided text.
 */
 
static int sym_load_text(const char *src,int srcc,const char *path) {
  struct sr_decoder decoder={.v=src,.c=srcc};
  const char *line;
  int linec,lineno=1;
  for (;(linec=sr_decode_line(&line,&decoder))>0;lineno++) {
    while (linec&&((unsigned char)line[linec-1]<=0x20)) linec--;
    while (linec&&((unsigned char)line[0]<=0x20)) { linec--; line++; }
    if ((linec<8)||memcmp(line,"#define ",8)) continue;
    int linep=8;
    while ((linep<linec)&&((unsigned char)line[linep]<=0x20)) linep++;
    const char *k=line+linep;
    int kc=0;
    while ((linep<linec)&&((unsigned char)line[linep++]>0x20)) kc++;
    while ((linep<linec)&&((unsigned char)line[linep]<=0x20)) linep++;
    const char *v=line+linep;
    int vc=0;
    while ((linep<linec)&&((unsigned char)line[linep++]>0x20)) vc++;
    while ((linep<linec)&&((unsigned char)line[linep]<=0x20)) linep++;
    if ((linep<linec)&&(line[linep]!='/')) continue; // Unexpected token. Should be end of line or a comment.
    int vn;
    if (sr_int_eval(&vn,v,vc)<1) continue; // Not an integer.
    if (sym_add(k,kc,vn)<0) return -1;
  }
  return 0;
}

/* Load symbols if we haven't got yet.
 */
 
static int sym_require() {
  if (dicer.sym_loaded) return 0;
  dicer.sym_loaded=1;
  char *src=0;
  const char *path="src/game/shared_symbols.h";
  int srcc=file_read(&src,path);
  if (srcc<0) return -1;
  sym_load_text(src,srcc,path);
  free(src);
  return 0;
}

/* Get symbol.
 */
 
int sym_get(int *v,const char *pfx,int pfxc,const char *k,int kc) {
  if (!pfx) pfxc=0; else if (pfxc<0) { pfxc=0; while (pfx[pfxc]) pfxc++; }
  if (!k) kc=0; else if (kc<0) { kc=0; while (k[kc]) kc++; }
  if (!pfxc&&!kc) return -1;
  if (kc&&(sr_int_eval(v,k,kc)>=2)) return 0;
  sym_require();
  const struct sym *sym=dicer.symv;
  int i=dicer.symc;
  int combinec=pfxc+kc;
  for (;i-->0;sym++) {
    if (sym->kc!=combinec) continue;
    if (memcmp(sym->k,pfx,pfxc)) continue;
    if (memcmp(sym->k+pfxc,k,kc)) continue;
    *v=sym->v;
    return 0;
  }
  return -1;
}
