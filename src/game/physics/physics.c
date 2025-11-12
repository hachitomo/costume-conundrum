#include "physics.h"
#include "raylib.h"
#include "../constants.h"
#include "../frame_timer.h"
#include "../map/map.h"
#include <math.h>
#include <stdio.h>

Actor base_actor = {
    .position={
        .x=0,
        .y=0,
        .width=1,
        .height=1
    },
    .velocity=VEC_ZERO,
    .grounded=0,
    .collidersc=0,
    .collidersmax=COLLIDERS_DEFAULT_MAX
};

Actor init_actor(){
    return base_actor;
}


float vector_magnitude(Vector2 vec) {
	return (float)sqrt(
		pow((float)vec.x, 2) + pow((float)vec.y, 2)
	);
}


void move_actor(Actor *actor, Solid *colliders, int collidersc){
    FrameTimer *ftimer = get_frame_timer();
    Rectangle newpos = {
        .x=actor->position.x+actor->velocity.x,
        .y=actor->position.y+actor->velocity.y,
        .width=actor->position.width,
        .height=actor->position.height,
    };
    CollisionFix fixes[10] = {0};
    int fixesc = 0;
    CollisionFix *fixiter = fixes;
    Solid *collider = colliders;
    Vector2 finalnudge = VEC_ZERO;

    for(int i=0; i<collidersc; i++){
        float actormaxy = newpos.y+newpos.height;
        float collidermaxy = collider->position.y+collider->position.height;
        float actormaxx = newpos.x+newpos.width;
        float collidermaxx = collider->position.x+collider->position.width;
        int passed_hor_pos = actormaxx <= collider->position.x && newpos.x >= collidermaxx ? 1 : 0;
        int passed_hor_neg = actor->position.x >= collidermaxx && actormaxx <= collider->position.x ? 1 : 0;
        int passed_ver_pos = actormaxy <= collider->position.y && newpos.y >= collidermaxy ? 1 : 0;
        int passed_ver_neg = actor->position.y >= collidermaxy && actormaxy <= collider->position.y ? 1 : 0;

        if(CheckCollisionRecs(newpos,collider->position)){
            actor_append_collider(*actor,collider);
            CollisionFix fix = {
                .collider=collider,
                .magnitude=0,
                .nudge=VEC_ZERO,
                .clip=0,
            };
            if(collider->physics == TILE_PHYSICAL){
                if(collidermaxx > newpos.x && actormaxx > collidermaxx){
                    fix.nudge.x = collidermaxx - newpos.x;
                }
                if(collider->position.x < actormaxx && newpos.x < collider->position.x){
                    fix.nudge.x = collider->position.x - actormaxx;
                }
                if(collidermaxy >= newpos.y && actormaxy > collidermaxy){
                    fix.nudge.y = collidermaxy - newpos.y;
                }
                if(collider->position.y < actormaxy && newpos.y < collider->position.y){
                    fix.nudge.y = collider->position.y - actormaxy;
                }
            }
            if(collider->physics == TILE_ONE_WAY && actor->velocity.y > 0 && collider->position.y >= actor->position.y+actor->position.height){
                fix.nudge.y = collider->position.y - actormaxy;
            }

            if(fix.nudge.x != 0 || fix.nudge.y != 0){
                float absx = fabs(fix.nudge.x);
                float absy = fabs(fix.nudge.y);
                if(absx != 0 && absy != 0){
                    if(absx < absy){
                        fix.nudge.y = 0;
                    }else {
                        fix.nudge.x = 0;
                    }
                }
            }
            fix.magnitude = vector_magnitude(fix.nudge);
            *fixiter = fix;
            fixiter++;
            fixesc++;
        } else if(passed_ver_neg || passed_ver_pos || passed_hor_neg || passed_hor_pos){
            actor_append_collider(*actor,collider);
            fixesc++;
            CollisionFix fix = {
                .collider=collider,
                .magnitude=0,
                .nudge=VEC_ZERO,
                .clip=1,
            };
			
            if(passed_hor_pos) {
				fix.nudge.x = -((newpos.x + actor->position.width) - collider->position.x);
			} else if(passed_hor_neg) {
				fix.nudge.x = collidermaxx - actor->position.x;
			}
            if(passed_hor_pos) {
				fix.nudge.y = -((newpos.y + actor->position.height) - collider->position.y);
			} else if(passed_hor_neg) {
				fix.nudge.y = collidermaxy - actor->position.y;
			}
			fix.magnitude = vector_magnitude(fix.nudge);

            *fixiter = fix;
            fixiter++;
            fixesc++;
        }
        collider++;
    }

    if(fixesc == 0){
        actor->position = newpos;
        return;
    } else if(fixesc == 1){
        if(fixes->nudge.x != 0 && fixes->nudge.y != 0){
            if(fabs(fixes->nudge.x) > fabs(fixes->nudge.y)){
                fixes->nudge.x = 0;
            } else{
                fixes->nudge.y = 0;
            }
        }
        if(fixes->nudge.x != 0){
            actor->velocity.x = 0;
        }
        if(fixes->nudge.y != 0){
            actor->velocity.y = 0;
            if(fixes->nudge.y < 0){
                actor->grounded = 1;
            }
        }
        finalnudge = fixes->nudge;
    } else {
        CollisionFix *result;
        float smallesty = 0;
        float smallestx = 0;
        for(int i=0; i<fixesc;i++){
            CollisionFix *fixiter = fixes+i;
            float yfix = fabs(fixiter->nudge.y);
            float xfix = fabs(fixiter->nudge.x);
            if(yfix > 0 && (yfix < fabs(smallesty) || smallesty == 0)){
                smallesty = fixiter->nudge.y;
            }
            if(xfix > 0 && (xfix < fabs(smallestx) || smallestx == 0)){
                smallestx = fixiter->nudge.x;
            }
        }
        finalnudge.y += smallesty;
        finalnudge.x += smallestx;
    }

    if(finalnudge.y < 0){
        actor->grounded = 1;
    }
    if(finalnudge.x != 0){
        actor->velocity.x = 0;
    }

    actor->position.x = newpos.x + finalnudge.x;
    actor->position.y = newpos.y + finalnudge.y;
}

void actor_append_collider(Actor actor, Solid *collider){
    for(int i=0; i<actor.collidersc;i++){
        if(&actor.colliders[i] == collider){
            return;
        }
    }
    actor.colliders[actor.collidersc] = *collider;
    actor.collidersc++;
}

void actor_clear_colliders(Actor actor){
    for(int i=0;i<actor.collidersc; i++){
        actor.colliders[i].position = base_actor.position;
        actor.colliders[i].physics = TILE_VISUAL;
    }
}