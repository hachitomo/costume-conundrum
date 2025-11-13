#include "input.h"
#include "raylib.h"
#include <math.h>

Inputs inputs;
int keys_lastframe = 0;
int gamepad_lastframe = 0;
float joystick_dead_zone = 0.15f;

void poll_inputs(void){
    int left = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    int down = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
    int right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
    int up = IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
    int jump = IsKeyPressed(KEY_SPACE);

    int gamepadleft = 0;
    int gamepaddown = 0;
    int gamepadright= 0;
    int gamepadup = 0;
    int gamepadjump = 0;

    if(IsGamepadAvailable(0)){
        gamepadleft =  IsGamepadButtonDown(0,GAMEPAD_BUTTON_LEFT_FACE_LEFT);
        gamepaddown =  IsGamepadButtonDown(0,GAMEPAD_BUTTON_LEFT_FACE_DOWN);
        gamepadright = IsGamepadButtonDown(0,GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
        gamepadup =  IsGamepadButtonPressed(0,GAMEPAD_BUTTON_LEFT_FACE_UP) ;
        gamepadjump =  IsGamepadButtonPressed(0,GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    }
    float joyx = GetGamepadAxisMovement(0,GAMEPAD_AXIS_LEFT_X);
    float joyy = GetGamepadAxisMovement(0,GAMEPAD_AXIS_LEFT_Y);
    if(fabs(joyx) > joystick_dead_zone){
        if(joyx > 0){
            gamepadright = 1;
        }else {
            gamepadleft = 1;
        }
    }
    if(fabs(joyy) > joystick_dead_zone){
        if(joyy > 0){
            gamepaddown = 1;
        }else {
            gamepadup = 1;
        }
    }

    inputs.left=left||gamepadleft;
    inputs.down=down||gamepaddown;
    inputs.right=right||gamepadright;
    inputs.up=up||gamepadup;
    inputs.interact=down||up||gamepaddown||gamepadup;
    inputs.jump=jump||gamepadjump;
}

Inputs get_inputs(void){
    return inputs;
}
