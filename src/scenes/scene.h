#ifndef SCENE_H_
#define SCENE_H_


typedef enum Scene {
    SCENE_MENU = 0,
    SCENE_GAME
} Scene;

struct AppState; // \skip AppState redeclaration to avoid circular imports 
void setScene(struct AppState* state, Scene scene);

#endif /* SCENE_H_ */
