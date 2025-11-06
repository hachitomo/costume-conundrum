#include "raylib.h"
#include "hero.h"
#include "../data.h"
#include "../constants.h"
#include "../frame_timer.h"
#include "../input/input.h"
#include "../scene/scene.h"
#include "../sprite/sprite.h"

Sprite _hero_sprite = {0};
Image hero_img;
Texture2D hero_tex;

Hero _hero = {
    .position=GAME_START_POS,
    .velocity=VEC_ZERO,
    .state=STATE_IDLE,
    .direction=1,
    .just_updated=1,
    .state_time=0.0f,
};

void init_hero(void){
    hero_img = LoadImageFromMemory(".png",image_sprites,image_sprites_length);
    _hero_sprite.texture = LoadTextureFromImage(hero_img);
    _hero_sprite.sprite_map = &hero_sprite_map;
    _hero_sprite.direction=1;
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
        return;
    }
    _hero.state = state;
    _hero.state_time = 0.0;
    _hero.just_updated = 0;
}

void update_hero(Hero *hero, Scene *scene, Inputs inputs){
    FrameTimer *ftimer = get_frame_timer();
    if(inputs.left && !inputs.right){
        hero->velocity.x = -10;
        hero->direction = -1;
        set_hero_state(STATE_WALK);
    }else if(!inputs.left && inputs.right){
        hero->velocity.x = 10;
        hero->direction = 1;
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
        hero->state_time=ftimer->global_exetime_sec;
        hero->just_updated = 1;
    }
    hero->sprite.position = hero->position;
    hero->sprite.direction = hero->direction;
    update_sprite_state(&hero->sprite,hero->state,hero->state_time);
    draw_hero(hero);
}

// animation_time - how long hero's been in this state
// this value % SpriteAnimation.duration gives us the anim frame
void draw_hero(Hero *hero){
    draw_sprite(&hero->sprite,hero->state_time);
}

Rectangle hero_bbox(Hero *hero){
    Rectangle result = {0};
    result.x = hero->position.x - (HERO_WIDTH*0.5);
    result.y = hero->position.y - (HERO_HEIGHT*0.5);
    result.width = HERO_WIDTH;
    result.height = HERO_HEIGHT;
    return result;
}
