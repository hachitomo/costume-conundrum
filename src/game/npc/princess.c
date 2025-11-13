#include "princess.h"
#include "raylib.h"
#include "../audio/audio.h"
#include "../draw/draw.h"
#include "../hero/hero.h"
#include "../map/map.h"
#include "../shared_symbols.h"
#include <stdio.h>


// area princess will 'notice' you
Rectangle princess_detect = {
    // .x=336,
    // .y=216,
    // .height=112,
    // .width=128,
};
Rectangle princess_hitbox = {
    // .x=360,
    // .y=280,
    // .height=33,
    // .width=28,
};
NPC princess = {0};
Sprite princess_sprite = {0};

NPC *get_princess(){
    return &princess;
}

void init_princess(int x, int y){
    princess.state = PRINCESS_WRONG;
    princess.just_updated = 0;
        
    princess_sprite.texture = get_texture(TEXTURE_SPRITES);
    princess_sprite.state = PRINCESS_WRONG;
    princess_sprite.xtransform = 1;
    princess_sprite.framesc=8;
    princess_sprite.frate=0.125;
    princess_sprite.get_animation_frame=&get_princess_frame;
    princess.position.x = x*TILE_SIZE;
    princess.position.y = y*TILE_SIZE;
    princess.width = decalsheet_sprites[NS_decal_princess_wrong].w;
    princess.height = decalsheet_sprites[NS_decal_princess_wrong].h;

    princess_detect.x=princess.position.x - 48;
    princess_detect.y=princess.position.y - 38;
    princess_detect.width=128;
    princess_detect.height=64;

    princess_hitbox.x=princess.position.x;
    princess_hitbox.y=princess.position.y;
    princess_hitbox.width=princess.width;
    princess_hitbox.height=princess.height;

    princess.id = CMD_map_princess;
    princess.sprite = &princess_sprite;
}

// pinkies up
void act_royalty(float ftime){
    if(princess.state == PRINCESS_WRONG){
        Hero *hero = get_hero();
        if(get_inventory(INV_crown) && CheckCollisionRecs(hero->bbox,princess_hitbox)){
            set_inventory(INV_crown,0);
            PlaySound(success_sound());
            princess.state = PRINCESS_RIGHT;
            princess.sprite->state = PRINCESS_HAPPY;
        }else if(CheckCollisionRecs(hero->bbox,princess_detect)){
            princess.sprite->state = PRINCESS_QUERY;
            if(hero->actor.position.x < princess.position.x){
                princess.sprite->xtransform = -1;
            }else {
                princess.sprite->xtransform = 1;
            }
        }
    }
}


Rectangle get_princess_frame(int state,float state_time){
    Hero *hero = get_hero();
    switch(state){
        case PRINCESS_WRONG:
            return get_anim_frame_rec(decalsheet_sprites,NS_decal_princess_wrong);
        case PRINCESS_QUERY:
            return get_anim_frame_rec(decalsheet_sprites,NS_decal_princess_query);
        case PRINCESS_RIGHT:
            return get_anim_frame_rec(decalsheet_sprites,NS_decal_princess_right);
        case PRINCESS_WALK:
            // todo: walk cycle
            return get_anim_frame_rec(decalsheet_sprites,NS_decal_princess_walk1);
    }
    printf("Fallback - princess state not detected: %d]\nDisplaying idle.\n",state);
    return get_anim_frame_rec(decalsheet_sprites,NS_decal_princess_walk1);
}

void draw_princess(){
    NPC *princess = get_princess();
    Rectangle dest = {
        .x=princess->position.x,
        .y=princess->position.y,
        .width=princess->width,
        .height=princess->height,
    };
    draw_sprite(princess->sprite,dest,princess->state_time);
}