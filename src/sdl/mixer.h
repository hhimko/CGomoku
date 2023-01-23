#ifndef MIXER_H_
#define MIXER_H_

#define SOUND_BUTTON_HOVER  0
#define SOUND_BUTTON_SELECT 1
#define SOUND_COUNT         2


void playSound(size_t index);

int initializeMixer();
void destroyMixer();

#endif /* MIXER_H_ */
