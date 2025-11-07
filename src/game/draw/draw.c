#include "draw.h"
#include "raylib.h"
#include "../data.h"
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
Rectangle output_rec = {
    .x=0,
    .y=0,
    .width=RENDER_WIDTH*2,
    .height=-RENDER_HEIGHT*2,
};

Texture2D skytex,logotex,terraintex,spritestex;

Texture2D *get_texture(int texture){
    switch(texture){
        case TEXTURE_SKY:
            return &skytex;
        break;
        case TEXTURE_TERRAIN:
            return &terraintex;
        break;
        case TEXTURE_SPRITES:
            return &spritestex;
        break;
        case TEXTURE_LOGO:
            return &logotex;
        break;
    }
}

void init_draw(){
    Image skyteximg = LoadImageFromMemory(".png",image_sky,image_sky_length);
    skytex = LoadTextureFromImage(skyteximg);
    Image terrainteximg = LoadImageFromMemory(".png",image_terrain,image_terrain_length);
    terraintex = LoadTextureFromImage(terrainteximg);
    Image spritesteximg = LoadImageFromMemory(".png",image_sprites,image_sprites_length);
    spritestex = LoadTextureFromImage(spritesteximg);
    Image logoteximg = LoadImageFromMemory(".png",image_logo,image_logo_length);
    logotex = LoadTextureFromImage(logoteximg);
    bbuf = LoadRenderTexture(RENDER_WIDTH,RENDER_HEIGHT);
};

void deinit_draw(){
    UnloadTexture(bbuf.texture);
    UnloadTexture(skytex);
    UnloadTexture(logotex);
    UnloadTexture(terraintex);
    UnloadTexture(spritestex);
}

void draw_game(Scene *scene){

    BeginTextureMode(bbuf);
        ClearBackground(scene->clearColor);
        // scene draw function calls will all draw to bbuf
        scene->draw(scene);

        // draw static things like UI/HUD here
    EndTextureMode();

    // bbuf -> screen
    BeginDrawing();
        DrawTexturePro(bbuf.texture,buffer_rec,output_rec,screen_origin,0,WHITE);
    EndDrawing();
}