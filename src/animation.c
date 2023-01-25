// Copyright (c) Dawid "hhimko" Cyganek <dawid.cyganek1@gmail.com>. Licensed under the MIT Licence.
// See the LICENCE file in the repository root for full licence text.

#include "./animation.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SMOOTHSTEP_RESOLUTION 100
#define CLAMP(a, b, x) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

typedef struct AnimationNode AnimationNode;
typedef void (*animationUpdateCallback)(AnimationNode* node, uint64_t dt);

struct AnimationNode {
    Animation* anim;
    int reversed; // defines the animation direction
    double tstep; // interpolator step for a unit of time passed 
    double interp; // inner interpolator, independent of animation direction
    animationUpdateCallback update;
    AnimationNode* next;
};

static double* __s_smoothstep_arr = NULL; // pointer to a precomputed smoothstep function array
static AnimationNode* s_animations = NULL; // first node of the linked list or NULL


double smoothstep(double x) {
    int t = (int)(x * SMOOTHSTEP_RESOLUTION + 0.5);
    return __s_smoothstep_arr[CLAMP(0, SMOOTHSTEP_RESOLUTION - 1, t)];
}

double smoothstep_inout(double x) {
    int t = (int)((1 - fabs((x - 0.5)*2)) * SMOOTHSTEP_RESOLUTION + 0.5);
    return __s_smoothstep_arr[CLAMP(0, SMOOTHSTEP_RESOLUTION - 1, t)];
}

int precomputeSmoothstep() {
    __s_smoothstep_arr = malloc(sizeof(double) * SMOOTHSTEP_RESOLUTION);
    if (__s_smoothstep_arr == NULL)
        return -1;

    for (int i = 0; i < SMOOTHSTEP_RESOLUTION; ++i) {
        double t = (double)i / (SMOOTHSTEP_RESOLUTION - 1);
        __s_smoothstep_arr[i] = t * t * (3.0 - t * 2.0);
    }

    return 0;
}

animationUpdateCallback getAnimationUpdateCallback(AnimationType type, int looping);

AnimationNode* createAnimationNode(Animation* anim, uint32_t duration, AnimationType type, int looping, int reversed) {
    if (anim == NULL) return NULL;
    AnimationNode* node = malloc(sizeof(AnimationNode));

    if (node != NULL) {
        node->anim = anim;
        node->reversed = reversed;
        anim->t = CLAMP(0.0, 1.0, anim->t); // t has to be in [0, 1] range for the interpolator to work
        node->tstep = 1.0 / (duration ? duration : 1); // avoid division by 0
        node->interp = reversed ? 1.0 - anim->t : anim->t; // independent of animation direction
        node->update = getAnimationUpdateCallback(type, looping);
        node->next = NULL;
    }

    return node;
}

void destroyAnimationNode(AnimationNode* node);

void animationCancel(Animation* anim){
    AnimationNode* node = s_animations;
    while (node != NULL) {
        if (node->anim == anim) {
            destroyAnimationNode(node);
            return;
        }

        node = node->next;
    }
}

void animationCancelDefault(Animation* anim){
    (void) anim;
}

void destroyAnimationNode(AnimationNode* node) {
    if (node == NULL) return;
    node->anim->cancel = animationCancelDefault; // detach the Animation.cancel callback before freeing AnimationNode

    // remove the node from the linked list
    AnimationNode* current = s_animations;
    if (current == node) {
        s_animations = current->next;
    } 
    else if (s_animations != NULL) {
        AnimationNode* previous = s_animations;
        current = s_animations->next;
        while (current != NULL) {
            if (current == node) {
                previous->next = current->next;
                break;
            }
            previous = current;
            current = current->next;
        }
    }

    // destroy the AnimationNode
    node->next = NULL;
    free(node);
}

Animation* createAnimation(double start_value) {
    Animation* anim = malloc(sizeof(Animation));

    if (anim != NULL) {
        anim->t = start_value;
        anim->cancel = animationCancelDefault;
    }

    return anim;
}

void destroyAnimation(Animation* anim) {
    anim->cancel(anim);
    free(anim);
}

void pushAnimation(Animation* anim, uint32_t duration, AnimationType type, int looping, int reversed) {
    AnimationNode* node = createAnimationNode(anim, duration, type, looping, reversed);
    if (node == NULL) {
        fprintf(stderr, "Failed to push a new AnimationNode.\n");
        return;
    }

    anim->cancel = animationCancel; // attach cancel implementation to Animation.cancel

    // append the newly created node to the end of the linked list
    if (s_animations == NULL) s_animations = node;
    else {
        AnimationNode* current = s_animations;
        while (current->next != NULL) current = current->next;
        current->next = node; 
    }
}

void updateAnimations(uint64_t dt){
    AnimationNode* current = s_animations;
    AnimationNode* next;
    while (current != NULL) {
        // next has to be extracted before current.update is run, since update can cause the node to be freed
        next = current->next; 
        current->update(current, dt);
        current = next;
    }
}

void clearAnimations() {
    while (s_animations != NULL) {
        destroyAnimationNode(s_animations);
    }

    s_animations = NULL;
}

#pragma region ANIMATION_NODE_UPDATE_IMPL 

static inline void _updateAnimationT(AnimationNode* node, double interp) {
    if (node->reversed){
        node->anim->t = (1 - interp); 
    } else{
        node->anim->t = interp; 
    }
}

void __AN_update_linear_looping(AnimationNode* node, uint64_t dt) {
    node->interp += node->tstep*dt;
    if (node->interp > 1) node->interp -= (int)node->interp;
    _updateAnimationT(node, node->interp); 
}

void __AN_update_linear_oneshot(AnimationNode* node, uint64_t dt) {
    node->interp += node->tstep*dt;
    if (node->interp <= 1.0) {
        _updateAnimationT(node, node->interp); 
    }
    else {
        _updateAnimationT(node, 1.0); 
        destroyAnimationNode(node);
    }
}

void __AN_update_smoothstep_looping(AnimationNode* node, uint64_t dt) {
    node->interp += node->tstep*dt;
    if (node->interp > 1) node->interp -= (int)node->interp;
    _updateAnimationT(node, smoothstep(node->interp)); 
}

void __AN_update_smoothstep_oneshot(AnimationNode* node, uint64_t dt) {
    node->interp += node->tstep*dt;
    if (node->interp <= 1.0) {
        _updateAnimationT(node, smoothstep(node->interp)); 
    }
    else {
        _updateAnimationT(node, 1.0); 
        destroyAnimationNode(node);
    }
}

void __AN_update_smoothstep_inout_looping(AnimationNode* node, uint64_t dt) {
    node->interp += node->tstep*dt;
    if (node->interp > 1) node->interp -= (int)node->interp;
    _updateAnimationT(node, smoothstep_inout(node->interp)); 
}

void __AN_update_smoothstep_inout_oneshot(AnimationNode* node, uint64_t dt) {
    node->interp += node->tstep*dt;
    if (node->interp <= 1.0) {
        _updateAnimationT(node, smoothstep_inout(node->interp));
    }
    else {
        _updateAnimationT(node, 0.0);
        destroyAnimationNode(node);
    }
}

void __AN_update_invalid(AnimationNode* node, uint64_t dt) {
    (void) dt;
    fprintf(stderr, "Unhandled AnimationType and looping combination in createAnimationNode.\n");
    destroyAnimationNode(node);
}

#pragma endregion

animationUpdateCallback getAnimationUpdateCallback(AnimationType type, int looping) {
    switch (type) {
    case ANIMATION_TYPE_LINEAR:
        return looping ? __AN_update_linear_looping : __AN_update_linear_oneshot;

    case ANIMATION_TYPE_SMOOTHSTEP:
        return looping ? __AN_update_smoothstep_looping : __AN_update_smoothstep_oneshot;
    
    case ANIMATION_TYPE_SMOOTHSTEP_INOUT:
        return looping ? __AN_update_smoothstep_inout_looping : __AN_update_smoothstep_inout_oneshot;
    }

    return __AN_update_invalid;
}

int initializeAnimationModule() {
    if (precomputeSmoothstep() < 0) {
        fprintf(stderr, "Failed to precompute smoothstep array.\n");
        return -1;
    }
    return 0;   
}

void destroyAnimationModule() {
    clearAnimations();
    free(__s_smoothstep_arr);
}
