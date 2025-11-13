#include "audio.h"
#include "raylib.h"
#include "../data.h"
#include <stdio.h>

Sound jumpsound,joeysound,pickupsound,fanfaresound,successonesound,successtwosound;
Wave arpwave,blip,boop,fanfare;
Music menumusic,gamemusic,endingmusic;

void init_audio(){
    InitAudioDevice();
    #define LOADSOUND(tag) \
      Wave tag##wav = LoadWaveFromMemory(".wav",sound_##tag,sound_##tag##_length); \
      tag##sound = LoadSoundFromWave(tag##wav); \
      UnloadWave(tag##wav);
    #define LOADSONG(tag) tag##music = LoadMusicStreamFromMemory(".mp3",song_##tag,song_##tag##_length); 
    LOADSOUND(jump)
    LOADSOUND(joey)
    LOADSOUND(pickup)
    LOADSOUND(fanfare)
    LOADSOUND(successone)
    LOADSOUND(successtwo)
    LOADSONG(menu)
    LOADSONG(game)
    LOADSONG(ending)
};

void PlaySoundVolume(int soundid, float volume){
    Sound snd = get_sound(soundid);
    if(volume < 0 || volume > 1) return;
    SetSoundVolume(snd,volume);
    PlaySound(snd);
}

Sound success_sound(){
    int rand = GetRandomValue(1,100);
    Sound success = rand >50 ? get_sound(SOUND_SUCCESSONE) : get_sound(SOUND_SUCCESSTWO);
    return success; // if only it was that easy in real life, huh?
}

Sound get_sound(int soundid){
    switch(soundid){
        case SOUND_JUMP:
            return jumpsound;
        case SOUND_JOEY:
            return joeysound;
        case SOUND_PICKUP:
            return pickupsound;
        case SOUND_FANFARE:
            return fanfaresound;
        case SOUND_SUCCESSONE:
            return successonesound;
        case SOUND_SUCCESSTWO:
            return successtwosound;
        default:
            printf("WARNING! Sound with ID %d not found, deaulting...",soundid);
            return jumpsound;
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
    UnloadSound(jumpsound);
    UnloadSound(joeysound);
    UnloadSound(pickupsound);
    UnloadSound(fanfaresound);
    UnloadSound(successtwosound);
    UnloadSound(successonesound);
    UnloadMusicStream(menumusic);
    UnloadMusicStream(gamemusic);
    UnloadMusicStream(endingmusic);
    CloseAudioDevice();
}