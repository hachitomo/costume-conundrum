// manage position, animation state, inventory
#ifndef HERO_H
#define HERO_H

#include "raylib.h"
#include "../input/input.h"
#include "../scene/scene.h"
#include "../sprite/sprite.h"

typedef struct Hero{
    Vector2 position,velocity;
    Sprite sprite;
    int state,direction,just_updated;
    float state_time;
}Hero;

void init_hero(void);
// void deinit_hero(void);
Hero *get_hero(void);
void set_hero_state(int state);
void update_hero(Hero *hero, Scene *scene, Inputs inputs);
void draw_hero(Hero *hero);
Rectangle hero_bbox(Hero *hero);
SpriteAnimation *get_hero_animations();

static const int HERO_WIDTH = 17;
static const int HERO_HEIGHT = 27;
static const int STATE_IDLE = 0;
static const int STATE_WALK = 1;
static const int STATE_AIRBORN = 2;

// vvv--ANIMATION DEFINITIONS--vvv

// idle
static const Rectangle hero_idle_frames[1]={
    {.x=1,
    .y=1,
    .width=17,
    .height=27}
};
static const SpriteAnimation hero_idle = {
    .frames=hero_idle_frames,
    .framesc=1,
    .duration=0.0,
    .time=0.0,
};

// walk
static const Rectangle hero_walk_frames[8]={
    {.x=38,
    .y=1,
    .width=16,
    .height=26},
    {.x=56,
    .y=1,
    .width=17,
    .height=27},
    {.x=74,
    .y=1,
    .width=16,
    .height=26},
    {.x=92,
    .y=1,
    .width=16,
    .height=26},
    {.x=110,
    .y=1,
    .width=16,
    .height=26},
    {.x=128,
    .y=1,
    .width=16,
    .height=26},
    {.x=146,
    .y=1,
    .width=16,
    .height=26},
    {.x=164,
    .y=1,
    .width=16,
    .height=26},
};
static const SpriteAnimation hero_walk = {
    .frames=hero_walk_frames,
    .framesc=8,
    .duration=0.125,
    .time=0.0,
};

// jump (airborn up)
// static const Rectangle jump_frame={
//     .x=1,
//     .y=1,
//     .width=17,
//     .height=27
// };
// static const SpriteAnimation jump = {
//     .duration=0,
//     .frames=&jump_frame,
//     .framesc=1,
//     .duration=0,
// };

// fall (airborn down)
// static const Rectangle jump_frame={
//     .x=1,
//     .y=1,
//     .width=17,
//     .height=27
// };
// static const SpriteAnimation jump = {
//     .duration=0,
//     .frames=&jump_frame,
//     .framesc=1,
//     .duration=0,
// };

static const SpriteMap hero_sprite_map = {
    .animations={
        hero_idle,
        hero_walk,
    },
    .animationc=2,
};

#endif