#include "input.h"
#include "raylib.h"

Inputs inputs;

void poll_inputs(void){
    int isleft = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    int isdown = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
    int isright = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
    int isup = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE);
    inputs.left=isleft;
    inputs.down=isdown;
    inputs.right=isright;
    inputs.up=isup;
    inputs.interact=false;
}

Inputs get_inputs(void){
    return inputs;
}
