#ifndef GHOST_H
#define GHOST_H

#include "raylib.h"
#include "../map/map.h"

void init_ghost();
NPC *get_ghost();
void ghooooost(float ftime);
Rectangle get_ghost_frame(int state,float frame_time);

#endif