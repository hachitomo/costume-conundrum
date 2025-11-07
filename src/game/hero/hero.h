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
    int state,xtransform,just_updated;
    float state_time;
}Hero;

void init_hero(void);
// void deinit_hero(void);
Hero *get_hero(void);
void set_hero_state(int state);
void update_hero(Hero *hero, Scene *scene, Inputs inputs);
void draw_hero(Hero *hero);
Rectangle get_hero_frame(int state, float state_time);
Rectangle hero_bbox(Hero *hero);
// SpriteAnimation *get_hero_animations();

static const int HERO_WIDTH = 34;
static const int HERO_HEIGHT = 54;
static const int STATE_IDLE = 0;
static const int STATE_WALK = 1;
static const int STATE_JUMP = 2;
static const int STATE_FALL = 3;

#endif