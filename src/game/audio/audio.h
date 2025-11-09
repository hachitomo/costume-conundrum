#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

#define SOUND_ARP     1
#define SOUND_BLIP    2
#define SOUND_BOOP    3
#define SOUND_FANFARE 4

void init_audio();
Sound get_sound(int soundid);
void deinit_audio();

#endif