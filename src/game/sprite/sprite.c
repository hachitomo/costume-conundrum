#include "sprite.h"
#include "../constants.h"
#include "../hero/hero.h"
#include <stdio.h>

RenderTexture2D _sprite_buf;
int init = 1;

void draw_sprite(Sprite *sprite,float state_time){
    int state = sprite->state;
    const SpriteMap *map = sprite->sprite_map;
    SpriteAnimation anim = map->animations[state];
    int frameid = animation_frame(&anim,state_time);
    Rectangle frame = anim.frames[frameid];
    frame.width *= sprite->xtransform;
    DrawTexturePro(sprite->texture,frame,sprite->destination,VEC_ZERO,0,WHITE);
};

// given how long in this state, which frame should we draw?
int animation_frame(SpriteAnimation *anim,float time){
    if(anim->duration==0){
        return 0;
    }
    int frame = 0;
    while(time > (anim->duration * anim->framesc)){
        time -= anim->duration * anim->framesc;
    }
    while(frame * anim->duration < time){
        frame++;
    };
    return frame;
};