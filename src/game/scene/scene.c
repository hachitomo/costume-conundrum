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
    .x=44,
    .y=10,
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

static int TEMP_framec=0;

static void draw_sky_layer(int texid,int speed) {
    int dstx = -((speed * TEMP_framec) % RENDER_WIDTH);
    Texture2D *texture = get_texture(texid);
    DrawTexture(*texture,dstx,0,WHITE);
    if (dstx) DrawTexture(*texture,dstx+RENDER_WIDTH,0,WHITE);
}

void run_scene_menu(Scene *scene){
    FrameTimer *timer = get_frame_timer();
    Texture2D *logotex = get_texture(TEXTURE_LOGO);
    
    // sky
    TEMP_framec++;
    draw_sky_layer(TEXTURE_ORBIS_FIXE,0);
    draw_sky_layer(TEXTURE_CLOUDS1,1);
    draw_sky_layer(TEXTURE_CLOUDS2,2);
    draw_sky_layer(TEXTURE_CLOUDS3,3);
    draw_sky_layer(TEXTURE_BGOVER,0);
    
    DrawTextureEx(*logotex,LOGO_OFFSET,0,2,WHITE);
    float time = GetTime();
    if(time - floor(time) < 0.8){
       DrawText("Press anything!",115,125,24,PURPLE);
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
    
    // sky
    TEMP_framec++;
    draw_sky_layer(TEXTURE_ORBIS_FIXE,0);
    draw_sky_layer(TEXTURE_CLOUDS1,1);
    draw_sky_layer(TEXTURE_CLOUDS2,2);
    draw_sky_layer(TEXTURE_CLOUDS3,3);
    draw_sky_layer(TEXTURE_BGOVER,0);

    // draw
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
