#include "input.h"
#include "raylib.h"

Inputs _inputs;

void poll_inputs(void){
    int isleft = IsKeyDown(KEY_LEFT) | IsKeyDown(KEY_A);
    int isdown = IsKeyDown(KEY_DOWN) | IsKeyDown(KEY_S);
    int isright = IsKeyDown(KEY_RIGHT) | IsKeyDown(KEY_D);
    int isup = IsKeyDown(KEY_UP) | IsKeyDown(KEY_W) | IsKeyDown(KEY_SPACE);
    _inputs.left=isleft;
    _inputs.down=isdown;
    _inputs.right=isright;
    _inputs.up=isup;
    _inputs.interact=false;
}

Inputs get_inputs(void){
    return _inputs;
}