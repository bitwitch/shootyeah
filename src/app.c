#include "app.h"

void init_sdl(void) {
    int renderer_flags, window_flags;

	renderer_flags = SDL_RENDERER_ACCELERATED;
	window_flags = 0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	app.window = SDL_CreateWindow("Game", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	app.renderer = SDL_CreateRenderer(app.window, -1, renderer_flags);
}



void do_input(void) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            app.quit = true;
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if (event.key.repeat == 0) {
                switch(event.key.keysym.scancode) {
                case SDL_SCANCODE_UP:
                    app.up = event.type == SDL_KEYDOWN;
                    break;
                case SDL_SCANCODE_DOWN:
                    app.down = event.type == SDL_KEYDOWN;
                    break;
                case SDL_SCANCODE_LEFT:
                    app.left = event.type == SDL_KEYDOWN;
                    break;
                case SDL_SCANCODE_RIGHT:
                    app.right = event.type == SDL_KEYDOWN;
                    break;
                case SDL_SCANCODE_RETURN:
                    app.enter = event.type == SDL_KEYDOWN;
                    break;
                case SDL_SCANCODE_X:
                    app.fire = event.type == SDL_KEYDOWN;
                    break;
                default:
                    break;
                }

            }
            break;

        default:
            break;
        }
    }
}

