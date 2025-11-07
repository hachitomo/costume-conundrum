#ifndef MAP_H
#define MAP_H

#include "raylib.h"

typedef struct Map{
    Texture2D terraintex,spritestex;
    Rectangle *areas;
    int areasc;
}Map;

#endif