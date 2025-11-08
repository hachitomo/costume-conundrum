#include "frame_timer.h"
#include "raylib.h"
#include <stdio.h>

FrameTimer frame_timer = {
    .frame_time=0.0,
    .global_exetime_sec=0,
    .total=0.0,
};

FrameTimer *get_frame_timer(){
    return &frame_timer;
}

void update_frame_timer() {
    frame_timer.frame_time = GetFrameTime();
    frame_timer.total = GetTime();
    frame_timer.global_exetime_sec = (long)frame_timer.total;
}

float elapsed(float since){
    if(since > frame_timer.global_exetime_sec){
        printf("WARNING: Requested elapsed since future time %f!",since);
        return 0.0;
    }
    return frame_timer.global_exetime_sec - since;
}
