#ifndef PHYSICS_H
#define PHYSICS_H

#include "raylib.h"

#define COLLIDERS_DEFAULT_MAX 5

// An (usually) immovable solid object that can be collided with like terrain
typedef struct Solid{
    Rectangle   position;
    int         physics;
}Solid;

// A solid locomotive object, like the player or NPCs
typedef struct Actor{
    Rectangle   position;
    Solid       colliders[10];
    Vector2     velocity;
    int        grounded,collidersc,collidersmax;
}Actor;

typedef struct CollisionFix{
    Solid   *collider;
    Vector2 nudge;
    float   magnitude;
    int clip;
}CollisionFix;

Actor init_actor();
void move_actor(Actor actor, Solid *colliders, int colidersc);
void actor_append_collider(Actor actor, Solid *collider);

#endif