// Copyright (c) Dawid "hhimko" Cyganek <dawid.cyganek1@gmail.com>. Licensed under the MIT Licence.
// See the LICENCE file in the repository root for full licence text.

#ifndef MIXER_H_
#define MIXER_H_

#define SOUND_BUTTON_HOVER  0
#define SOUND_BUTTON_SELECT 1
#define SOUND_COUNT         2


void playSound(size_t index);

int initializeMixer();
void destroyMixer();

#endif /* MIXER_H_ */
