#include "./animation.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SMOOTHSTEP_RESOLUTION 100
#define CLAMP(a, b, x) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

typedef struct AnimationNode AnimationNode;
typedef void (*animationUpdateCallback)(AnimationNode* node, uint64_t dt);

struct AnimationNode {
    double* pvar;
    double interp;
    double tstep;
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

AnimationNode* createAnimationNode(double* pvar, uint32_t duration, AnimationType type, int looping) {
    if (pvar == NULL) return NULL;
    AnimationNode* node = malloc(sizeof(AnimationNode));

    if (node != NULL) {
        node->pvar = pvar;
        node->interp = (*pvar) - (int)(*pvar); // start value depends on the variable pointer value
        node->tstep = 1.0 / (duration ? duration : 1); // avoid division by 0
        node->update = getAnimationUpdateCallback(type, looping);
        node->next = NULL;
    }

    return node;
}

void destroyAnimationNode(AnimationNode* node) {
    if (node == NULL) return;

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

void pushAnimation(double* pvar, uint32_t duration, AnimationType type, int looping) {
    AnimationNode* node = createAnimationNode(pvar, duration, type, looping);
    if (node == NULL) {
        fprintf(stderr, "Failed to push a new AnimationNode.\n");
        return;
    }

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

void __AN_update_linear_looping(AnimationNode* node, uint64_t dt) {
    node->interp += node->tstep*dt;
    if (node->interp > 1) node->interp -= (int)node->interp;
    *node->pvar = node->interp; 
}

void __AN_update_linear_oneshot(AnimationNode* node, uint64_t dt) {
    node->interp += node->tstep*dt;
    if (node->interp <= 1.0) {
        *node->pvar = node->interp; 
    }
    else {
        *node->pvar = 1.0; 
        destroyAnimationNode(node);
    }
}

void __AN_update_smoothstep_looping(AnimationNode* node, uint64_t dt) {
    node->interp += node->tstep*dt;
    if (node->interp > 1) node->interp -= (int)node->interp;
    *node->pvar = smoothstep(node->interp); 
}

void __AN_update_smoothstep_oneshot(AnimationNode* node, uint64_t dt) {
    node->interp += node->tstep*dt;
    if (node->interp <= 1.0) {
        *node->pvar = smoothstep(node->interp); 
    }
    else {
        *node->pvar = 1.0; 
        destroyAnimationNode(node);
    }
}

void __AN_update_smoothstep_inout_looping(AnimationNode* node, uint64_t dt) {
    node->interp += node->tstep*dt;
    if (node->interp > 1) node->interp -= (int)node->interp;
    *node->pvar = smoothstep_inout(node->interp); 
}

void __AN_update_smoothstep_inout_oneshot(AnimationNode* node, uint64_t dt) {
    node->interp += node->tstep*dt;
    if (node->interp <= 1.0) {
        *node->pvar = smoothstep_inout(node->interp); 
    }
    else {
        *node->pvar = 0.0; 
        destroyAnimationNode(node);
    }
}

void __AN_update_invalid(AnimationNode* node, uint64_t dt) {
    (void) dt;
    fprintf(stderr, "Unhandled AnimationType and looping combination in createAnimationNode.\n");
    destroyAnimationNode(node);
}

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
