#include "audio.h"
#include "raylib.h"
#include "../data.h"
#include <stdio.h>

Sound arpsound,blipsound,boopsound,fanfaresound;
Wave arpwave,blip,boop,fanfare;

void init_audio(){
    InitAudioDevice();
    // do a macro like LOADIMG, load WAVE, convert WAVE to SOUND, make get_sound() return reference function
    #define LOADSOUND(tag) \
      Wave tag##wav = LoadWaveFromMemory(".wav",sound_##tag,sound_##tag##_length); \
      tag##sound = LoadSoundFromWave(tag##wav);
    LOADSOUND(arp)
    LOADSOUND(blip)
    LOADSOUND(boop)
    LOADSOUND(fanfare)
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

void deinit_audio(){
    CloseAudioDevice();
}