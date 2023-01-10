#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <stdint.h>


typedef enum AnimationType {
    ANIMATION_TYPE_LINEAR,
    ANIMATION_TYPE_SMOOTHSTEP,
    ANIMATION_TYPE_SMOOTHSTEP_INOUT
} AnimationType;

void pushAnimation(double* pvar, uint32_t duration, AnimationType type, int looping);
void updateAnimations(uint64_t dt);
void clearAnimations();

int initializeAnimationModule();
void destroyAnimationModule();

#endif /* _ANIMATION_H */
