#include <SDL.h>

#include "number.h"

namespace rtl {

namespace sdl {

extern const Number CONSTANT_INIT_VIDEO = number_from_uint32(SDL_INIT_VIDEO);
extern const Number CONSTANT_RENDERER_ACCELERATED = number_from_uint32(SDL_RENDERER_ACCELERATED);
extern const Number CONSTANT_RENDERER_PRESENTVSYNC = number_from_uint32(SDL_RENDERER_PRESENTVSYNC);
extern const Number CONSTANT_SDL_BUTTON_LEFT = number_from_uint32(SDL_BUTTON_LEFT);
extern const Number CONSTANT_SDL_BUTTON_RIGHT = number_from_uint32(SDL_BUTTON_RIGHT);
extern const Number CONSTANT_SDL_KEYDOWN = number_from_uint32(SDL_KEYDOWN);
extern const Number CONSTANT_SDL_KEYUP = number_from_uint32(SDL_KEYUP);
extern const Number CONSTANT_SDL_MOUSEBUTTONDOWN = number_from_uint32(SDL_MOUSEBUTTONDOWN);
extern const Number CONSTANT_SDL_MOUSEBUTTONUP = number_from_uint32(SDL_MOUSEBUTTONUP);
extern const Number CONSTANT_SDL_QUIT = number_from_uint32(SDL_QUIT);
extern const Number CONSTANT_SDL_WINDOWEVENT = number_from_uint32(SDL_WINDOWEVENT);
extern const Number CONSTANT_SDL_WINDOWEVENT_SHOWN = number_from_uint32(SDL_WINDOWEVENT_SHOWN);
extern const Number CONSTANT_SDLK_LEFT = number_from_uint32(SDLK_LEFT);
extern const Number CONSTANT_SDLK_LSHIFT = number_from_uint32(SDLK_LSHIFT);
extern const Number CONSTANT_SDLK_RIGHT = number_from_uint32(SDLK_RIGHT);
extern const Number CONSTANT_SDLK_RSHIFT = number_from_uint32(SDLK_RSHIFT);
extern const Number CONSTANT_SDLK_SPACE = number_from_uint32(SDLK_SPACE);
extern const Number CONSTANT_SDLK_UP = number_from_uint32(SDLK_UP);
extern const Number CONSTANT_WINDOW_SHOWN = number_from_uint32(SDL_WINDOW_SHOWN);

} // namespace sdl

} // namespace rtl
