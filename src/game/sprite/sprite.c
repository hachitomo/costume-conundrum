#include "sprite.h"
#include "../hero/hero.h"

RenderTexture2D _sprite_buf;
int init = 1;

void update_sprite_state(Sprite *sprite,int newstate, float state_time){
    sprite->state=newstate; // ok if it overwrites, and no need to check
    int animframe=0;
    if(sprite->sprite_map->animations[newstate].duration == 0){
        return;
    }
};

void draw_sprite(Sprite *sprite,float state_time){
    // if(init == 1){
    //     _sprite_buf = LoadRenderTexture(HERO_WIDTH*2,HERO_HEIGHT*2);
    //     init = 0;
    // }
    int state = sprite->state;
    const SpriteMap *map = sprite->sprite_map;
    SpriteAnimation anim = map->animations[state];
    Rectangle frame = anim.frames[0];
    frame.width *= sprite->direction;
    if(sprite->direction<0){
        frame.width += 1;
    }
    DrawTextureRec(sprite->texture,frame,sprite->position,WHITE);
};