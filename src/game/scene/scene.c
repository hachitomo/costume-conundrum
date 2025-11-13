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
#include "../npc/npc.h"
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

static void draw_sky_layer(int texid,int speed,double now,int xoffset) {
    const double base_speed = 20.0; // px/s for the slowest layer.
    int dstx = -((int)(speed * now * base_speed + ((xoffset*7)>>3)) % RENDER_WIDTH);
    if (dstx>0) dstx-=RENDER_WIDTH;
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
    draw_sky_layer(TEXTURE_ORBIS_FIXE,0,timer->total,0);
    draw_sky_layer(TEXTURE_CLOUDS1,1,timer->total,0);
    draw_sky_layer(TEXTURE_CLOUDS2,2,timer->total,0);
    draw_sky_layer(TEXTURE_CLOUDS3,3,timer->total,0);
    draw_sky_layer(TEXTURE_BGOVER,0,timer->total,0);
    
    DrawTextureEx(logotex,LOGO_OFFSET,0,1,WHITE);
    Font font = get_font();
    float time = GetTime();

    if(time - floor(time) < 0.8){
       DrawTextEx(font,"Press anything!",PROMPT_OFFSET,12,1,PURPLE);
    }
    
    Inputs inputs = get_inputs();
    if(inputs.left|inputs.down|inputs.right|inputs.up|inputs.jump|inputs.interact){
        //set_scene(&SCENE_END);
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

    // check completion
    NPC *npcs;
    int i = 0;
    int npcc = get_npcs(&npcs);
    int complete = 1;
    for(; i<npcc; i++,npcs++){
        if (npcs->argv[1]==1) {
            if (npcs->argv[0]!=2) {
                complete=0;
                break;
            }
        }
    }
    if(complete){
        PlaySound(get_sound(SOUND_FANFARE));
        set_scene(&SCENE_END);
    }

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

    update_npcs();

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
        draw_sky_layer(TEXTURE_ORBIS_FIXE,0,timer->total,camera->target.x);
        draw_sky_layer(TEXTURE_CLOUDS1,1,timer->total,camera->target.x);
        draw_sky_layer(TEXTURE_CLOUDS2,2,timer->total,camera->target.x);
        draw_sky_layer(TEXTURE_CLOUDS3,3,timer->total,camera->target.x);
        draw_sky_layer(TEXTURE_BGOVER,0,timer->total,camera->target.x);
    EndBlendMode();
    // draw
    BeginMode2D(*camera);
        draw_scene_game(scene);
    EndMode2D();
}

void draw_scene_game(Scene *scene){
    draw_map();
    Hero *hero = get_hero();
    draw_npcs();
    draw_hero(hero);
}

static const struct finale_message {
  int col,row; // 40x5
  int t0,dur; // Start time and duration in s/60. Sorted by (t0).
  const char *text;
} finale_messagev[]={
  {16,0,   0,840,"You win!"},
  {12,1, 120,720,"Code: Alex Hansen"},
  { 8,2, 240,600,"Graphics: AK Sommerville"},
  {11,3, 360,480,"Audio: Aster Kanke"},
  { 4,4, 480,360,"For Uplifting Jam, November 2025"},
  {10,2, 850,360,"Thanks for playing!"},
};
static unsigned int finale_start_time=0;

void run_scene_end(Scene *scene){
    FrameTimer *timer = get_frame_timer();
    unsigned int masterframe = (unsigned int)(timer->total * 5.0);
    ClearBackground(BLACK);
    
    // Feast scene.
    Texture2D texture = get_texture(TEXTURE_FINALE);
    DrawTexture(texture,0,0,WHITE);
    texture = get_texture(TEXTURE_FEAST);
    // const int finaleh = 120;
    const int feastw = 123;
    const int feasth = 57;
    const int colw = 25;
    const int periodv[5] = {29,37,17,19,23};
    int dstx = (RENDER_WIDTH>>1)-(feastw>>1)-8;
    int dsty = 55;
    int kidi = 0;
    for (; kidi<5; kidi++) {
        int frame = 0; // 0,1,2
        int c = periodv[kidi];
        int t = masterframe%c;
        if (t>c-7) {
          frame = 1+(t&1);
        }
        Rectangle srcr = (Rectangle){kidi*colw,frame*58,colw,feasth};
        Rectangle dstr = (Rectangle){dstx+kidi*colw,dsty,colw,feasth};
        DrawTexturePro(texture,srcr,dstr,VEC_ZERO,0.0f,WHITE);
    }
    
    // Text. 40x5 cells. Timing in s/60.
    const int frames_per_glyph = 5;
    unsigned int ttime = (unsigned int)(timer->total * 60.0);
    if (!finale_start_time) finale_start_time = ttime;
    ttime -= finale_start_time;
    Font font = get_font();
    const struct finale_message *message=finale_messagev;
    int i = sizeof(finale_messagev)/sizeof(struct finale_message);
    for (;i-->0;message++) {
        if (ttime<message->t0) break; // Message is in the future. They're sorted by (t0), so we're done.
        if (ttime>=message->t0+message->dur) continue; // Message finished, move along.
        int glyphc=(ttime-message->t0)/frames_per_glyph;
        char tmp[64];
        int tmpc=0;
        const char *src=message->text;
        for (;*src&&(tmpc<glyphc)&&(tmpc<63);src++,tmpc++) {
          tmp[tmpc]=*src;
        }
        tmp[tmpc]=0;
        Vector2 dst={message->col*8,120+message->row*12};
        DrawTextEx(font,tmp,dst,12.0f,0.0f,WHITE);
    }
}
