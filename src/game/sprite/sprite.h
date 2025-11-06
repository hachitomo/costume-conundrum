#ifndef SPRITE_H
#define SPRITE_H

#include "raylib.h"

// duration - length of each frame of the animation, in secs
// 0 for static (no movement)
typedef struct SpriteAnimation{
    const Rectangle *frames;
    int framesc;
    float time,duration;
}SpriteAnimation;

// map state int to different animations
typedef struct SpriteMap{
    SpriteAnimation animations[2];
    int animationc;
}SpriteMap;

// state_time: how long since the state last changed, in seconds
typedef struct Sprite{
    Texture2D texture;
    Rectangle destination;
    int state,xtransform;
    const SpriteMap *sprite_map;
}Sprite;


void draw_sprite(Sprite *sprite,float state_time);
// void move_sprite(Vector2 position);
int animation_frame(SpriteAnimation *anim,float time);

#endif