#include "scene.h"
#include "../constants.h"
#include "../data.h"
#include "../frame_timer.h"
#include "../camera/camera.h"
#include "../hero/hero.h"
#include "../input/input.h"
#include "../draw/draw.h"
#include <stdio.h>
#include <math.h>

static Vector2 LOGO_OFFSET = {
    .x=88,
    .y=20,
};
Scene *current_scene;
Rectangle render_bounds = {
    .x=0,
    .y=0,
    .width=RENDER_WIDTH,
    .height=RENDER_HEIGHT,
};


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
    Rectangle sky_clip=render_bounds;
    sky_clip.x=25*GetTime();
    Texture2D *skytex = get_texture(TEXTURE_SKY);
    Texture2D *logotex = get_texture(TEXTURE_LOGO);
    DrawTexturePro(*skytex,sky_clip,render_bounds,VEC_ZERO,0,WHITE);
    DrawTextureEx(*logotex,LOGO_OFFSET,0,4,WHITE);
    float time = GetTime();
    if(time - floor(time) < 0.8){
       DrawText("Press anything!",230,250,24,PURPLE);
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
    Rectangle sky_clip=render_bounds;
    sky_clip.x=25*GetTime();
    Texture2D *skytex = get_texture(TEXTURE_SKY);
    DrawTexturePro(*skytex,sky_clip,render_bounds,VEC_ZERO,0,WHITE);
    // BeginMode2D(*camera);
        draw_scene_game(scene);
    // EndMode2D();
}

void run_scene_end(Scene *scene){

}

void draw_scene_game(Scene *scene){
    Hero *hero = get_hero();
    draw_hero(hero);
}
