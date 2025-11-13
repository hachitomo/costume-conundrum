#include <string.h>
#include "../shared_symbols.h"
#include "../frame_timer.h"
#include "../map/map.h"
#include "../draw/draw.h"
#include "npc.h"

/* Global NPC list.
 */

#define NPC_LIMIT 32
static NPC npcv[NPC_LIMIT];
static int npcc=0;

int get_npcs(NPC **v) {
    *v=npcv;
    return npcc;
}

/* Change decal.
 * Preserves center position.
 */
 
static void npc_set_decal(NPC *npc,int decalid) {
    npc->id=decalid;
    if ((decalid<0)||(decalid>0xff)) return;
    const struct decal *decal=decalsheet_sprites+decalid;
    int midx=(int)npc->position.x+(((int)npc->width)>>1);
    int midy=(int)npc->position.y+(((int)npc->height)>>1);
    npc->width=decal->w;
    npc->height=decal->h;
    npc->position.x=midx-(decal->w>>1);
    npc->position.y=midy-(decal->h>>1);
}

/* Initial position nudge.
 * NPCs begin centered on their nominal cell.
 * Hardly any of them actually want to stay right there.
 */
 
static void npc_initial_nudge(NPC *npc) {
    switch (npc->id) {
        case NS_decal_robot_wrong: npc->position.y-=7.0f; break;
        case NS_decal_clown_wrong: npc->position.y-=8.0f; break;
        case NS_decal_lightbear_dig1: npc->position.x-=1.0f; npc->position.y-=7.0f; break;
        case NS_decal_cat1: npc->position.x+=4.0f; npc->position.y+=1.0f; break;
        case NS_decal_jack1: npc->position.x-=1.0f; npc->position.y-=5.0f; break;
        case NS_decal_pumpkinhat: npc->position.y+=4.0f; break;
    }
}

/* Init NPC.
 */
 
NPC *init_npc(int x,int y,int decalid) {
    if (npcc>=NPC_LIMIT) return 0;
    NPC *npc = npcv+npcc++;
    memset(npc,0,sizeof(NPC));
    npc->position=(Vector2){x*TILE_SIZE,y*TILE_SIZE};
    npc->id=decalid;
    npc->id0=decalid;
    npc->state=0;
    npc->xtransform=0;
    npc->just_updated=0;
    npc->state_time=0.0f;
    npc->width=TILE_SIZE;
    npc->height=TILE_SIZE;
    npc_set_decal(npc,decalid);
    npc_initial_nudge(npc);
    return npc;
}

/* Advance animation frame and clock.
 */
 
static void npc_animate(NPC *npc) {
    switch (npc->id0) {
    
        // 1,2,1,3 if sated.
        case NS_decal_robot_wrong: {
            //TODO how to flag if sated?
          } break;
          
        // 1,2,3,4 if sated.
        case NS_decal_clown_wrong: {
            //TODO how to flag if sated?
          } break;
          
        // Bunch of dig(1,2) then bunch of eat(1,2).
        case NS_decal_lightbear_dig1: {
            npc->animclock+=0.250;
            if (++(npc->animframe)>=16) npc->animframe=0;
            if (npc->animframe&8) {
              npc_set_decal(npc,(npc->animframe&1)?NS_decal_lightbear_eat2:NS_decal_lightbear_eat1);
            } else {
              npc_set_decal(npc,(npc->animframe&1)?NS_decal_lightbear_dig2:NS_decal_lightbear_dig1);
            }
          } break;
          
        // 1,2,3,4
        case NS_decal_cat1: {
            npc->animclock+=0.300;
            if (++(npc->animframe)>=4) npc->animframe=0;
            switch (npc->animframe) {
                case 0: npc_set_decal(npc,NS_decal_cat1); break;
                case 1: npc_set_decal(npc,NS_decal_cat2); break;
                case 2: npc_set_decal(npc,NS_decal_cat3); break;
                case 3: npc_set_decal(npc,NS_decal_cat4); break;
            }
          } break;
          
        // 1,2
        case NS_decal_jack1: {
            npc->animclock+=0.200;
            if (++(npc->animframe)>=2) npc->animframe=0;
            switch (npc->animframe) {
                case 0: npc_set_decal(npc,NS_decal_jack1); break;
                case 1: npc_set_decal(npc,NS_decal_jack2); break;
            }
          } break;
        
        // Not animated.
        case NS_decal_pumpkinhat: npc->animclock+=999.999; break;
    }
}

/* Update.
 */
 
void update_npc(NPC *npc,FrameTimer *ftimer) {
    if ((npc->animclock-=ftimer->frame_time)<=0.0) npc_animate(npc);
    //TODO motion
}

/* Draw.
 */
 
void draw_npc(NPC *npc) {
    if (!npc||(npc->id<0)||(npc->id>0xff)) return;
    Texture2D texture=get_texture(TEXTURE_SPRITES);
    const struct decal *decal=decalsheet_sprites+npc->id;
    Rectangle srcr={decal->x,decal->y,decal->w,decal->h};
    Rectangle dstr={npc->position.x,npc->position.y,npc->width,npc->height};
    if (npc->xtransform) {
      dstr.width*=-1.0f;
    }
    DrawTexturePro(texture,srcr,dstr,VEC_ZERO,0,WHITE);
}
