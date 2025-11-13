#ifndef SPRITE_H
#define SPRITE_H

#include "raylib.h"


typedef struct Sprite{
    Texture2D texture;
    int state,xtransform,framesc;
    float frate;
    Rectangle (*get_animation_frame)(int state, float state_time);
}Sprite;


// given a sprite object and how long it's been in its current state,
// area of the sprite texture do we clip to draw this frame?
// Rectangle animation_frame(Sprite *sprite,float time);
void draw_sprite(Sprite *sprite,Rectangle dest,float state_time);
int get_animation_frame(float animtime, float rate,int framec);

#endif