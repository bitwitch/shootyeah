#include <stdio.h>
#include "app.h"
#include "game.h"
#include "renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

App app = {0};
/*Game game = {0};*/

int main(void) {

    init_sdl();

    game_init();
    
    while (!app.quit) {
        renderer_prepare();
        do_input();
        game_update();
        game_render();
        renderer_present();
    }

    return 0;
}

