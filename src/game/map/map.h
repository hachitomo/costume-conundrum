#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include "../physics/physics.h"

#define TILE_SIZE 16

#define TILE_VISUAL     0
#define TILE_PHYSICAL   1
#define TILE_ONE_WAY    2

typedef const unsigned char *Tiles;

typedef struct Tilesheet{
    Texture2D terraintex;
    int sourcetex,width,height;
    Tiles tiles;
}Tilesheet;

typedef struct Map{
    Rectangle *areas;
    Tilesheet terrain;
    const unsigned char *tiles;
    int areasc,width,height;
}Map;

void init_map();
Rectangle get_tile_rect(int tileid);
void draw_map(); // draw the entire map (suitable for small maps)
void draw_map_selection(Rectangle coords); // rectangle of map coords! NOT world space
int solids_in_selection(Solid *out, Rectangle coords, int maxitems); // generate colliders in given selection 

#endif