#include "raylib.h"
#include "camera.h"
#include "../constants.h"

Camera2D camera;
Vector2 transition_target;
float transition_duration;

void init_camera(){
    camera.target=VEC_ZERO;
    camera.offset.x=RENDER_WIDTH*0.5f;
    camera.offset.y=RENDER_HEIGHT*0.5f;
    camera.rotation=0.0f;
    camera.zoom=1.0f;
}

Camera2D* get_camera(void){
    return &camera;
};

void camera_set_target(Vector2 target){
    camera.target = target;
};

void camera_set_offset(Vector2 offset){
    camera.offset = offset;
};

// begins a transition. interrupts any current transition
void camera_transition_offset(Vector2 offset,float duration_sec){
    transition_target = offset;
    transition_duration = duration_sec;
}
