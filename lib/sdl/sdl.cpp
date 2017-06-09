#include <string>
#include <vector>

#include <SDL.h>

#include "neonext.h"

const struct Ne_MethodTable *Ne;

SDL_Rect *pack_Rect(SDL_Rect &out, const Ne_Cell *in)
{
    if (Ne->cell_get_boolean(Ne->cell_get_array_cell(in, 4))) {
        return NULL;
    }
    out.x = Ne->cell_get_number_int(Ne->cell_get_array_cell(in, 0));
    out.y = Ne->cell_get_number_int(Ne->cell_get_array_cell(in, 1));
    out.w = Ne->cell_get_number_int(Ne->cell_get_array_cell(in, 2));
    out.h = Ne->cell_get_number_int(Ne->cell_get_array_cell(in, 3));
    return &out;
}

void unpack_Event(Ne_Cell *out, const SDL_Event &in)
{
    Ne->cell_set_number_int(Ne->cell_set_array_cell(out, 0), in.type);
    switch (in.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            Ne->cell_set_number_int(Ne->cell_set_array_cell(Ne->cell_set_array_cell(out, 1), 0), in.key.timestamp);
            Ne->cell_set_number_int(Ne->cell_set_array_cell(Ne->cell_set_array_cell(out, 1), 1), in.key.windowID);
            Ne->cell_set_number_int(Ne->cell_set_array_cell(Ne->cell_set_array_cell(out, 1), 2), in.key.state);
            Ne->cell_set_number_int(Ne->cell_set_array_cell(Ne->cell_set_array_cell(out, 1), 3), in.key.repeat);
            Ne->cell_set_number_int(Ne->cell_set_array_cell(Ne->cell_set_array_cell(Ne->cell_set_array_cell(out, 1), 4), 0), in.key.keysym.scancode);
            Ne->cell_set_number_int(Ne->cell_set_array_cell(Ne->cell_set_array_cell(Ne->cell_set_array_cell(out, 1), 4), 1), in.key.keysym.sym);
            Ne->cell_set_number_int(Ne->cell_set_array_cell(Ne->cell_set_array_cell(Ne->cell_set_array_cell(out, 1), 4), 2), in.key.keysym.mod);
            break;
    }
}

extern "C" {

Ne_EXPORT int Ne_INIT(const struct Ne_MethodTable *methodtable)
{
    Ne = methodtable;
    return Ne_SUCCESS;
}

Ne_CONST_INT(Ne_INIT_VIDEO, SDL_INIT_VIDEO)
Ne_CONST_INT(Ne_RENDERER_ACCELERATED, SDL_RENDERER_ACCELERATED)
Ne_CONST_INT(Ne_RENDERER_PRESENTVSYNC, SDL_RENDERER_PRESENTVSYNC)
Ne_CONST_INT(Ne_SDL_KEYDOWN, SDL_KEYDOWN)
Ne_CONST_INT(Ne_SDL_KEYUP, SDL_KEYUP)
Ne_CONST_INT(Ne_SDL_QUIT, SDL_QUIT)
Ne_CONST_INT(Ne_SDLK_LEFT, SDLK_LEFT)
Ne_CONST_INT(Ne_SDLK_RIGHT, SDLK_RIGHT)
Ne_CONST_INT(Ne_SDLK_SPACE, SDLK_SPACE)
Ne_CONST_INT(Ne_SDLK_UP, SDLK_UP)
Ne_CONST_INT(Ne_WINDOW_SHOWN, SDL_WINDOW_SHOWN)

Ne_FUNC(Ne_CreateRGBSurface)
{
    int flags = Ne_PARAM_INT(0);
    int width = Ne_PARAM_INT(1);
    int height = Ne_PARAM_INT(2);
    int depth = Ne_PARAM_INT(3);
    int Rmask = Ne_PARAM_INT(4);
    int Gmask = Ne_PARAM_INT(5);
    int Bmask = Ne_PARAM_INT(6);
    int Amask = Ne_PARAM_INT(7);

    Ne_RETURN_POINTER(SDL_CreateRGBSurface(flags, width, height, depth, Rmask, Gmask, Bmask, Amask));
}

Ne_FUNC(Ne_CreateRenderer)
{
    SDL_Window *window = Ne_PARAM_POINTER(SDL_Window, 0);
    int index = Ne_PARAM_INT(1);
    int flags = Ne_PARAM_INT(2);

    Ne_RETURN_POINTER(SDL_CreateRenderer(window, index, flags));
}

Ne_FUNC(Ne_CreateSoftwareRenderer)
{
    SDL_Surface *surface = Ne_PARAM_POINTER(SDL_Surface, 0);

    Ne_RETURN_POINTER(SDL_CreateSoftwareRenderer(surface));
}

Ne_FUNC(Ne_CreateTextureFromSurface)
{
    SDL_Renderer *renderer = Ne_PARAM_POINTER(SDL_Renderer, 0);
    SDL_Surface *surface = Ne_PARAM_POINTER(SDL_Surface,  1);

    Ne_RETURN_POINTER(SDL_CreateTextureFromSurface(renderer, surface));
}

Ne_FUNC(Ne_CreateWindow)
{
    std::string title = Ne_PARAM_STRING(0);
    int x = Ne_PARAM_INT(1);
    int y = Ne_PARAM_INT(2);
    int w = Ne_PARAM_INT(3);
    int h = Ne_PARAM_INT(4);
    int flags = Ne_PARAM_INT(5);

    Ne_RETURN_POINTER(SDL_CreateWindow(title.c_str(), x, y, w, h, flags));
}

Ne_FUNC(Ne_Delay)
{
    int ms = Ne_PARAM_INT(0);

    SDL_Delay(ms);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_DestroyRenderer)
{
    SDL_Renderer *renderer = Ne_PARAM_POINTER(SDL_Renderer, 0);

    SDL_DestroyRenderer(renderer);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_DestroyTexture)
{
    SDL_Texture *texture = Ne_PARAM_POINTER(SDL_Texture, 0);

    SDL_DestroyTexture(texture);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_DestroyWindow)
{
    SDL_Window *window = Ne_PARAM_POINTER(SDL_Window, 0);

    SDL_DestroyWindow(window);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_FreeSurface)
{
    SDL_Surface *surface = Ne_PARAM_POINTER(SDL_Surface, 0);

    SDL_FreeSurface(surface);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_Init)
{
    int flags = Ne_PARAM_INT(0);

    SDL_Init(flags);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_LoadBMP)
{
    std::string file = Ne_PARAM_STRING(0);

    Ne_RETURN_POINTER(SDL_LoadBMP(file.c_str()));
}

Ne_FUNC(Ne_PollEvent)
{
    Ne_Cell *event = Ne->parameterlist_set_cell(out_params, 0);

    SDL_Event e;
    int r = SDL_PollEvent(&e);
    if (r != 0) {
        unpack_Event(event, e);
    }
    Ne_RETURN_BOOL(r != 0);
}

Ne_FUNC(Ne_Quit)
{
    SDL_Quit();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_RenderClear)
{
    SDL_Renderer *renderer = Ne_PARAM_POINTER(SDL_Renderer, 0);

    SDL_RenderClear(renderer);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_RenderCopy)
{
    SDL_Renderer *renderer = Ne_PARAM_POINTER(SDL_Renderer, 0);
    SDL_Texture *texture = Ne_PARAM_POINTER(SDL_Texture, 1);
    const Ne_Cell *srcrect = Ne->parameterlist_get_cell(in_params, 2);
    const Ne_Cell *dstrect = Ne->parameterlist_get_cell(in_params, 3);

    SDL_Rect src, dst;
    SDL_RenderCopy(renderer, texture, pack_Rect(src, srcrect), pack_Rect(dst, dstrect));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_RenderDrawLine)
{
    SDL_Renderer *renderer = Ne_PARAM_POINTER(SDL_Renderer, 0);
    int x1 = Ne_PARAM_INT(1);
    int y1 = Ne_PARAM_INT(2);
    int x2 = Ne_PARAM_INT(3);
    int y2 = Ne_PARAM_INT(4);

    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_RenderDrawLines)
{
    SDL_Renderer *renderer = Ne_PARAM_POINTER(SDL_Renderer, 0);
    const Ne_Cell *points = Ne->parameterlist_get_cell(in_params, 1);
    int n = Ne->cell_get_array_size(points);
    std::vector<SDL_Point> p(n);
    for (int i = 0; i < n; i++) {
        p[i].x = Ne->cell_get_number_int(Ne->cell_get_array_cell(Ne->cell_get_array_cell(points, i), 0));
        p[i].y = Ne->cell_get_number_int(Ne->cell_get_array_cell(Ne->cell_get_array_cell(points, i), 1));
    }

    SDL_RenderDrawLines(renderer, p.data(), static_cast<int>(p.size()));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_RenderDrawPoint)
{
    SDL_Renderer *renderer = Ne_PARAM_POINTER(SDL_Renderer, 0);
    int x = Ne_PARAM_INT(1);
    int y = Ne_PARAM_INT(2);

    SDL_RenderDrawPoint(renderer, x, y);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_RenderFillRect)
{
    SDL_Renderer *renderer = Ne_PARAM_POINTER(SDL_Renderer, 0);
    const Ne_Cell *r = Ne->parameterlist_get_cell(in_params, 1);

    SDL_Rect rect;
    SDL_RenderFillRect(renderer, pack_Rect(rect, r));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_RenderPresent)
{
    SDL_Renderer *renderer = Ne_PARAM_POINTER(SDL_Renderer, 0);

    SDL_RenderPresent(renderer);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_SetRenderDrawColor)
{
    SDL_Renderer *renderer = Ne_PARAM_POINTER(SDL_Renderer, 0);
    int r = Ne_PARAM_INT(1);
    int g = Ne_PARAM_INT(2);
    int b = Ne_PARAM_INT(3);
    int a = Ne_PARAM_INT(4);

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    return Ne_SUCCESS;
}

} // extern "C"
