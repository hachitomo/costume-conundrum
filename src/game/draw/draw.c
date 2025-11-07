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

Texture2D logotex,terraintex,spritestex;
Texture2D orbis_fixetex,clouds1tex,clouds2tex,clouds3tex,bgovertex;

Texture2D *get_texture(int texture){
    switch(texture){
        case TEXTURE_TERRAIN:
            return &terraintex;
        break;
        case TEXTURE_SPRITES:
            return &spritestex;
        break;
        case TEXTURE_LOGO:
            return &logotex;
        break;
        case TEXTURE_ORBIS_FIXE:
            return &orbis_fixetex;
        break;
        case TEXTURE_CLOUDS1:
            return &clouds1tex;
        break;
        case TEXTURE_CLOUDS2:
            return &clouds2tex;
        break;
        case TEXTURE_CLOUDS3:
            return &clouds3tex;
        break;
        case TEXTURE_BGOVER:
            return &bgovertex;
        break;
    }
}

void init_draw(){
    #define LOADIMG(tag) \
      Image tag##img = LoadImageFromMemory(".png",image_##tag,image_##tag##_length); \
      tag##tex = LoadTextureFromImage(tag##img);
    LOADIMG(terrain)
    LOADIMG(sprites)
    LOADIMG(logo)
    LOADIMG(orbis_fixe)
    LOADIMG(clouds1)
    LOADIMG(clouds2)
    LOADIMG(clouds3)
    LOADIMG(bgover)
    #undef LOADIMG
    bbuf = LoadRenderTexture(RENDER_WIDTH,RENDER_HEIGHT);
};

void deinit_draw(){
    UnloadTexture(bbuf.texture);
    UnloadTexture(logotex);
    UnloadTexture(terraintex);
    UnloadTexture(spritestex);
    UnloadTexture(orbis_fixetex);
    UnloadTexture(clouds1tex);
    UnloadTexture(clouds2tex);
    UnloadTexture(clouds3tex);
    UnloadTexture(bgovertex);
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
