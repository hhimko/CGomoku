#ifndef SCENE_H_
#define SCENE_H_


typedef enum Scene {
    SCENE_MENU = 0,
    SCENE_GAME
} Scene;

typedef struct AppState AppState; // AppState declaration to avoid circular imports
void setScene(AppState* state, Scene scene);

#endif /* SCENE_H_ */
