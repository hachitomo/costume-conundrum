
#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"

typedef struct Scene{
    int id;
    Color clearColor;
    // Texture2D terrain;
    void (*draw)();
}Scene;

extern Scene SCENE_MENU;
extern Scene SCENE_GAME;
extern Scene SCENE_END;

void set_scene(Scene *);
Scene *get_current_scene();

void draw_scene_menu(Scene *);
void draw_scene_game(Scene *);
void draw_scene_end(Scene *);

#endif