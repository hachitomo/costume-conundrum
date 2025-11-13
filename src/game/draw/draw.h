#ifndef DRAW_H
#define DRAW_H

#include "raylib.h"
#include "../constants.h"
#include "../scene/scene.h"

#define TEXTURE_TERRAIN 2
#define TEXTURE_SPRITES 3
#define TEXTURE_LOGO 4
#define TEXTURE_ORBIS_FIXE 5
#define TEXTURE_CLOUDS1 6
#define TEXTURE_CLOUDS2 7
#define TEXTURE_CLOUDS3 8
#define TEXTURE_BGOVER 9
#define TEXTURE_FINALE 10
#define TEXTURE_FEAST 11

void init_draw();
void deinit_draw();


// top-level control; called from main
void draw_game(Scene *);
Texture2D get_texture(int texture);
Font get_font(void);
void load_font_from_decalsheet(void);
Rectangle get_anim_frame_rec(const struct decal sheet[], int frame);

#endif
