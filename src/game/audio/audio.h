#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

#define SOUND_JUMP          1
#define SOUND_JOEY          2
#define SOUND_PICKUP        3
#define SOUND_FANFARE       4
#define SOUND_SUCCESSONE   5
#define SOUND_SUCCESSTWO   6

#define SONG_MENU       7
#define SONG_GAME       8
#define SONG_ENDING     9

void init_audio();
Sound get_sound(int soundid);
Sound success_sound();
Music get_song(int soundid);
void PlaySoundVolume(int soundid,float volume);
void deinit_audio();

#endif