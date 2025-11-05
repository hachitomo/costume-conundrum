#include "scene.h"
#include "../data.h"
#include "../frame_timer.h"
#include "../input/input.h"
#include <stdio.h>

Scene *current_scene;
Image testimg;
Texture2D testtex;

void set_scene(Scene *scene){
    current_scene = scene;
}

Scene SCENE_MENU = {
    .id=0,
    .clearColor=ORANGE,
    .draw=draw_scene_menu,
};

Scene SCENE_GAME = {
    .id=1,
    .clearColor=ORANGE,
    .draw=draw_scene_game,
};

Scene SCENE_END = {
    .id=2,
    .clearColor=ORANGE,
    .draw=draw_scene_end,
};

Scene *get_current_scene(void){
    return current_scene;
}

void draw_scene_menu(Scene *scene){
    FrameTimer *timer = get_frame_timer();
    DrawText("Costume",160,90,80,PURPLE);
    DrawText("Conundrum",105,175,80,PURPLE);
    if(timer->global_framect%2 == 0){
       DrawText("Press anything!",270,250,18,BLUE);
    }
    Inputs inputs = get_inputs();
    if(inputs.left|inputs.down|inputs.right|inputs.up|inputs.interact){
        set_scene(&SCENE_GAME);
    }
}

void draw_scene_game(Scene *scene){
    DrawText("Wow, it's the game!!!",160,90,42,PURPLE);
    DrawText("AMAZING!",105,175,32,PURPLE);
}

void draw_scene_end(Scene *scene){

}
