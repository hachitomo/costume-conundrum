#ifndef PUMPKIN_H
#define PUMPKIN_H

#include "raylib.h"
#include "../map/map.h"

#define PUMPKIN_WRONG 0
#define PUMPKIN_RIGHT 1

#define PUMPKIN_SAD   2
#define PUMPKIN_HAPPY 3
#define PUMPKIN_QUERY 4
#define PUMPKIN_WALK  5
#define PUMPKIN_WALK2 6
#define PUMPKIN_WALK3 7
#define PUMPKIN_WALK4 8

void init_pumpkin(int x, int y);
NPC *get_pumpkin();
void pump_it_up(float ftime);
Rectangle get_pumpkin_frame(int state,float frame_time);
void draw_pumpkin();

#endif