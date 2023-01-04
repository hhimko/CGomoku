#include "./animation.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SMOOTHSTEP_RESOLUTION 100
#define CLAMP(a, b, x) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

typedef struct AnimationNode AnimationNode;
typedef void (*animationUpdateCallback)(AnimationNode* node, uint64_t dt);

static double* __s_smoothstep_arr = NULL; // pointer to a precomputed smoothstep function array
static AnimationNode* s_animations; // holds the first node of the linked list 


struct AnimationNode {
    double* pvar;
    double interp;
    AnimationType type;
    int looping; // whether the animation should loop or not
    AnimationNode* next;
    animationUpdateCallback update;
};

AnimationNode* createAnimationNode(double* pvar, AnimationType type, int looping) {
    AnimationNode* node = malloc(sizeof(AnimationNode));
    if (node != NULL) {
        node->pvar = pvar;
        node->interp = 0.0;
        node->type = type;
        node->looping = looping;
    }

    return node;
}

void destroyAnimationNode(AnimationNode* node) {
    // frees all subsequent nodes recursively
    if (node->next) destroyAnimationNode(node->next);

    node->next = NULL;
    free(node);
}

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

void pushAnimation(double* pvar, AnimationType animation_type, int looping) {
    AnimationNode* node = createAnimationNode(pvar, animation_type, looping);

    // append the newly created node to the end of the linked list
    AnimationNode* current = s_animations;
    while (current != NULL) current = current->next;
    *current = *node; 
}

void updateAnimations(uint64_t dt){
    AnimationNode* current = s_animations;
    while (current != NULL) {
        current->update(current, dt);
        current = current->next;
    }
}

void clearAnimations() {
    destroyAnimationNode(s_animations);
    s_animations = NULL;
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
