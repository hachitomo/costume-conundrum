#include "raylib.h"
#include "hero.h"
#include "../data.h"
#include "../audio/audio.h"
#include "../constants.h"
#include "../frame_timer.h"
#include "../input/input.h"
#include "../scene/scene.h"
#include "../sprite/sprite.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void reset_colliders();

Sprite hero_sprite = {0};
Image hero_img;
Texture2D hero_tex;
float HERO_ANIM_RATE = 0.125;
Solid hero_colliders[100];
int collidersc = 0;
Actor hero_actor = {
    .position={
        .x=GAME_START_X,
        .y=GAME_START_Y,
        .width=17,
        .height=27,
    },
    .velocity=VEC_ZERO,
};

Hero hero = {
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
    hero_sprite.texture = LoadTextureFromImage(hero_img);
    hero_sprite.get_animation_frame = get_hero_frame;
    hero_sprite.xtransform=1;
    hero.sprite=hero_sprite;
    hero.actor=hero_actor;
    if(hero_colliders == NULL) {
        // idk, burn everything down?
        printf("Oh no, no memory for colliders I guess");
    }
};

void deinit_hero(void){
    UnloadTexture(hero_sprite.texture);
};

Hero *get_hero(void){
    return &hero;
}

void set_hero_state(int state){
    if(hero.state == state){
        hero.just_updated = 1;
        return;
    }
    hero.state = state;
    hero.state_time = 0.0;
    hero.just_updated = 0;
}

#define LEFT -1
#define RIGHT 1
#define accel 15
#define decel 20
#define airdecel 7
#define driftreduce 20
#define topspeed 40
#define gravity 0.35

int solidc = 0;

void update_hero(Hero *hero, Scene *scene, Inputs inputs){
    FrameTimer *ftimer = get_frame_timer();
    Vector2 newvel = hero->actor.velocity;

	if(inputs.left){
		int acc = accel;
		if(newvel.x > 0){
			acc = acc + driftreduce;
		}
		newvel.x -= acc * ftimer->frame_time;
		if(newvel.x < -topspeed){
			newvel.x = -topspeed;
		}
		if(!inputs.right){
			hero->xtransform = LEFT;
		}
	}

	if(inputs.right){
		float acc = accel;
		if(newvel.x < 0){
			acc = acc + driftreduce;
		}
		newvel.x += acc * ftimer->frame_time;
		if(newvel.x > topspeed){
			newvel.x = topspeed;
		}
		if(!inputs.left){
			hero->xtransform = RIGHT;
		}
	}

	if(!inputs.left && !inputs.right){
		float slow = decel;
		if(!hero->actor.grounded){
			slow = airdecel;
		}
		if(newvel.x > 0){
			newvel.x -= slow * ftimer->frame_time;
		} else if(newvel.x < 0){
			newvel.x += slow * ftimer->frame_time;
		}
		if(fabs(newvel.x) < 0.25){
			newvel.x = 0;
		}
	}

    if(hero->just_updated==0){
        hero->state_time = 0;
        hero->just_updated = 1;
    }else{
        hero->state_time += ftimer->frame_time;
    }
    hero->sprite.xtransform = hero->xtransform;
    hero->sprite.state = hero->state;

    int widthtiles = (int)(RENDER_WIDTH / TILE_SIZE);
    int heighttiles = (int)(RENDER_HEIGHT / TILE_SIZE);
    Rectangle physics_zone = {
        .x=(int)((hero->actor.position.x / TILE_SIZE) - widthtiles*0.5),
        .y=(int)((hero->actor.position.y / TILE_SIZE) - heighttiles*0.5),
        .width=widthtiles,
        .height=heighttiles,
    };
    solidc = solids_in_selection(hero_colliders,physics_zone,100);
    move_actor(hero->actor,hero_colliders,solidc);
    reset_colliders();
}

void reset_colliders(){
    for(int i=0; i<collidersc; i++){
        hero_colliders[i].physics = TILE_VISUAL;
        hero_colliders[i].position.x = 0;
        hero_colliders[i].position.y = 0;
        hero_colliders[i].position.width = 1;
        hero_colliders[i].position.height = 1;
    }
    collidersc = 0;
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
    result.x = (int)hero->actor.position.x;
    result.y = (int)hero->actor.position.y;
    result.width = (int)HERO_WIDTH;
    result.height = (int)HERO_HEIGHT;
    return result;
}
