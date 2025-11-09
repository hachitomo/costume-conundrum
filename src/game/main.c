#include "raylib.h"
#include "data.h"
#include "frame_timer.h"
#include "main.h"
#include "audio/audio.h"
#include "draw/draw.h"
#include "hero/hero.h"
#include "input/input.h"
#include "scene/scene.h"
#include <stdio.h>

#ifdef PLATFORM_WEB

    #include <emscripten/emscripten.h>

#endif

int main(void)
{
    SetTraceLogLevel(LOG_LEVEL);
    InitWindow(RENDER_WIDTH*2, RENDER_HEIGHT*2, "Costume Conundrum");
    set_scene(&SCENE_MENU);

    init_audio();
    init_draw();
    init_hero();

    #if defined(PLATFORM_WEB)
        SetTargetFPS(TARGET_FPS);
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else
        SetTargetFPS(TARGET_FPS);
        while (!WindowShouldClose())
        {
            UpdateDrawFrame();
        }
    #endif

    deinit_audio();
    deinit_draw();
    CloseWindow();

    return 0;
}

void UpdateDrawFrame(){
    update_frame_timer();
    poll_inputs();
    draw_game(get_current_scene());
}

