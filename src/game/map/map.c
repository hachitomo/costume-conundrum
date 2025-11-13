#include "map.h"
#include "raylib.h"
#include "../data.h"
#include "../frame_timer.h"
#include "../shared_symbols.h"
#include "../draw/draw.h"
#include "../npc/ghost.h"
#include "../npc/npc.h"
#include "../physics/physics.h"
#include <stdio.h>

Map game_map;
Rectangle areas[256];
Tilesheet map_tilesheet;

// static void init_npc(NPC *npc){
//     switch(npc->id){
//         case CMD_map_ghost:
//             init_ghost();
//             break;
//         case CMD_map_princess:
//             break;
//         case CMD_map_pumpkin:
//             break;
//         case CMD_map_robot:
//             break;
//         case CMD_map_clown:
//             break;
//         case CMD_map_lightbear:
//             break;
//         case CMD_map_cat:
//             break;
//         case CMD_map_jack:
//             break;
//         case CMD_map_pumpkinhat:
//             break;
//         default:
//             break;
//     }
// }

// act will set NPC state, just_updated, position, etc. for each sprite
void update_npcs(){
    FrameTimer *ftimer = get_frame_timer();
    ghooooost(ftimer->frame_time);
}

void draw_npcs(){
    // for(int i=0; i<map_poic; i++){
    // for(int i=0; i<1; i++){
        NPC *ghost = get_ghost();
        Rectangle dest = {
            .x=ghost->position.x,
            .y=ghost->position.y,
            .width=ghost->width,
            .height=ghost->height,
        };
        draw_sprite(ghost->sprite,dest,ghost->state_time);
    // }
}

void init_npcs(){
    // NPC *iter = npcs;
    // for(int i=0; i<map_poic; i++){
    // for(int i=2; i<3; i++){
    // npcs->position.x = map_poiv[2].x * TILE_SIZE;
    // npcs->position.y = map_poiv[2].y * TILE_SIZE;
    // npcs->id = map_poiv[2].cmd;
        // iter->position.x = map_poiv[i].x * TILE_SIZE;
        // iter->position.y = map_poiv[i].y * TILE_SIZE;
        // iter->id = map_poiv[i].cmd;
    // init_npc(33);
        // iter++;
    // }
    const struct map_poi *poi=map_poiv;
    int i=map_poic;
    for (;i-->0;poi++) {
        switch (poi->cmd) {
            case CMD_map_hero: break;//TODO put dot here
            case CMD_map_ghost: init_ghost(poi->x,poi->y); break;
            case CMD_map_princess: break;//TODO
            case CMD_map_pumpkin: break;//TODO
            case CMD_map_robot: break;//TODO
            case CMD_map_clown: break;//TODO
            case CMD_map_lightbear: break;//TODO
            case CMD_map_cat: break;//TODO
            case CMD_map_jack: break;//TODO
            case CMD_map_pumpkinhat: break;//TODO
        }
    }
}

void init_map(){
    // simulate load tilesheet by id
    map_tilesheet.sourcetex=TEXTURE_TERRAIN;
    map_tilesheet.terraintex=get_texture(TEXTURE_TERRAIN);
    map_tilesheet.width=16;
    map_tilesheet.height=16;
    map_tilesheet.tiles=tilesheet_terrain;

    game_map.terrain=map_tilesheet;
    game_map.areas=areas;
    game_map.tiles=map;
    game_map.width=map_w;
    game_map.height=map_h;
    init_npcs();
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
    if(selection.x < 0){
        selection.x = 0;
    }
    if(selection.x > game_map.width-1){
        selection.x = game_map.width-1;
    }
    if(selection.y < 0){
        selection.y = 0;
    }
    if(selection.y > game_map.height-1){
        selection.y = game_map.height-1;
    }
    if(selection.width < 1){
        selection.width = 1;
    }
    if(selection.width > game_map.width-1-selection.x){
        selection.width = game_map.width-1-selection.x;
    }
    if(selection.height < 1){
        selection.height = 1;
    }
    if(selection.height > game_map.height-1-selection.y){
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

int solids_in_selection(Solid *out, Rectangle selection, int maxitems){
    int resultc = 0;
    Solid *iter = out;

    if(selection.x < 0){
        selection.x = 0;
    }
    if(selection.x > game_map.width-2){
        selection.x = game_map.width-2;
    }
    if(selection.y < 0){
        selection.y = 0;
    }
    if(selection.y > game_map.height-1){
        selection.y = game_map.height-1;
    }
    if(selection.width < 1){
        selection.width = 1;
    }
    if(selection.width > game_map.width-selection.x){
        selection.width = game_map.width-selection.x;
    }
    if(selection.height < 1){
        selection.height = 1;
    }
    if(selection.height > game_map.height-selection.y){
        selection.height = game_map.height-selection.y;
    }

    for(int y = selection.y; y < selection.y+selection.height; y++){
        if(y >= map_h) break;
        for(int x = selection.x; x < selection.x+selection.width; x++){
            if(x >= map_w) break;
            if(resultc >= maxitems) {
                printf("Warning! Found max colliders (%d)",maxitems);
                return resultc;
            }
            int tilecoord = (map_w * y) + x;
            if(tilecoord > map_w * map_h) {
                printf("Unexpected tileid out of range %d",tilecoord);
                return resultc;
            }
            int tileid = game_map.tiles[tilecoord];
            int physics = tilesheet_terrain[tileid];
            if(physics == TILE_VISUAL) {
                continue;
            } // not a solid
            int length=1;

            //int length = 1;
            //int one_way = physics == 2 ? 0 : 1;
            //printf("x: %d, y: %d is solid\n",x,y);
            int neighborsnext=0;
            int neighborid = game_map.tiles[tilecoord+1];
            int neighborphysics = tilesheet_terrain[neighborid];
            while(x+neighborsnext < map_w && x+neighborsnext < selection.x+selection.width && neighborphysics == physics){
                neighborsnext++;
                neighborid = game_map.tiles[tilecoord+neighborsnext+1];
                neighborphysics = tilesheet_terrain[neighborid];
                length++;
            }
            iter->position.x=x*TILE_SIZE;
            iter->position.y=y*TILE_SIZE;
            iter->position.width=length*TILE_SIZE;
            iter->position.height=TILE_SIZE;
            iter->physics=physics;
            if(iter->physics==TILE_ONE_WAY){
                iter->position.height = 4;
            }
            resultc++;
            x+=length-1;
            iter++;
        }
    }

    return resultc;
}
