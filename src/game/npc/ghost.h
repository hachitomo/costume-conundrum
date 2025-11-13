#ifndef GHOST_H
#define GHOST_H

#include "raylib.h"
#include "../map/map.h"

#define GHOST_WRONG 0
#define GHOST_RIGHT 1

#define GHOST_DEFEATED 2
#define GHOST_CURIOUS 3
#define GHOST_HAPPY 4
#define GHOST_WALK 5

void init_ghost(int x,int y);
NPC *get_ghost();
void ghooooost(float ftime);
Rectangle get_ghost_frame(int state,float frame_time);
void draw_ghost();

#endif
