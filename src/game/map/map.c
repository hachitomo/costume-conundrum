#include "map.h"
#include "raylib.h"
#include "../data.h"
#include "../frame_timer.h"
#include "../shared_symbols.h"
#include "../draw/draw.h"
#include "../npc/ghost.h"
#include "../npc/npc.h"
#include "../npc/princess.h"
#include "../npc/pumpkin.h"
#include "../physics/physics.h"
#include <stdio.h>

Map game_map;
Rectangle areas[256];
Tilesheet map_tilesheet;
int npcs[7] = {0};
int npcc = 0;

// act will set NPC state, just_updated, position, etc. for each sprite
void update_npcs(){
    FrameTimer *ftimer = get_frame_timer();
    ghooooost(ftimer->frame_time);
    act_royalty(ftimer->frame_time);
    pump_it_up(ftimer->frame_time);
    
    NPC *p;
    int c=get_npcs(&p);
    for (;c-->0;p++) {
      update_npc(p,ftimer);
    }
}

void draw_npcs(){
    for (int i=0;i<npcc;i++) {
        int npcid = npcs[i];
        switch (npcid) {
            case CMD_map_ghost:
                draw_ghost();
                break;
            case CMD_map_princess:
                draw_princess();
                break;
            case CMD_map_pumpkin:
                draw_pumpkin();
                break;
                // TODO others
            default:
                return;
        }
    }
    
    NPC *p;
    int c=get_npcs(&p);
    for (;c-->0;p++) {
      draw_npc(p);
    }
}

void init_npcs(){
    const struct map_poi *poi=map_poiv;
    int i=map_poic;
    for (;i-->0;poi++) {
        switch (poi->cmd) {
            case CMD_map_ghost: 
                init_ghost(poi->x,poi->y);
                npcs[npcc] = get_ghost()->id;
                npcc++;
                break;
            case CMD_map_princess: 
                init_princess(poi->x,poi->y);
                npcs[npcc] = get_princess()->id;
                npcc++;
                break;
            case CMD_map_pumpkin: 
                init_pumpkin(poi->x,poi->y);
                npcs[npcc] = get_pumpkin()->id;
                npcc++;
                break;
            case CMD_map_robot: init_npc(poi->x,poi->y,NS_decal_robot_wrong); break;
            case CMD_map_clown: init_npc(poi->x,poi->y,NS_decal_clown_wrong); break;
            case CMD_map_lightbear: init_npc(poi->x,poi->y,NS_decal_lightbear_dig1); break;
            case CMD_map_cat: init_npc(poi->x,poi->y,NS_decal_cat1); break;
            case CMD_map_jack: init_npc(poi->x,poi->y,NS_decal_jack1); break;
            case CMD_map_pumpkinhat: init_npc(poi->x,poi->y,NS_decal_pumpkinhat); break;
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
