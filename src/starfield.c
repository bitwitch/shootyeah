#include "app.h"

#define MAX_STARS 512

typedef struct {
    float x, y;
    int w;
    int speed;
} Star;

static Star stars[MAX_STARS];

void starfield_init(void) {
    for (int i=0; i<MAX_STARS; ++i) {
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = rand() % SCREEN_HEIGHT;
        stars[i].w = 1 + rand() % 4;
        stars[i].speed = 1 + rand() % 4;
    }
}

void starfield_update(void) {
    for (int i=0; i<MAX_STARS; ++i) {
        Star *s = &stars[i];
        s->x -= s->speed;
        if (s->x < -s->w) {
            s->x += SCREEN_WIDTH + s->w;
            s->y = rand() % SCREEN_HEIGHT;
        }
    }
}

void starfield_render(void) {
    for (int i=0; i<MAX_STARS; ++i) {
        Star *s = &stars[i];
		int c = 32 * s->speed;
		SDL_SetRenderDrawColor(app.renderer, c, c, c, 255);
		SDL_RenderDrawLine(app.renderer, s->x, s->y, s->x + s->w, s->y);
    }
}

