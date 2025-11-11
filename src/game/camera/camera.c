#include "raylib.h"
#include "camera.h"
#include "../constants.h"
#include "../data.h"
#include "../map/map.h"

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

Vector2 clamp_camera(Vector2 target){
    Vector2 result = target;

    // clamp to edges of the screen
    float minscrollx = RENDER_WIDTH*0.5;
    float minscrolly = RENDER_HEIGHT*0.5;
    float maxscrollx = (map_w*TILE_SIZE) - RENDER_WIDTH*0.5;
    float maxscrolly = (map_h*TILE_SIZE) - RENDER_HEIGHT*0.5;
    if(target.x < minscrollx){
        result.x = minscrollx;
    }
    if(target.x > maxscrollx){
        result.x = maxscrollx;
    }
    if(target.y < minscrolly){
        result.y = minscrolly;
    }
    if(target.y > maxscrolly){
        result.y = maxscrolly;
    }

    // TODO: only update camera when hero leaves a box in the center of the screen

    return result;
}
