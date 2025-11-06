#include "scene.h"
#include "../constants.h"
#include "../data.h"
#include "../frame_timer.h"
#include "../camera/camera.h"
#include "../hero/hero.h"
#include "../input/input.h"
#include <stdio.h>

Scene *current_scene;
Image testimg;
Texture2D testtex;

void set_scene(Scene *scene){
    current_scene = scene;
    if(scene->id == SCENE_GAME.id){
        init_camera();
    }
}

Scene SCENE_MENU = {
    .id=0,
    .clearColor=ORANGE,
    .draw=run_scene_menu,
};

Scene SCENE_GAME = {
    .id=1,
    .clearColor=ORANGE,
    .draw=run_scene_game,
};

Scene SCENE_END = {
    .id=2,
    .clearColor=ORANGE,
    .draw=run_scene_end,
};

Scene *get_current_scene(void){
    return current_scene;
}

void run_scene_menu(Scene *scene){
    FrameTimer *timer = get_frame_timer();
    DrawText("Costume",160,90,80,PURPLE);
    DrawText("Conundrum",105,175,80,PURPLE);
    if(timer->global_exetime_sec%2 == 0){
       DrawText("Press anything!",270,250,18,BLUE);
    }
    Inputs inputs = get_inputs();
    if(inputs.left|inputs.down|inputs.right|inputs.up|inputs.interact){
        set_scene(&SCENE_GAME);
    }
}

void run_scene_game(Scene *scene){
    // update
    Inputs inputs = get_inputs();
    Hero *hero = get_hero();
    update_hero(hero,scene,inputs);

    // update camera 
    Camera2D* camera = get_camera();
    //...

    // draw
    BeginMode2D(*camera);
        draw_scene_game(scene);
    EndMode2D();
}

void run_scene_end(Scene *scene){

}

void draw_scene_game(Scene *scene){
    
}
