#include "app.h"
#include "renderer.h"
#include "stb_image.h"

void renderer_prepare(void) {
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
	SDL_RenderClear(app.renderer);
}

void renderer_present(void) {
	SDL_RenderPresent(app.renderer);
}

SDL_Texture *load_texture(char *filename) {
    SDL_Surface *surface;
    SDL_Texture *texture;
    int texture_width, texture_height;
    unsigned char *texture_pixels;
    int color_format = STBI_rgb_alpha;

    printf("[INFO] Loading %s\n", filename);

    texture_pixels = stbi_load(filename, &texture_width, &texture_height, NULL, color_format);

    if(texture_pixels == NULL) {
        /*TODO(shaw): try to load a default texture if this one is not found*/
        fprintf(stderr, "[ERROR] Loading image %s failed: %s", filename, stbi_failure_reason());
        return NULL;
    }

    // Set up the pixel format color masks for RGB(A) byte arrays.
    Uint32 r_mask, g_mask, b_mask, a_mask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int shift = (color_format == STBI_rgb) ? 8 : 0;
    r_mask = 0xff000000 >> shift;
    g_mask = 0x00ff0000 >> shift;
    b_mask = 0x0000ff00 >> shift;
    a_mask = 0x000000ff >> shift;
#else // little endian, like x86
    r_mask = 0x000000ff;
    g_mask = 0x0000ff00;
    b_mask = 0x00ff0000;
    a_mask = (color_format == STBI_rgb) ? 0 : 0xff000000;
#endif

    int depth, pitch;
    if (color_format == STBI_rgb) {
        depth = 24;
        pitch = 3*texture_width; // 3 bytes per pixel * pixels per row
    } else { // STBI_rgb_alpha (RGBA)
        depth = 32;
        pitch = 4*texture_width;
    }

    surface = SDL_CreateRGBSurfaceFrom(texture_pixels, texture_width, texture_height, 
        depth, pitch, r_mask, g_mask, b_mask, a_mask);

    if (surface == NULL) {
      fprintf(stderr, "Creating surface failed: %s", SDL_GetError());
      stbi_image_free(texture_pixels);
      return NULL;
    }

    texture = SDL_CreateTextureFromSurface(app.renderer, surface);

    SDL_FreeSurface(surface);
    stbi_image_free(texture_pixels);

    return texture;
}

void blit(SDL_Texture *texture, int x, int y) {
	SDL_Rect dest = {.x = x, .y = y};
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
	SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

void blit_scaled(SDL_Texture *texture, int x, int y, int w, int h) {
	SDL_Rect dest = { .x = x, .y = y, .w = w, .h = h };
	SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

void blit_rect(SDL_Texture *texture, SDL_Rect *src, int x, int y) {
	SDL_Rect dest = {
        .x = x,
        .y = y,
        .w = src->w,
        .h = src->h
    };
	SDL_RenderCopy(app.renderer, texture, src, &dest);
}

void blit_rect_scaled(SDL_Texture *texture, SDL_Rect *src, int x, int y, int w, int h) {
	SDL_Rect dest = { .x = x, .y = y, .w = w, .h = h };
	SDL_RenderCopy(app.renderer, texture, src, &dest);
}



