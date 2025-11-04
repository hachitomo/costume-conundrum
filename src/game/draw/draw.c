#include "draw.h"
#include "raylib.h"
#include "../scene/scene.h"
#include "../constants.h"
#include <stdlib.h>

RenderTexture2D bbuf;

Vector2 zeropos = {
    .x=0,
    .y=0
};
Vector2 screen_origin = {
    .x=0,
    .y=0,
};
Rectangle buffer_rec = {
    .x=0,
    .y=0,
    .width=RENDER_WIDTH,
    .height=-RENDER_HEIGHT,
};

void init_draw(){
    bbuf = LoadRenderTexture(RENDER_WIDTH,RENDER_HEIGHT);
}
void deinit_draw(){
    UnloadTexture(bbuf.texture);
}

void draw_game(Scene *scene){

    BeginTextureMode(bbuf);
        ClearBackground(scene->clearColor);
        // scene draw function calls will all draw to bbuf
        scene->draw();

        // draw static things like UI/HUD here
    EndTextureMode();

    // bbuf -> screen
    BeginDrawing();
        ClearBackground(scene->clearColor);
        DrawTexturePro(bbuf.texture,buffer_rec,buffer_rec,screen_origin,0,WHITE);
    EndDrawing();
}