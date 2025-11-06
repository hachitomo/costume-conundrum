#include "raylib.h"
#include "camera.h"
#include "../constants.h"

Camera2D _camera;
Vector2 _transition_target;
float _transition_duration;

void init_camera(){
    _camera.target=VEC_ZERO;
    _camera.offset.x=RENDER_WIDTH*0.5f;
    _camera.offset.y=RENDER_HEIGHT*0.5f;
    _camera.rotation=0.0f;
    _camera.zoom=1.0f;
}

Camera2D* get_camera(void){
    return &_camera;
};

void camera_set_target(Vector2 target){
    _camera.target = target;
};

void camera_set_offset(Vector2 offset){
    _camera.offset = offset;
};

// begins a transition. interrupts any current transition
void camera_transition_offset(Vector2 offset,float duration_sec){
    _transition_target = offset;
    _transition_duration = duration_sec;
}
