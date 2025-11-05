#ifndef INPUT_H
#define INPUT_H

typedef struct Inputs{
    int up,left,down,right,interact;
}Inputs;

void poll_inputs(void);
struct Inputs get_inputs(void);

#endif