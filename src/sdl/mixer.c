// Copyright (c) Dawid "hhimko" Cyganek <dawid.cyganek1@gmail.com>. Licensed under the MIT Licence.
// See the LICENCE file in the repository root for full licence text.

#include "./mixer.h"

#include <SDL_mixer.h>
#include <stdio.h>
#include <SDL.h>

Mix_Chunk* s_chunks[SOUND_COUNT];


void playSound(size_t index) {
    if (index >= SOUND_COUNT) return;
    Mix_PlayChannel(-1, s_chunks[index], 0);
}

int loadAudio() {
    s_chunks[SOUND_BUTTON_HOVER] = Mix_LoadWAV("../assets/sfx/button_hover.wav");
    s_chunks[SOUND_BUTTON_SELECT] = Mix_LoadWAV("../assets/sfx/button_select.wav");

    for (size_t i = 0; i < SOUND_COUNT; ++i) {
        if (s_chunks[i] == NULL) return -1;
    }

    return 0;
}


int initializeMixer() {
    // initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("Failed to initalize SDL_mixer! SDL_mixer Error: %s\n", Mix_GetError());
        return -1;
    }

    if (loadAudio() < 0)
    {
        printf("Failed to load audio media! SDL_mixer Error: %s\n", Mix_GetError());
        return -1;
    }
    

    return 0;
}

void destroyMixer() {
    Mix_Quit(); // quit SDL_mixer
}
