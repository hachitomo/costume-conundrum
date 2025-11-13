#include "ghost.h"
#include "raylib.h"
#include "../audio/audio.h"
#include "../draw/draw.h"
#include "../hero/hero.h"
#include "../map/map.h"
#include "../shared_symbols.h"
#include <stdio.h>

#define GHOST_WRONG 0
#define GHOST_RIGHT 1

#define GHOST_DEFEATED 2
#define GHOST_CURIOUS 3
#define GHOST_HAPPY 4
#define GHOST_WALK 5

// area ghost will 'notice' you
Rectangle ghost_detect = {
    .x=336,
    .y=216,
    .height=112,
    .width=128,
};
Rectangle ghost_hitbox = {
    .x=360,
    .y=280,
    .height=33,
    .width=28,
};
NPC ghost = {0};
Sprite ghost_sprite = {0};

NPC *get_ghost(){
    return &ghost;
}

void init_ghost(){
    ghost.state = GHOST_WRONG;
    ghost.just_updated = 0;
    
    ghost_sprite.texture = get_texture(TEXTURE_SPRITES);
    ghost_sprite.state = GHOST_DEFEATED;
    ghost_sprite.xtransform = 1;
    ghost_sprite.framesc=8;
    ghost_sprite.frate=0.125;
    ghost_sprite.get_animation_frame=&get_ghost_frame;
    ghost.position.x = map_poiv[2].x*TILE_SIZE;
    ghost.position.y = map_poiv[2].y*TILE_SIZE;
    ghost.width = decalsheet_sprites[NS_decal_ghost_right].w;
    ghost.height = decalsheet_sprites[NS_decal_ghost_right].h;
    ghost.sprite = &ghost_sprite;
}

// Please note:
// ooooOOOoooo
// this is how the ghost doooOOOOoooo
void ghooooost(float ftime){
    if(ghost.state == GHOST_WRONG){
        Hero *hero = get_hero();
        if(CheckCollisionRecs(hero_bbox(hero),ghost_hitbox)){
            // retrieve crown
            set_inventory(INV_crown,1);
            ghost.state = GHOST_RIGHT;
            PlaySound(get_sound(SOUND_PICKUP));
        }else if(CheckCollisionRecs(hero_bbox(hero),ghost_detect)){
            ghost.sprite->state = GHOST_CURIOUS;
            ghost.just_updated = 1;
            if(hero->actor.position.x < ghost.position.x){
                ghost.sprite->xtransform = -1;
            }else{
                ghost.sprite->xtransform = 1;
            }
        }else{
            ghost.sprite->state = GHOST_DEFEATED;
            ghost.just_updated = 1;
        }
    }else{
        ghost.sprite->state = GHOST_HAPPY;
    }
}

Rectangle get_ghost_frame(int state,float state_time){
    Hero *hero = get_hero();
    switch(state){
        case GHOST_DEFEATED:
            return get_anim_frame_rec(decalsheet_sprites,NS_decal_ghost_wrong);
        case GHOST_CURIOUS:
            if(hero->actor.position.y+hero->actor.position.height < ghost.position.y){
                return get_anim_frame_rec(decalsheet_sprites,NS_decal_ghost_query_up);
            }else{
                return get_anim_frame_rec(decalsheet_sprites,NS_decal_ghost_query);
            }
        case GHOST_HAPPY:
            return get_anim_frame_rec(decalsheet_sprites,NS_decal_ghost_right);
        case GHOST_WALK:
            // todo: walk cycle
            return get_anim_frame_rec(decalsheet_sprites,NS_decal_ghost_walk1);
    }
    printf("Fallback - ghost state not detected: %d]\nDisplaying idle.\n",state);
    return get_anim_frame_rec(decalsheet_sprites,NS_decal_ghost_walk1);
}
