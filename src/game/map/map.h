#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include "../data.h"
#include "../physics/physics.h"
#include "../sprite/sprite.h"

#define TILE_SIZE 16

#define TILE_VISUAL     0
#define TILE_PHYSICAL   1
#define TILE_ONE_WAY    2

typedef const unsigned char *Tiles;

// Generic def of Map NPCs
typedef struct NPC{
    Vector2 position; // initialized from map_poiv, updated with act
    Sprite *sprite;
    int id0,id,state,xtransform,just_updated;
    float state_time,width,height;
    //Tilesheet *tilesheet // assumed to be sprites.png for game jam, but could set here
    int animframe;
    double animclock;
    int argv[4]; // usage depends on (id0)
    int defunct;
}NPC;

typedef struct Tilesheet{
    Texture2D terraintex;
    int sourcetex,width,height;
    Tiles tiles;
}Tilesheet;

typedef struct Map{
    Rectangle *areas;
    Tilesheet terrain;
    NPC *npcs;
    const unsigned char *tiles;
    int areasc,npcc,width,height;
}Map;

void init_map();
Rectangle get_tile_rect(int tileid);
void draw_map(); // draw the entire map (suitable for small maps)
void draw_map_selection(Rectangle coords); // rectangle of map coords! NOT world space
int solids_in_selection(Solid *out, Rectangle coords, int maxitems); // generate colliders in given selection 

void init_npcs();
void update_npcs();
void draw_npcs();

#endif
