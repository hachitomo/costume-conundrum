#include "raylib.h"
#include <stdio.h>

#ifdef PLATFORM_WEB

    #include <emscripten/emscripten.h>

#endif

void UpdateDrawFrame(void);

float frameTime = 0;
Texture2D chartex;
const int screenWidth = 640;
const int screenHeight = 360;

int main(void)
{
    SetTraceLogLevel(LOG_ALL);
    ChangeDirectory(GetApplicationDirectory());
    InitWindow(screenWidth, screenHeight, "Costume Conundrum");
    chartex = LoadTexture("data/image/3-sprites.png");

    SetTargetFPS(60);

    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else
        SetTargetFPS(60);
        while (!WindowShouldClose())
        {
            UpdateDrawFrame();
        }
    #endif

    UnloadTexture(chartex);
    CloseWindow();

    return 0;
}

void UpdateDrawFrame(void){
    frameTime = frameTime + GetFrameTime();
    int animFrame = (int)(frameTime * 8) % 8;
    char output[30];
    sprintf(output,"Frame: %d",(int)animFrame);
    Rectangle texClip = {
        .width=-16,
        .height=26,
        .x=37+(18*animFrame),
        .y=1,
    };
    Vector2 zeropos = {
        .x=0,
        .y=0,
    };
    Vector2 pos = {
        .x=(screenWidth*0.5)-8,
        .y=260,
    };
    
    // draw to framebuffer
    RenderTexture2D fbuffer = LoadRenderTexture(16,26);
    BeginTextureMode(fbuffer);
        ClearBackground(WHITE);
        DrawTextureRec(chartex,texClip,zeropos,WHITE);
    EndTextureMode();

    // draw buffer to screen
    BeginDrawing();

        ClearBackground(WHITE);

        // 4th param is scale, this upscales the output buffer to the screen size
        DrawTextureEx(fbuffer.texture,pos,180,8,WHITE);
        DrawText(output,10,10,14,BLACK);

    EndDrawing();
}

