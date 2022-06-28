#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <SDL2/SDL.h>

void renderer_prepare(void);
void renderer_present(void);
SDL_Texture *load_texture(char *filename);
void blit(SDL_Texture *texture, int x, int y);
void blit_scaled(SDL_Texture *texture, int x, int y, int w, int h);
void blit_rect(SDL_Texture *texture, SDL_Rect *src, int x, int y);

#endif
