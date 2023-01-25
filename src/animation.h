// Copyright (c) Dawid "hhimko" Cyganek <dawid.cyganek1@gmail.com>. Licensed under the MIT Licence.
// See the LICENCE file in the repository root for full licence text.

#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <stdint.h>

#define ANIMATION_ONESHOT 0
#define ANIMATION_LOOPING 1

#define ANIMATION_NO_REVERSE 0
#define ANIMATION_REVERSED   1


typedef enum AnimationType {
    ANIMATION_TYPE_LINEAR,
    ANIMATION_TYPE_SMOOTHSTEP,
    ANIMATION_TYPE_SMOOTHSTEP_INOUT
} AnimationType;

typedef struct Animation Animation;
typedef void (*animationCancelCallback)(Animation* anim);

struct Animation {
    double t;
    animationCancelCallback cancel;
};

Animation* createAnimation(double start_value);
void destroyAnimation(Animation* anim);

void pushAnimation(Animation* anim, uint32_t duration, AnimationType type, int looping, int reversed);
void updateAnimations(uint64_t dt);
void clearAnimations();

int initializeAnimationModule();
void destroyAnimationModule();

#endif /* _ANIMATION_H */
