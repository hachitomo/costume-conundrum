#include "raylib.h"
#include "hero.h"
#include "../data.h"
#include "../constants.h"
#include "../shared_symbols.h"
#include "../audio/audio.h"
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
Solid hero_colliders[100];
int collidersc = 0;

int crown   = 0;
int clown   = 0;
int robo    = 0;
int pump    = 0;

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

// idle
Rectangle idle_frames[]={
    {.x=1,
    .y=1,
    .width=17,
    .height=27}
};

// walk
int walk_frames[]={
    NS_decal_dot_walk1,
    NS_decal_dot_walk2,
    NS_decal_dot_walk3,
    NS_decal_dot_walk4,
    NS_decal_dot_walk5,
    NS_decal_dot_walk6,
    NS_decal_dot_walk7,
    NS_decal_dot_walk8,
};

// jump (airborn up)
int jump_frames[]={NS_decal_dot_jump1,NS_decal_dot_jump2};

// fall (airborn down)
int fall_frames[]={NS_decal_dot_fall1,NS_decal_dot_fall2};


void init_hero(void){
    hero_img = LoadImageFromMemory(".png",image_sprites,image_sprites_length);
    hero_sprite.texture = LoadTextureFromImage(hero_img);
    hero_sprite.get_animation_frame = get_hero_frame;
    hero_sprite.xtransform=1;
    hero_sprite.frate=0.125;
    hero.sprite=hero_sprite;
    hero.actor=hero_actor;
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
#define accel 5
#define decel 7
#define airdecel 4
#define driftreduce 2
#define topspeed 15
#define gravity 3.5
#define maxgravity 8

int solidc = 0;

void update_hero(Hero *hero, Scene *scene, Inputs inputs){
    FrameTimer *ftimer = get_frame_timer();
    Vector2 newvel = hero->actor.velocity;
    newvel.y += gravity * ftimer->frame_time;
    if(newvel.y > maxgravity){
        newvel.y = maxgravity;
    }

	if(inputs.left){
		float acc = accel;
		if(newvel.x > 0){
			acc = acc + driftreduce;
		}
		newvel.x -= acc * ftimer->frame_time;
		if(newvel.x < -topspeed){
			newvel.x = -topspeed;
		}
		if(newvel.x > topspeed){
			newvel.x = topspeed;
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
    hero->actor.velocity.x = newvel.x;
    hero->actor.velocity.y = newvel.y;

    if(hero->just_updated==0){
        hero->state_time = 0;
        hero->just_updated = 1;
    }else{
        hero->state_time += ftimer->frame_time;
    }
    hero->sprite.xtransform = hero->xtransform;

    int widthtiles = (int)(RENDER_WIDTH / TILE_SIZE);
    int heighttiles = (int)(RENDER_HEIGHT / TILE_SIZE);
    Rectangle physics_zone = {
        .x=(int)((hero->actor.position.x / TILE_SIZE) - widthtiles*0.5),
        .y=(int)((hero->actor.position.y / TILE_SIZE) - heighttiles*0.5),
        .width=widthtiles,
        .height=heighttiles + 5,
    };
    solidc = solids_in_selection(hero_colliders,physics_zone,100);
    move_actor(&hero->actor,hero_colliders,solidc);

    if(!hero->actor.grounded){
        if(newvel.y > 0){
            hero->state = STATE_FALL;
        }else{
            hero->state = STATE_JUMP;
        }
    } else {
        if((!inputs.left && inputs.right) || (inputs.left && !inputs.right)){
            hero->state = STATE_WALK;
        }
        if(!inputs.left && !inputs.right){
            hero->state = STATE_IDLE;
        }
        if(inputs.jump){
            hero->actor.velocity.y = -2.5;
            hero->actor.grounded = 0;
            hero->state = STATE_JUMP;
            PlaySoundVolume(SOUND_JUMP,0.5);
        }
    }
    hero->sprite.state = hero->state;

    // reset_colliders();
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
    if(crown){
        Texture2D tex = hero->sprite.texture;
        Rectangle crown = {
            .x=21,
            .y=29,
            .width=13,
            .height=6,
        };
        Vector2 offset;
        offset.x = hero->sprite.xtransform == 1 ? 2 : 5;
        offset.y = hero->sprite.xtransform == 1 ? 4 : -5;
        Rectangle crownpos = {
            .x=(int)(hero->actor.position.x + offset.x),
            .y=(int)(hero->actor.position.y + offset.y),
            .width=13,
            .height=6,
        };
        float rotation = hero->sprite.xtransform == 1 ? 315 : 45;
        DrawTexturePro(tex,crown,crownpos,VEC_ZERO,rotation,WHITE);
    }
    /*
    for(int i=0; i<solidc; i++){
        Color color = hero_colliders[i].physics == TILE_PHYSICAL ? LIME : ORANGE;
        DrawRectangleLines(
            (int)hero_colliders[i].position.x,
            (int)hero_colliders[i].position.y,
            (int)hero_colliders[i].position.width,
            (int)hero_colliders[i].position.height,
            color
        );
        if(hero_colliders[i].physics == TILE_ONE_WAY){
            DrawRectangleLines(
                (int)hero_colliders[i].position.x+2,
                (int)hero_colliders[i].position.y+6,
                (int)hero_colliders[i].position.width-4,
                (int)hero_colliders[i].position.height,
                color
            );
        }
    }
    DrawRectangleLinesEx(hero_bbox(get_hero()),1,RED);
    */
    reset_colliders();
}

int hero_framesc[] = {1,8,2,2};

Rectangle get_sprite_frame_info(const struct decal sheet[], int frame){
    Rectangle result = {
        .x=sheet[frame].x,
        .y=sheet[frame].y,
        .width=sheet[frame].w,
        .height=sheet[frame].h
    };
    return result;
}

Rectangle get_hero_frame(int state,float state_time){
    if(state == 0){
        return get_sprite_frame_info(decalsheet_sprites,NS_decal_dot_idle);
    }
    if(state > 3){
        printf("tried to get hero frame length OOB %d",state);
        return get_sprite_frame_info(decalsheet_sprites,NS_decal_dot_idle);
    }
    int frame = get_animation_frame(state_time,hero_sprite.frate,hero_framesc[state]);
    switch(state){
        case 1:
            return get_sprite_frame_info(decalsheet_sprites,walk_frames[frame]);
        case 2:
            return get_sprite_frame_info(decalsheet_sprites,jump_frames[frame]);
        case 3:
            return get_sprite_frame_info(decalsheet_sprites,fall_frames[frame]);
    }
    printf("Fallback - hero state not detected: %d]\nDisplaying idle.\n",state);
    return get_sprite_frame_info(decalsheet_sprites,NS_decal_dot_idle);
}

Rectangle hero_bbox(Hero *hero){
    Rectangle result = {0};
    result.x = (int)hero->actor.position.x;
    result.y = (int)hero->actor.position.y;
    result.width = (int)HERO_WIDTH;
    result.height = (int)HERO_HEIGHT;
    return result;
}


void set_inventory(int item, int value){
    switch(item){
        case INV_crown:
            crown = value;
            break;
        case INV_clown:
            clown = value;
            break;
        case INV_robo:
            robo = value;
            break;
        case INV_pump:
            pump = value;
            break;
    }
}

int get_inventory(int item){
     switch(item){
        case INV_crown:
            return crown;
        case INV_clown:
            return clown;
        case INV_robo:
            return robo;
        case INV_pump:
            return pump;
    }
    return 0;
}