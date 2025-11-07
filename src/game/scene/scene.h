
#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"

typedef struct Scene{
    int id;
    Color clearColor;
    // Texture2D terrain;
    void (*draw)(struct Scene*);
}Scene;

extern Scene SCENE_MENU;
extern Scene SCENE_GAME;
extern Scene SCENE_END;

void init_menu();
void set_scene(Scene *scene);
Scene *get_current_scene();

void run_scene_menu(Scene *scene);
void run_scene_game(Scene *scene);
void run_scene_end(Scene *scene);
void draw_scene_game(Scene *scene);

#endif
