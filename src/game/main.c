#include "raylib.h"
#include "data.h"
#include "main.h"
#include "draw/draw.h"
#include "hero/hero.h"
#include "scene/scene.h"
#include "frame_timer.h"
#include "input/input.h"
#include <stdio.h>

#ifdef PLATFORM_WEB

    #include <emscripten/emscripten.h>

#endif

int main(void)
{
    SetTraceLogLevel(LOG_LEVEL);
    InitWindow(RENDER_WIDTH, RENDER_HEIGHT, "Costume Conundrum");
    set_scene(&SCENE_MENU);

    init_menu();
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

    deinit_draw();
    CloseWindow();

    return 0;
}

void UpdateDrawFrame(){
    update_frame_timer();
    poll_inputs();
    draw_game(get_current_scene());
}

