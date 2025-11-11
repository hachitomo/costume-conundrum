#include "physics.h"
#include "raylib.h"
#include "../constants.h"
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
    .grounded=1,
    .collidersc=0,
    .collidersmax=COLLIDERS_DEFAULT_MAX
};

Actor init_actor(){
    return base_actor;
}

void move_actor(Actor actor, Solid *colliders, int collidersc){
    Rectangle newpos = {
        .x=actor.position.x,
        .y=actor.position.y+actor.velocity.y, // check Y axis first
        .width=actor.position.width,
        .height=actor.position.height,
    };
    CollisionFix yfixes[10] = {0};
    int yfixesc = 0;
    Solid *collider = colliders;

    // Y rectification 
    for(int i=0; i<collidersc; i++){
        float actormaxy = newpos.y+newpos.height;
        float collidermaxy = collider->position.y+collider->position.height;
        int passed_pos = actormaxy <= collider->position.y && newpos.y >= collidermaxy ? 0 : 1;
        int passed_neg = actor.position.y >= collidermaxy && actormaxy <= collider->position.y ? 0 : 1;

        if(CheckCollisionRecs(newpos,collider->position)){
            actor_append_collider(actor,collider);
            CollisionFix fix = {
                .collider=collider,
                .magnitude=0,
                .nudge=VEC_ZERO,
                .clip=1,
            };
            float ychange = 0;
            if(actor.velocity.y > 0 && collidermaxy >= actormaxy && actormaxy > collider->position.y){
                ychange = -(actormaxy - collider->position.y);
            }
			if(actor.velocity.y < 0 && collider->position.y <= newpos.y && newpos.y < collider->position.y){
				ychange = collidermaxy - newpos.y;
			}
            fix.nudge.y = ychange;
            fix.magnitude = ychange;
            yfixes[i] = fix;
            yfixesc++;
        } else if(passed_neg || passed_pos){
            actor_append_collider(actor,collider);
            yfixesc++;
            CollisionFix fix = {
                .collider=collider,
                .magnitude=0,
                .nudge=VEC_ZERO,
                .clip=0,
            };
			if(passed_pos) {
				fix.nudge.y = -((newpos.y + actor.position.height) - collider->position.y);
			} else if(passed_neg) {
				fix.nudge.y = collidermaxy - actor.position.y;
			}
            fix.magnitude = fabs(fix.nudge.y);
        }

        if(yfixesc == 0){
            actor.grounded = 1;
        } else if(yfixesc == 1){
            newpos.y += yfixes[0].nudge.y;
        } else {
            int smallest = 0;
            int sign = 1;
            for(int i=0; i<yfixesc;i++){
                float yfix = yfixes[i].magnitude;
                if((yfix > 0 && yfix < smallest) || smallest == 0){
                    smallest = yfix;
                    sign = smallest > 0 ? 1 : -1;
                }
            }
            newpos.y += (smallest * sign);
        }
    }

    // X rectification
    newpos.x += actor.velocity.x;
    CollisionFix xfixes[10] = {0};
    int xfixesc = 0;
    collider = colliders;
    for(int i=0; i<collidersc; i++){
        float actormaxx = newpos.x+newpos.width;
        float collidermaxx = collider->position.x+collider->position.width;
        int passed_pos = actormaxx <= collider->position.x && newpos.x >= collidermaxx ? 0 : 1;
        int passed_neg = actor.position.x >= collidermaxx && actormaxx <= collider->position.x ? 0 : 1;

        if(CheckCollisionRecs(newpos,collider->position)){
            actor_append_collider(actor,collider);
            xfixesc++;
            CollisionFix fix = {
                .collider=collider,
                .magnitude=0,
                .nudge=VEC_ZERO,
                .clip=1,
            };
            float ychange = 0;
            if(actor.velocity.x > 0 && collidermaxx >= actormaxx && actormaxx > collider->position.x){
                ychange = -(actormaxx - collider->position.x);
            }
			if(actor.velocity.x < 0 && collider->position.x <= newpos.x && newpos.x < collider->position.x){
				ychange = collidermaxx - newpos.x;
			}
            fix.nudge.x = ychange;
            fix.magnitude = ychange;
        } else if(passed_neg || passed_pos){
            actor_append_collider(actor,collider);
            xfixesc++;
            CollisionFix fix = {
                .collider=collider,
                .magnitude=0,
                .nudge=VEC_ZERO,
                .clip=0,
            };
			if(passed_pos) {
				fix.nudge.x = -((newpos.x + actor.position.width) - collider->position.x);
			} else if(passed_neg) {
				fix.nudge.x = collidermaxx - actor.position.x;
			}
            fix.magnitude = fabs(fix.nudge.x);
        }

        if(xfixesc == 0){
            actor.grounded = 1;
        } else if(xfixesc == 1){
            newpos.x += xfixes[0].nudge.x;
        } else {
            int smallest = 0;
            int sign = 1;
            for(int i=0; i<xfixesc;i++){
                float yfix = xfixes[i].magnitude;
                if((yfix > 0 && yfix < smallest) || smallest == 0){
                    smallest = yfix;
                    sign = smallest > 0 ? 1 : -1;
                }
            }
            newpos.x += (smallest * sign);
        }
    }

    actor.position = newpos;
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