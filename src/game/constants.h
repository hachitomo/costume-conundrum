#ifndef CONSTANTS_H
#define CONSTANTS_H

#define RENDER_HEIGHT 180
#define RENDER_WIDTH 360
#define LOG_LEVEL LOG_WARNING
#define TARGET_FPS 60

// Gen. purpose use for when a Vec2 param is required
// but no transform is desired
static const Vector2 VEC_ZERO = {
    .x=0,
    .y=0,
};

static const Vector2 GAME_START_POS = {
    .x=200,
    .y=200,
};

#endif
