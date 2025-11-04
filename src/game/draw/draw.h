//

#ifndef DRAW_H
#define DRAW_H

#include "raylib.h"
#include "../constants.h"
#include "../scene/scene.h"

void init_draw();
void deinit_draw();

// top-level control; called from main
void draw_game(Scene *);

// Raylib wrappers to draw to back buffer
// int DrawToBuffer(void);

#endif
