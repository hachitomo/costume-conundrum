#ifndef FRAMETIME_H
#define FRAMETIME_H

// Helps regulate speed of game updates. 
// global_exetime_sec represents how many 
// seconds the game has been executing in total
// frame_time is how long this frame took to generate
typedef struct FrameTimer{
    float frame_time;
    long global_exetime_sec;
    double total; // Same as global_exetime_sec but the whole thing.
}FrameTimer;

FrameTimer *get_frame_timer();
void update_frame_timer();

#endif
