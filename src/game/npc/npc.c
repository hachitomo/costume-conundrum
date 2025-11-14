#include <string.h>
#include "../shared_symbols.h"
#include "../frame_timer.h"
#include "../audio/audio.h"
#include "../map/map.h"
#include "../draw/draw.h"
#include "../hero/hero.h"
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

/* Bounds.
 */
 
static Rectangle npc_get_bbox(const NPC *npc) {
    return (Rectangle){npc->position.x,npc->position.y,npc->width,npc->height};
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
        case NS_decal_ghost_wrong: npc->argv[1]=1; npc->position.y-=7.0f; break;
        case NS_decal_princess_wrong: npc->argv[1]=1; npc->position.y-=6.0f; break;
        case NS_decal_pumpkin_wrong: npc->argv[1]=1; npc->position.y-=8.0f; break;
        case NS_decal_robot_wrong: npc->argv[1]=1; npc->position.y-=7.0f; break;
        case NS_decal_clown_wrong: npc->argv[1]=1; npc->position.y-=8.0f; break;
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

/* After creating all of them, do a one-time sort.
 * Mostly we don't care about the order, but (npc->argv[1]==0) must render before (npc->argv[1]==1).
 */
 
static int npc_rendercmp(const NPC *a,const NPC *b) {
    return a->argv[1]-b->argv[1];
}
 
void sort_npcs() {
    int lo=0,hi=npcc-1,d=1;
    while (lo<hi) {
        int done=1,i,last;
        if (d==1) {
            i=lo;
            last=hi;
        } else {
            i=hi;
            last=lo;
        }
        for (;i!=last;i+=d) {
            int cmp=npc_rendercmp(npcv+i,npcv+i+d);
            if (cmp==d) {
                NPC tmp=npcv[i];
                npcv[i]=npcv[i+d];
                npcv[i+d]=tmp;
                done=0;
            }
        }
        if (done) break;
        if (d==1) {
            hi--;
            d=-1;
        } else {
            lo++;
            d=1;
        }
    }
}

/* Advance animation frame and clock.
 */
 
static void npc_animate(NPC *npc) {
    switch (npc->id0) {
    
        // 1,2,3,4 if sated.
        case NS_decal_ghost_wrong: switch (npc->argv[0]) {
            case 0: npc->animclock+=999.999; npc_set_decal(npc,NS_decal_ghost_wrong); break;
            case 1: npc->animclock+=999.999; npc_set_decal(npc,NS_decal_ghost_right); break;
            case 2: {
                npc->animclock+=0.200;
                if (++(npc->animframe)>=4) npc->animframe=0;
                switch (npc->animframe) {
                    case 0: npc_set_decal(npc,NS_decal_ghost_walk1); break;
                    case 1: npc_set_decal(npc,NS_decal_ghost_walk2); break;
                    case 2: npc_set_decal(npc,NS_decal_ghost_walk3); break;
                    case 3: npc_set_decal(npc,NS_decal_ghost_walk4); break;
                }
              } break;
          } break;
    
        // 1,2,3,4 if sated.
        case NS_decal_princess_wrong: switch (npc->argv[0]) {
            case 0: npc->animclock+=999.999; npc_set_decal(npc,NS_decal_princess_wrong); break;
            case 1: npc->animclock+=999.999; npc_set_decal(npc,NS_decal_princess_right); break;
            case 2: {
                npc->animclock+=0.200;
                if (++(npc->animframe)>=4) npc->animframe=0;
                switch (npc->animframe) {
                    case 0: npc_set_decal(npc,NS_decal_princess_walk1); break;
                    case 1: npc_set_decal(npc,NS_decal_princess_walk2); break;
                    case 2: npc_set_decal(npc,NS_decal_princess_walk3); break;
                    case 3: npc_set_decal(npc,NS_decal_princess_walk4); break;
                }
              } break;
          } break;
    
        // 1,2,1,3 if sated.
        case NS_decal_pumpkin_wrong: switch (npc->argv[0]) {
            case 0: npc->animclock+=999.999; npc_set_decal(npc,NS_decal_pumpkin_wrong); break;
            case 1: npc->animclock+=999.999; npc_set_decal(npc,NS_decal_pumpkin_right); break;
            case 2: {
                npc->animclock+=0.200;
                if (++(npc->animframe)>=4) npc->animframe=0;
                switch (npc->animframe) {
                    case 0: npc_set_decal(npc,NS_decal_pumpkin_walk1); break;
                    case 1: npc_set_decal(npc,NS_decal_pumpkin_walk2); break;
                    case 2: npc_set_decal(npc,NS_decal_pumpkin_walk1); break;
                    case 3: npc_set_decal(npc,NS_decal_pumpkin_walk3); break;
                }
              } break;
          } break;
    
        // 1,2,1,3 if sated.
        case NS_decal_robot_wrong: switch (npc->argv[0]) {
            case 0: npc->animclock+=999.999; npc_set_decal(npc,NS_decal_robot_wrong); break;
            case 1: npc->animclock+=999.999; npc_set_decal(npc,NS_decal_robot_right); break;
            case 2: {
                npc->animclock+=0.200;
                if (++(npc->animframe)>=4) npc->animframe=0;
                switch (npc->animframe) {
                    case 0: npc_set_decal(npc,NS_decal_robot_walk1); break;
                    case 1: npc_set_decal(npc,NS_decal_robot_walk2); break;
                    case 2: npc_set_decal(npc,NS_decal_robot_walk1); break;
                    case 3: npc_set_decal(npc,NS_decal_robot_walk3); break;
                }
              } break;
          } break;
          
        // 1,2,3,4 if sated.
        case NS_decal_clown_wrong: switch (npc->argv[0]) {
            case 0: npc->animclock+=999.999; npc_set_decal(npc,NS_decal_clown_wrong); break;
            case 1: npc->animclock+=999.999; npc_set_decal(npc,NS_decal_clown_right); break;
            case 2: {
                npc->animclock+=0.200;
                if (++(npc->animframe)>=4) npc->animframe=0;
                switch (npc->animframe) {
                    case 0: npc_set_decal(npc,NS_decal_clown_walk1); break;
                    case 1: npc_set_decal(npc,NS_decal_clown_walk2); break;
                    case 2: npc_set_decal(npc,NS_decal_clown_walk3); break;
                    case 3: npc_set_decal(npc,NS_decal_clown_walk4); break;
                }
              } break;
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

/* Update robot or clown (ghost and princess are separate for no particular reason).
 */

static void npc_update_kid(NPC *npc,FrameTimer *ftimer,int inv_give,int inv_take) {

    // argv[0]==2: Sated. Walk back and forth.
    if (npc->argv[0]==2) {
        const float walk_speed=32.0f; // px/s
        int footx;
        if (npc->xtransform) {
            npc->position.x-=walk_speed*ftimer->frame_time;
            footx=(int)(npc->position.x/TILE_SIZE);
        } else {
            npc->position.x+=walk_speed*ftimer->frame_time;
            footx=(int)((npc->position.x+npc->width)/TILE_SIZE);
        }
        if (footx<0) footx=0; else if (footx>=map_w) footx=map_w-1;
        int footy_up=(int)((npc->position.y+npc->height-8.0f)/TILE_SIZE);
        if (footy_up<0) footy_up=0; else if (footy_up>=map_h-1) footy_up=map_h-2;
        int footy_down=footy_up+1;
        unsigned char ph_up=tilesheet_terrain[map[footy_up*map_w+footx]];
        unsigned char ph_down=tilesheet_terrain[map[footy_down*map_w+footx]];
        if ((ph_up!=NS_physics_vacant)||(ph_down==NS_physics_vacant)) {
            npc->xtransform^=1;
        }
    
    // argv[0] 0 or 1: Look at Dot. And if 0, give her our item on collision.
    } else {
        Hero *hero=get_hero();
        if (hero->bbox.x>npc->position.x+npc->width) {
            npc->xtransform=0;
        } else if (hero->bbox.x+hero->bbox.width<npc->position.x) {
            npc->xtransform=1;
        }
        // Take a penny?
        if (npc->argv[0]==0) {
            if (!inv_give) {
                npc->argv[0]=1;
            } else {
                Rectangle bbox=npc_get_bbox(npc);
                if (CheckCollisionRecs(hero->bbox,bbox)) {
                    npc->argv[0]=inv_take?1:2;
                    npc->animclock=0.0;
                    PlaySound(get_sound(SOUND_PICKUP));
                    set_inventory(inv_give,1);
                }
            }
        // Give a penny?
        } else if (npc->argv[0]==1) {
            if (get_inventory(inv_take)) {
                Rectangle bbox=npc_get_bbox(npc);
                if (CheckCollisionRecs(hero->bbox,bbox)) {
                    npc->argv[0]=2;
                    npc->animclock=0.0;
                    PlaySound(success_sound());
                    set_inventory(inv_take,0);
                }
            }
        }
    }
}

/* Update pumpkinhat.
 */
 
static void npc_update_pumpkinhat(NPC *npc,FrameTimer *ftimer) {
    Hero *hero = get_hero();
    Rectangle bbox=npc_get_bbox(npc);
    if (CheckCollisionRecs(hero->bbox,bbox)) {
        npc->defunct=1;
        PlaySound(get_sound(SOUND_PICKUP));
        set_inventory(INV_pump,1);
    }
}

/* Update.
 */
 
void update_npc(NPC *npc,FrameTimer *ftimer) {

    // Animate.
    if ((npc->animclock-=ftimer->frame_time)<=0.0) npc_animate(npc);
    
    // Clown and robot do a lot of other stuff, and they are essentially identical.
    if (npc->id0==NS_decal_robot_wrong) {
        npc_update_kid(npc,ftimer,INV_clown,INV_robo);
    } else if (npc->id0==NS_decal_clown_wrong) {
        npc_update_kid(npc,ftimer,INV_robo,INV_clown);
    } else if (npc->id0==NS_decal_ghost_wrong) {
        npc_update_kid(npc,ftimer,INV_crown,0);
    } else if (npc->id0==NS_decal_princess_wrong) {
        npc_update_kid(npc,ftimer,0,INV_crown);
    } else if (npc->id0==NS_decal_pumpkin_wrong) {
        npc_update_kid(npc,ftimer,0,INV_pump);
    
    // Pumpkinhat can get picked up.
    } else if (npc->id0==NS_decal_pumpkinhat) {
        npc_update_pumpkinhat(npc,ftimer);
    }
    
    //TODO motion
}

/* Efface from this plane of existence those beings which are no longer worthy.
 */
 
void reap_defunct_npcs() {
    int i=npcc;
    NPC *npc=npcv+i-1;
    for (;i-->0;npc--) {
        if (!npc->defunct) continue; // Still got the funk.
        // Cleanup? I think not necessary.
        npcc--;
        memmove(npc,npc+1,sizeof(NPC)*(npcc-i));
    }
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
      srcr.width*=-1.0f;
    }
    DrawTexturePro(texture,srcr,dstr,VEC_ZERO,0,WHITE);
}

void clear_all_npcs(){
    int i=npcc;
    NPC *npc=npcv+i-1;
    for (;i-->0;npc--) {
        npc->defunct = 1;
        npcc--;
    }
    reap_defunct_npcs();
    npcc = 0;
}