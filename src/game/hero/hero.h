// manage position, animation state, inventory
#ifndef HERO_H
#define HERO_H

#include "raylib.h"
#include "../input/input.h"
#include "../physics/physics.h"
#include "../scene/scene.h"
#include "../sprite/sprite.h"

#define INV_crown   1
#define INV_clown   2
#define INV_robo    3
#define INV_pump    4

typedef struct Hero{
    Actor actor;
    Sprite sprite;
    Rectangle bbox;
    int state,xtransform,just_updated,inventory;
    float state_time;
}Hero;

void init_hero(void);
void reinit_hero(void);
// void deinit_hero(void);
Hero *get_hero(void);
void set_hero_state(int state);
void update_hero(Hero *hero, Scene *scene, Inputs inputs);
void draw_hero(Hero *hero);
Rectangle get_hero_frame(int state, float state_time);
// Rectangle hero_bbox(Hero *hero);
void set_inventory(int item, int value);
int get_inventory(int item);
// SpriteAnimation *get_hero_animations();

static const int HERO_WIDTH = 17;
static const int HERO_HEIGHT = 27;
static const int STATE_IDLE = 0;
static const int STATE_WALK = 1;
static const int STATE_JUMP = 2;
static const int STATE_FALL = 3;

#endif
