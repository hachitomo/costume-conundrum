#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"

// camera origin fixed at center of render window for this game
// use target (where camera origin is in world)
// and offset (relative to target)
// to control the camera

void init_camera();
Camera2D* get_camera(void);
void camera_set_target(Vector2 target);
void camera_set_offset(Vector2 offset);
void camera_transition_offset(Vector2 offset,float duration_sec);
Vector2 clamp_camera(Vector2 toclamp);

#endif
