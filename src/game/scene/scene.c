#include "scene.h"
#include "rlgl.h"
#include "../constants.h"
#include "../data.h"
#include "../frame_timer.h"
#include "../audio/audio.h"
#include "../camera/camera.h"
#include "../hero/hero.h"
#include "../input/input.h"
#include "../draw/draw.h"
#include <stdio.h>
#include <math.h>

Color SKYCOLOR = { 42, 60, 91, 255 };
static Vector2 LOGO_OFFSET = {
    .x=32,
    .y=10,
};
static Vector2 PROMPT_OFFSET = {.x=115,.y=125};
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
    .clearColor=WHITE,
    .draw=run_scene_menu,
};

Scene SCENE_GAME = {
    .id=1,
    .clearColor=WHITE,
    .draw=run_scene_game,
};

Scene SCENE_END = {
    .id=2,
    .clearColor=WHITE,
    .draw=run_scene_end,
};

Scene *get_current_scene(void){
    return current_scene;
}

static void draw_sky_layer(int texid,int speed,double now) {
    const double base_speed = 50.0; // px/s for the slowest layer.
    int dstx = -((int)(speed * now * base_speed) % RENDER_WIDTH);
    Texture2D texture = get_texture(texid);
    DrawTexture(texture,dstx,0,WHITE);
    if (dstx) DrawTexture(texture,dstx+RENDER_WIDTH,0,WHITE);
}

void run_scene_menu(Scene *scene){
    Music menu_song = get_song(SONG_MENU);
    bool playing = IsMusicStreamPlaying(menu_song);
    if(!playing){
        PlayMusicStream(menu_song);
    }
    UpdateMusicStream(menu_song);
    FrameTimer *timer = get_frame_timer();
    Texture2D logotex = get_texture(TEXTURE_LOGO);
    ClearBackground(WHITE);
    
    // sky
    draw_sky_layer(TEXTURE_ORBIS_FIXE,0,timer->total);
    draw_sky_layer(TEXTURE_CLOUDS1,1,timer->total);
    draw_sky_layer(TEXTURE_CLOUDS2,2,timer->total);
    draw_sky_layer(TEXTURE_CLOUDS3,3,timer->total);
    draw_sky_layer(TEXTURE_BGOVER,0,timer->total);
    
    DrawTextureEx(logotex,LOGO_OFFSET,0,1,WHITE);
    Font font = get_font();
    float time = GetTime();

    if(time - floor(time) < 0.8){
       DrawTextEx(font,"Press anything!",PROMPT_OFFSET,12,1,PURPLE);
    }
    
    Inputs inputs = get_inputs();
    if(inputs.left|inputs.down|inputs.right|inputs.up|inputs.interact){
        set_scene(&SCENE_GAME);
        StopMusicStream(menu_song);
    }
}

Vector2 intvector(Vector2 v){
    Vector2 result = {
        .x=(int)v.x,
        .y=(int)v.y,
    };
    return result;
}

void run_scene_game(Scene *scene){
    Music game_song = get_song(SONG_GAME);
    bool playing = IsMusicStreamPlaying(game_song);
    if(!playing){
        PlayMusicStream(game_song);
    }
    UpdateMusicStream(game_song);
    FrameTimer *timer = get_frame_timer();
    
    // update
    Inputs inputs = get_inputs();
    Hero *hero = get_hero();
    
    update_hero(hero,scene,inputs);

    // update camera 
    Camera2D* camera = get_camera();
    Vector2 playerpos = {
        .x=hero->actor.position.x,
        .y=hero->actor.position.y,
    };
    camera->target = intvector(clamp_camera(playerpos));
    
    
    // sky
    ClearBackground(WHITE);
    BeginBlendMode(BLEND_CUSTOM_SEPARATE);
    rlSetBlendFactorsSeparate(0x0302, 0x0303, 1, 0x0303, 0x8006, 0x8006);
        draw_sky_layer(TEXTURE_ORBIS_FIXE,0,timer->total);
        draw_sky_layer(TEXTURE_CLOUDS1,1,timer->total);
        draw_sky_layer(TEXTURE_CLOUDS2,2,timer->total);
        draw_sky_layer(TEXTURE_CLOUDS3,3,timer->total);
        draw_sky_layer(TEXTURE_BGOVER,0,timer->total);
    EndBlendMode();
    // draw
    BeginMode2D(*camera);
        draw_scene_game(scene);
    EndMode2D();
}

void run_scene_end(Scene *scene){

}

void draw_scene_game(Scene *scene){
    draw_map();
    Hero *hero = get_hero();
    draw_hero(hero);
}
