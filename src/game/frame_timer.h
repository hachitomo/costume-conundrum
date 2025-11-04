#ifndef FRAMETIME_H
#define FRAMETIME_H

typedef struct FrameTimer{
    float frame_offset, frame_time;
    int global_framect;
}FrameTimer;

FrameTimer *get_frame_timer();
void update_frame_timer();

#endif