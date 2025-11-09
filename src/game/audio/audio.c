#include "audio.h"
#include "raylib.h"
#include "../data.h"
#include <stdio.h>

Sound arpsound,blipsound,boopsound,fanfaresound;
Wave arpwave,blip,boop,fanfare;
Music menumusic,gamemusic,endingmusic;

void init_audio(){
    InitAudioDevice();
    #define LOADSOUND(tag) \
      Wave tag##wav = LoadWaveFromMemory(".wav",sound_##tag,sound_##tag##_length); \
      tag##sound = LoadSoundFromWave(tag##wav); \
      UnloadWave(tag##wav);
    #define LOADSONG(tag) tag##music = LoadMusicStreamFromMemory(".mp3",song_##tag,song_##tag##_length); 
    LOADSOUND(arp)
    LOADSOUND(blip)
    LOADSOUND(boop)
    LOADSOUND(fanfare)
    LOADSONG(menu)
    LOADSONG(game)
    LOADSONG(ending)
};

Sound get_sound(int soundid){
    switch(soundid){
        case SOUND_ARP:
            return arpsound;
        case SOUND_BLIP:
            return blipsound;
        case SOUND_BOOP:
            return boopsound;
        case SOUND_FANFARE:
            return fanfaresound;
        default:
            printf("WARNING! Sound with ID %d not found, deaulting...",soundid);
            return blipsound;
    }
}

Music get_song(int songid){
    switch(songid){
        case SONG_MENU:
            return menumusic;
        case SONG_GAME:
            return gamemusic;
        case SONG_ENDING:
            return endingmusic;
        default:
            printf("WARNING! Song with ID %d not found, deaulting...",songid);
            return menumusic;
    }
}

void deinit_audio(){
    UnloadSound(blipsound);
    UnloadSound(boopsound);
    UnloadSound(arpsound);
    UnloadSound(fanfaresound);
    UnloadMusicStream(menumusic);
    UnloadMusicStream(gamemusic);
    UnloadMusicStream(endingmusic);
    CloseAudioDevice();
}