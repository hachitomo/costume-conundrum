#include "sprite.h"
#include "../constants.h"
#include "../hero/hero.h"
#include <math.h>
#include <stdio.h>

RenderTexture2D _sprite_buf;
int init = 1;

void draw_sprite(Sprite *sprite,Rectangle dest,float state_time){
    int state = sprite->state;
    Rectangle frame = sprite->get_animation_frame(sprite->state,state_time);
    frame.width *= sprite->xtransform;
    DrawTexturePro(sprite->texture,frame,dest,VEC_ZERO,0,WHITE);
};

// given how long in this state, which frame should we draw?
// rate = float in s of each frame
int get_animation_frame(float animtime, float rate,int framec){
    float anim_dur = framec * rate;
    float elapsed = animtime;
    while(elapsed >= anim_dur){
        elapsed -= anim_dur;
    }
    if(rate * framec == 0){
        return 0;
    }
    return floor(elapsed / rate);
}