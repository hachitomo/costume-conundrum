#ifndef DRAW_H
#define DRAW_H

#include "raylib.h"
#include "../constants.h"
#include "../scene/scene.h"

#define TEXTURE_SKY 1
#define TEXTURE_TERRAIN 2
#define TEXTURE_SPRITES 3
#define TEXTURE_LOGO 4

void init_draw();
void deinit_draw();


// top-level control; called from main
void draw_game(Scene *);
Texture2D *get_texture(int texture);

// Raylib wrappers to draw to back buffer
// int DrawToBuffer(void);

#endif
