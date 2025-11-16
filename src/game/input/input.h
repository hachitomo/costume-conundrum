#ifndef INPUT_H
#define INPUT_H

typedef struct Inputs{
    int up,left,down,right,jump,fullscreen;
}Inputs;

void poll_inputs(void);
struct Inputs get_inputs(void);
int any_inputs(void);

#endif
