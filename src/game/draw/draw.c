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

// how many glyphs? why does font decalsheet seem like it has more than 99? hmmm
Rectangle fontrecs[96];
GlyphInfo fontglyphs[96];
Texture2D logotex,terraintex,spritestex;
Texture2D orbis_fixetex,clouds1tex,clouds2tex,clouds3tex,bgovertex,fonttex,errortex;

int font_codepoints[96] = {
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 
    96, 97, 98, 99, 100,101,102,103,104,105,106,107,108,109,110,111,
    112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,0,
};

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
    return &errortex;
}

Font get_font(){
    Font font = {
        .baseSize=12,
        .glyphCount=96,
        .glyphPadding=0,
        .texture=fonttex,
        .recs=fontrecs,
        .glyphs=fontglyphs,
    };
    return font;
};

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
    LOADIMG(font)
    LOADIMG(error)
    #undef LOADIMG
    load_font_from_decalsheet();
    bbuf = LoadRenderTexture(RENDER_WIDTH,RENDER_HEIGHT);
};

void load_font_from_decalsheet(){
    for(int i=0;i<96;i++){
        Rectangle letter = {
            .x=decalsheet_font[i].x,
            .y=decalsheet_font[i].y,
            .width=decalsheet_font[i].w,
            .height=decalsheet_font[i].h,
        };
        fontrecs[i] = letter;
        GlyphInfo glyph = {
            .value=font_codepoints[i]-1,
            .offsetX=0,
            .offsetY=0,
            .advanceX=0,
        };
        fontglyphs[i] = glyph;
    }
}

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
