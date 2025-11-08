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
float HERO_ANIM_RATE = 0.125;

Hero _hero = {
    .position=GAME_START_POS,
    .velocity=VEC_ZERO,
    .state=STATE_IDLE,
    .xtransform=1,
    .just_updated=1,
    .state_time=0.0f,
};

// vvv--ANIMATION DEFINITIONS--vvv

// idle
// SpriteAnimation hero_idle = {
//     .frames={
//         {.x=1,
//         .y=1,
//         .width=17,
//         .height=27}
//     },
//     .framesc=1,
//     .duration=0.0,
//     .time=0.0,
// };

// idle
Rectangle idle_frames[]={
    {.x=1,
    .y=1,
    .width=17,
    .height=27}
};

// walk
Rectangle walk_frames[]={
    {.x=37,
    .y=1,
    .width=17,
    .height=27},
    {.x=55,
    .y=1,
    .width=17,
    .height=27},
    {.x=73,
    .y=1,
    .width=17,
    .height=27},
    {.x=91,
    .y=1,
    .width=17,
    .height=27},
    {.x=109,
    .y=1,
    .width=17,
    .height=27},
    {.x=127,
    .y=1,
    .width=17,
    .height=27},
    {.x=145,
    .y=1,
    .width=17,
    .height=27},
    {.x=163,
    .y=1,
    .width=17,
    .height=27},
};
// SpriteAnimation hero_walk = {
//     .frames={
//         {.x=37,
//         .y=1,
//         .width=17,
//         .height=27},
//         {.x=55,
//         .y=1,
//         .width=17,
//         .height=27},
//         {.x=73,
//         .y=1,
//         .width=17,
//         .height=27},
//         {.x=91,
//         .y=1,
//         .width=17,
//         .height=27},
//         {.x=109,
//         .y=1,
//         .width=17,
//         .height=27},
//         {.x=127,
//         .y=1,
//         .width=17,
//         .height=27},
//         {.x=145,
//         .y=1,
//         .width=17,
//         .height=27},
//         {.x=163,
//         .y=1,
//         .width=17,
//         .height=27},
//     },
//     .framesc=7,
//     .duration=0.125,
//     .time=0.0,
// };

// jump (airborn up)
Rectangle jump_frames[]={
    {
    .x=181,
    .y=1,
    .width=17,
    .height=27
    },{
    .x=217,
    .y=1,
    .width=17,
    .height=27
    },
};
// SpriteAnimation hero_jump = {
//     .duration=0.125,
//     .frames={
//         {
//         .x=181,
//         .y=1,
//         .width=17,
//         .height=27
//         },{
//         .x=217,
//         .y=1,
//         .width=17,
//         .height=27
//         },
//     },
//     .framesc=2,
//     .time=0.0,
// };

// fall (airborn down)
Rectangle fall_frames[]={
    {
    .x=199,
    .y=1,
    .width=17,
    .height=27
    },{
    .x=235,
    .y=1,
    .width=17,
    .height=27
    },
};
// SpriteAnimation hero_fall = {
//     .duration=0.125,
//     .frames={
//         {
//         .x=199,
//         .y=1,
//         .width=17,
//         .height=27
//         },{
//         .x=235,
//         .y=1,
//         .width=17,
//         .height=27
//         },
//     },
//     .framesc=2,
//     .time=0.0,
// };

// SpriteMap hero_sprite_map = {
//     .animations={
//         hero_idle,
//         hero_walk,
//         hero_jump,
//         hero_fall,
//     },
//     .animationc=4,
// };


void init_hero(void){
    hero_img = LoadImageFromMemory(".png",image_sprites,image_sprites_length);
    _hero_sprite.texture = LoadTextureFromImage(hero_img);
    _hero_sprite.get_animation_frame = get_hero_frame;
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
        hero->velocity.x = -75;
        hero->xtransform = -1;
        set_hero_state(STATE_WALK);
    }else if(!inputs.left && inputs.right){
        hero->velocity.x = 75;
        hero->xtransform = 1;
        set_hero_state(STATE_WALK);
    }else{
        hero->velocity.x=0;
        set_hero_state(STATE_IDLE);
    }
    
    if(hero->position.y >=100){
        if(inputs.up){
            hero->velocity.y = -125;
            set_hero_state(STATE_JUMP);
        }else{
            hero->velocity.y = 0;
            hero->position.y = 100;
        }
    }else if(hero->position.y < 100){
        hero->velocity.y += 4;
        if(hero->velocity.y > 0){
            set_hero_state(STATE_FALL);
        }else{
            set_hero_state(STATE_JUMP);
        }
    }
    
    hero->position.x += hero->velocity.x * ftimer->frame_time;
    hero->position.y += hero->velocity.y * ftimer->frame_time;

    // temp. physics check here... nothing to collide with for now
    // except fake bounds at the edge of the creen
    if(hero->position.x <=8){
        hero->position.x = 8;
    }
    if(hero->position.x >=312){
        hero->position.x = 312;
    }

    if(hero->just_updated==0){
        hero->state_time = 0;
        hero->just_updated = 1;
    }else{
        hero->state_time += ftimer->frame_time;
    }
    hero->sprite.xtransform = hero->xtransform;
    hero->sprite.state = hero->state;
}

void draw_hero(Hero *hero){
    draw_sprite(&hero->sprite,hero_bbox(hero),hero->state_time);
}

int hero_framesc[] = {1,8,2,2};

Rectangle get_hero_frame(int state,float state_time){
    if(state == 0){
        return idle_frames[0];
    }
    if(state > 3){
        printf("tried to get hero frame length OOB %d",state);
        return idle_frames[0];
    }
    int frame = get_animation_frame(state_time,HERO_ANIM_RATE,hero_framesc[state]);
    switch(state){
        case 1:
            return walk_frames[frame];
        case 2:
            return jump_frames[frame];
        case 3:
            return fall_frames[frame];
    }
    printf("Fallback - hero state not detected: %d]\nDisplaying idle.\n",state);
    return idle_frames[0];
}

Rectangle hero_bbox(Hero *hero){
    Rectangle result = {0};
    result.x = (int)hero->position.x;
    result.y = (int)hero->position.y;
    result.width = (int)HERO_WIDTH;
    result.height = (int)HERO_HEIGHT;
    return result;
}
