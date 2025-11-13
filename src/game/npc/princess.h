#ifndef PRINCESS_H
#define PRINCESS_H

#include "raylib.h"
#include "../map/map.h"

#define PRINCESS_WRONG 0
#define PRINCESS_RIGHT 1

#define PRINCESS_SAD   2
#define PRINCESS_HAPPY 3
#define PRINCESS_QUERY 4
#define PRINCESS_WALK  5
#define PRINCESS_WALK2 6
#define PRINCESS_WALK3 7
#define PRINCESS_WALK4 8

void init_princess(int x, int y);
NPC *get_princess();
void act_royalty(float ftime);
Rectangle get_princess_frame(int state,float frame_time);
void draw_princess();

#endif