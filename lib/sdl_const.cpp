#include <SDL.h>

#include "number.h"

namespace rtl {

extern const Number sdl$INIT_VIDEO = number_from_uint32(SDL_INIT_VIDEO);
extern const Number sdl$RENDERER_ACCELERATED = number_from_uint32(SDL_RENDERER_ACCELERATED);
extern const Number sdl$RENDERER_PRESENTVSYNC = number_from_uint32(SDL_RENDERER_PRESENTVSYNC);
extern const Number sdl$SDL_BUTTON_LEFT = number_from_uint32(SDL_BUTTON_LEFT);
extern const Number sdl$SDL_BUTTON_RIGHT = number_from_uint32(SDL_BUTTON_RIGHT);
extern const Number sdl$SDL_KEYDOWN = number_from_uint32(SDL_KEYDOWN);
extern const Number sdl$SDL_KEYUP = number_from_uint32(SDL_KEYUP);
extern const Number sdl$SDL_MOUSEBUTTONDOWN = number_from_uint32(SDL_MOUSEBUTTONDOWN);
extern const Number sdl$SDL_MOUSEBUTTONUP = number_from_uint32(SDL_MOUSEBUTTONUP);
extern const Number sdl$SDL_QUIT = number_from_uint32(SDL_QUIT);
extern const Number sdl$SDLK_LEFT = number_from_uint32(SDLK_LEFT);
extern const Number sdl$SDLK_LSHIFT = number_from_uint32(SDLK_LSHIFT);
extern const Number sdl$SDLK_RIGHT = number_from_uint32(SDLK_RIGHT);
extern const Number sdl$SDLK_RSHIFT = number_from_uint32(SDLK_RSHIFT);
extern const Number sdl$SDLK_SPACE = number_from_uint32(SDLK_SPACE);
extern const Number sdl$SDLK_UP = number_from_uint32(SDLK_UP);
extern const Number sdl$WINDOW_SHOWN = number_from_uint32(SDL_WINDOW_SHOWN);

} // namespace rtl
