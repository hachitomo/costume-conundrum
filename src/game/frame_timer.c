#include "frame_timer.h"
#include "raylib.h"

FrameTimer frame_timer = {
    .global_framect=0,
};

FrameTimer *get_frame_timer(){
    return &frame_timer;
}

void update_frame_timer() {
    float ftime = GetFrameTime();
    frame_timer.frame_time = ftime;
    frame_timer.frame_offset += ftime;
    if(frame_timer.frame_offset > 1){
        frame_timer.global_framect += (int)frame_timer.frame_offset;
        frame_timer.frame_offset = frame_timer.frame_offset - (int)frame_timer.frame_offset;
    }
}