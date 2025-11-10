#include "map.h"
#include "raylib.h"
#include "../data.h"
#include "../draw/draw.h"
#include <stdio.h>

Map game_map;
Rectangle areas[256];
Tilesheet map_tilesheet;

void init_map(){
    // simulate load tilesheet by id
    map_tilesheet.sourcetex=TEXTURE_TERRAIN;
    map_tilesheet.terraintex=get_texture(TEXTURE_TERRAIN);
    map_tilesheet.width=16;
    map_tilesheet.height=16;

    game_map.terrain=map_tilesheet;
    game_map.areas=areas;
    game_map.tiles=map;
    game_map.width=map_w;
    game_map.height=map_h;
}

Rectangle get_tile_rect(int tileid){
    int srcx=(tileid&15)*TILE_SIZE;
    int srcy=(tileid>>4)*TILE_SIZE;
    Rectangle result = {
        .x=srcx,
        .y=srcy,
        .width=TILE_SIZE,
        .height=TILE_SIZE,
    };
    return result;
}

void draw_tile(Texture2D tex, int tileid, Rectangle dest){
    if(tileid < 0 || tileid>map_tilesheet.width*map_tilesheet.height){
        printf("WARNING! Tried to draw tile with invalid id: %d",tileid);
        return;
    }
    Rectangle source = get_tile_rect(tileid);
    DrawTexturePro(tex,source,dest,VEC_ZERO,0,WHITE);
}

void draw_map(){
    for(int y=0; y<game_map.height; y++){
        for(int x=0; x<game_map.width;x++){
            Rectangle dest = {
                .x=x*TILE_SIZE,
                .y=y*TILE_SIZE,
                .width=TILE_SIZE,
                .height=TILE_SIZE,
            };
            int tile = (map_w * y) + x;
            draw_tile(game_map.terrain.terraintex,game_map.tiles[tile],dest);
        }
    }
}

void draw_map_selection(Rectangle selection){
    #define warn(param,value) printf("WARNING: clamping invalid selection selection.%s in draw_map_selection: %f",param,value);
    if(selection.x < 0){
        warn("x",selection.x)
        selection.x = 0;
    }
    if(selection.x > game_map.width-2){
        warn("x",selection.x)
        selection.x = game_map.width-2;
    }
    if(selection.y < 0){
        warn("y",selection.y)
        selection.y = 0;
    }
    if(selection.y > game_map.height-1){
        warn("y",selection.y)
        selection.y = game_map.height-1;
    }
    if(selection.width < 1){
        warn("width",selection.width)
        selection.width = 1;
    }
    if(selection.width > game_map.width-1-selection.x){
        warn("width",selection.width)
        selection.width = game_map.width-1-selection.x;
    }
    if(selection.height < 1){
        warn("height",selection.height)
        selection.height = 1;
    }
    if(selection.height > game_map.height-1-selection.y){
        warn("height",selection.height)
        selection.height = game_map.height-1-selection.y;
    }
    for(int y=selection.y; y<game_map.height; y++){
        for(int x=selection.x; x<game_map.width;x++){
            Rectangle dest = {
                .x=x*TILE_SIZE,
                .y=y*TILE_SIZE,
                .width=TILE_SIZE,
                .height=TILE_SIZE,
            };
            int tile = (map_w * y) + x;
            draw_tile(game_map.terrain.terraintex,game_map.tiles[tile],dest);
        }
    }
}