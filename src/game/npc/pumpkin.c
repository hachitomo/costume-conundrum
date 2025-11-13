#include "pumpkin.h"
#include "raylib.h"
#include "../audio/audio.h"
#include "../draw/draw.h"
#include "../hero/hero.h"
#include "../map/map.h"
#include "../shared_symbols.h"
#include <stdio.h>


// area pumpkin will 'notice' you
Rectangle pumpkin_detect = {0};
Rectangle pumpkin_hitbox = {0};
NPC pumpkin = {0};
Sprite pumpkin_sprite = {0};

NPC *get_pumpkin(){
    return &pumpkin;
}

void init_pumpkin(int x, int y){
    pumpkin.state = PUMPKIN_WRONG;
    pumpkin.just_updated = 0;
        
    pumpkin_sprite.texture = get_texture(TEXTURE_SPRITES);
    pumpkin_sprite.state = PUMPKIN_WRONG;
    pumpkin_sprite.xtransform = 1;
    pumpkin_sprite.framesc=8;
    pumpkin_sprite.frate=0.125;
    pumpkin_sprite.get_animation_frame=&get_pumpkin_frame;
    pumpkin.width = decalsheet_sprites[NS_decal_pumpkin_wrong].w;
    pumpkin.height = decalsheet_sprites[NS_decal_pumpkin_wrong].h;
    pumpkin.position.x = (int)(x*TILE_SIZE - 0.5 * pumpkin.width);
    pumpkin.position.y = (int)(y*TILE_SIZE - 0.5 * pumpkin.height)+1;

    pumpkin_detect.x=pumpkin.position.x - 32;
    pumpkin_detect.y=pumpkin.position.y - 32;
    pumpkin_detect.width=96;
    pumpkin_detect.height=64;

    pumpkin_hitbox.x=pumpkin.position.x;
    pumpkin_hitbox.y=pumpkin.position.y;
    pumpkin_hitbox.width=pumpkin.width;
    pumpkin_hitbox.height=pumpkin.height;

    pumpkin.id = CMD_map_pumpkin;
    pumpkin.sprite = &pumpkin_sprite;
}

// just like the arcade game
void pump_it_up(float ftime){
    if(pumpkin.state == PUMPKIN_WRONG){
        Hero *hero = get_hero();
        if(get_inventory(INV_pump) && CheckCollisionRecs(hero->bbox,pumpkin_hitbox)){
            // give pumpkin hat
            set_inventory(INV_pump,0);
            PlaySound(success_sound());
            pumpkin.state = PUMPKIN_RIGHT;
            pumpkin.sprite->state = PUMPKIN_HAPPY;
            pumpkin.just_updated = 1;
        }else if(CheckCollisionRecs(hero->bbox,pumpkin_detect)){
            pumpkin.sprite->state = PUMPKIN_QUERY;
            pumpkin.just_updated = 1;
            if(hero->actor.position.x < pumpkin.position.x){
                pumpkin.sprite->xtransform = -1;
            }else {
                pumpkin.sprite->xtransform = 1;
            }
        } else {
            pumpkin.sprite->state = PUMPKIN_SAD;
            pumpkin.just_updated = 1;
        }
    } else {
        pumpkin.sprite->state = PUMPKIN_HAPPY;
        pumpkin.just_updated = 1;
    }
}

Rectangle get_pumpkin_frame(int state,float state_time){
    Hero *hero = get_hero();
    switch(state){
        case PUMPKIN_SAD:
            return get_anim_frame_rec(decalsheet_sprites,NS_decal_pumpkin_wrong);
        case PUMPKIN_QUERY:
            return get_anim_frame_rec(decalsheet_sprites,NS_decal_pumpkin_query);
        case PUMPKIN_HAPPY:
            return get_anim_frame_rec(decalsheet_sprites,NS_decal_pumpkin_right);
        case PUMPKIN_WALK:
            // todo: walk cycle
            return get_anim_frame_rec(decalsheet_sprites,NS_decal_pumpkin_walk1);
        default:
            printf("Fallback - pumpkin state not detected: %d]\nDisplaying idle.\n",state);
            return get_anim_frame_rec(decalsheet_sprites,NS_decal_pumpkin_walk1);
    }
}

void draw_pumpkin(){
    NPC *pumpkin = get_pumpkin();
    Rectangle dest = {
        .x=pumpkin->position.x,
        .y=pumpkin->position.y,
        .width=pumpkin->width,
        .height=pumpkin->height,
    };
    draw_sprite(pumpkin->sprite,dest,pumpkin->state_time);
}