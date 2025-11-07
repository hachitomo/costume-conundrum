#include "raylib.h"
#include "hero.h"
#include "../data.h"
#include "../constants.h"
#include "../frame_timer.h"
#include "../input/input.h"
#include "../scene/scene.h"
#include "../sprite/sprite.h"
#include <stdio.h>

Sprite _hero_sprite = {0};
Image hero_img;
Texture2D hero_tex;

Hero _hero = {
    .position=GAME_START_POS,
    .velocity=VEC_ZERO,
    .state=STATE_IDLE,
    .xtransform=1,
    .just_updated=1,
    .state_time=0.0f,
};

void init_hero(void){
    hero_img = LoadImageFromMemory(".png",image_sprites,image_sprites_length);
    _hero_sprite.texture = LoadTextureFromImage(hero_img);
    _hero_sprite.sprite_map = &hero_sprite_map;
    _hero_sprite.xtransform=1;
    _hero.sprite=_hero_sprite;
};

void deinit_hero(void){
    UnloadTexture(_hero_sprite.texture);
};

Hero *get_hero(void){
    return &_hero;
}

void set_hero_state(int state){
    if(_hero.state == state){
        _hero.just_updated = 1;
        return;
    }
    _hero.state = state;
    _hero.state_time = 0.0;
    _hero.just_updated = 0;
}

void update_hero(Hero *hero, Scene *scene, Inputs inputs){
    FrameTimer *ftimer = get_frame_timer();
    if(inputs.left && !inputs.right){
        hero->velocity.x = -125;
        hero->xtransform = -1;
        set_hero_state(STATE_WALK);
    }else if(!inputs.left && inputs.right){
        hero->velocity.x = 125;
        hero->xtransform = 1;
        set_hero_state(STATE_WALK);
    }else{
        hero->velocity.x=0;
        set_hero_state(STATE_IDLE);
    }
    hero->position.x += hero->velocity.x * ftimer->frame_time;

    // physics check here... nothing to collide with for now
    // except fake bounds at the edge of the creen
    if(hero->position.x <=8){
        hero->position.x = 8;
    }
    if(hero->position.x >=600){
        hero->position.x = 600;
    }

    if(hero->just_updated==0){
        hero->state_time = 0;
        hero->just_updated = 1;
    }else{
        hero->state_time += ftimer->frame_time;
    }
    hero->sprite.destination = hero_bbox(hero);
    hero->sprite.xtransform = hero->xtransform;
    hero->sprite.state = hero->state;
}

void draw_hero(Hero *hero){
    draw_sprite(&hero->sprite,hero->state_time);
}

Rectangle hero_bbox(Hero *hero){
    Rectangle result = {0};
    result.x = (int)hero->position.x;
    result.y = (int)hero->position.y;
    result.width = (int)HERO_WIDTH;
    result.height = (int)HERO_HEIGHT;
    return result;
}
