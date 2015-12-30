#include <SDL.h>

#include "cell.h"

SDL_Rect *pack_Rect(SDL_Rect &out, Cell &in)
{
    if (in.array_index_for_write(4).boolean()) {
        return NULL;
    }
    out.x = number_to_sint32(in.array_index_for_read(0).number());
    out.y = number_to_sint32(in.array_index_for_read(1).number());
    out.w = number_to_sint32(in.array_index_for_read(2).number());
    out.h = number_to_sint32(in.array_index_for_read(3).number());
    return &out;
}

void unpack_Event(Cell *out, const SDL_Event &in)
{
    out->array_index_for_write(0) = Cell(number_from_uint32(in.type));
    switch (in.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            out->array_index_for_write(1).array_index_for_write(0) = Cell(number_from_uint32(in.key.timestamp));
            out->array_index_for_write(1).array_index_for_write(1) = Cell(number_from_uint32(in.key.windowID));
            out->array_index_for_write(1).array_index_for_write(2) = Cell(number_from_uint8(in.key.state));
            out->array_index_for_write(1).array_index_for_write(3) = Cell(number_from_uint8(in.key.repeat));
            out->array_index_for_write(1).array_index_for_write(4).array_index_for_write(0) = Cell(number_from_uint32(in.key.keysym.scancode));
            out->array_index_for_write(1).array_index_for_write(4).array_index_for_write(1) = Cell(number_from_uint32(in.key.keysym.sym));
            out->array_index_for_write(1).array_index_for_write(4).array_index_for_write(2) = Cell(number_from_uint16(in.key.keysym.mod));
            break;
    }
}

namespace rtl {

void *sdl$CreateRGBSurface(Number flags, Number width, Number height, Number depth, Number Rmask, Number Gmask, Number Bmask, Number Amask)
{
    return SDL_CreateRGBSurface(
        number_to_uint32(flags),
        number_to_sint32(width),
        number_to_sint32(height),
        number_to_sint32(depth),
        number_to_uint32(Rmask),
        number_to_uint32(Gmask),
        number_to_uint32(Bmask),
        number_to_uint32(Amask)
    );
}

void *sdl$CreateRenderer(void *window, Number index, Number flags)
{
    return SDL_CreateRenderer(static_cast<SDL_Window *>(window), number_to_sint32(index), number_to_uint32(flags));
}

void *sdl$CreateSoftwareRenderer(void *surface)
{
    return SDL_CreateSoftwareRenderer(static_cast<SDL_Surface *>(surface));
}

void *sdl$CreateTextureFromSurface(void *renderer, void *surface)
{
    return SDL_CreateTextureFromSurface(static_cast<SDL_Renderer *>(renderer), static_cast<SDL_Surface *>(surface));
}

void *sdl$CreateWindow(const std::string &title, Number x, Number y, Number w, Number h, Number flags)
{
    return SDL_CreateWindow(title.c_str(), number_to_sint32(x), number_to_sint32(y), number_to_sint32(w), number_to_sint32(h), number_to_uint32(flags));
}

void sdl$Delay(Number ms)
{
    SDL_Delay(number_to_uint32(ms));
}

void sdl$DestroyRenderer(void *renderer)
{
    SDL_DestroyRenderer(static_cast<SDL_Renderer *>(renderer));
}

void sdl$DestroyTexture(void *texture)
{
    SDL_DestroyTexture(static_cast<SDL_Texture *>(texture));
}

void sdl$DestroyWindow(void *window)
{
    SDL_DestroyWindow(static_cast<SDL_Window *>(window));
}

void sdl$FreeSurface(void *surface)
{
    SDL_FreeSurface(static_cast<SDL_Surface *>(surface));
}

void sdl$Init(Number flags)
{
    SDL_Init(number_to_uint32(flags));
}

void *sdl$LoadBMP(const std::string &file)
{
    return SDL_LoadBMP(file.c_str());
}

bool sdl$PollEvent(Cell *event)
{
    SDL_Event e;
    int r = SDL_PollEvent(&e);
    if (r != 0) {
        unpack_Event(event, e);
    }
    return r != 0;
}

void sdl$Quit()
{
    SDL_Quit();
}

void sdl$RenderClear(void *renderer)
{
    SDL_RenderClear(static_cast<SDL_Renderer *>(renderer));
}

void sdl$RenderCopy(void *renderer, void *texture, Cell &srcrect, Cell &dstrect)
{
    SDL_Rect src, dst;
    SDL_RenderCopy(static_cast<SDL_Renderer *>(renderer), static_cast<SDL_Texture *>(texture), pack_Rect(src, srcrect), pack_Rect(dst, dstrect));
}

void sdl$RenderDrawLine(void *renderer, Number x1, Number y1, Number x2, Number y2)
{
    SDL_RenderDrawLine(
        static_cast<SDL_Renderer *>(renderer),
        number_to_sint32(x1),
        number_to_sint32(y1),
        number_to_sint32(x2),
        number_to_sint32(y2)
    );
}

void sdl$RenderDrawLines(void *renderer, Cell &points)
{
    std::vector<SDL_Point> p(points.array().size());
    for (size_t i = 0; i < points.array().size(); i++) {
        p[i].x = number_to_sint32(points.array_index_for_read(i).array_index_for_read(0).number());
        p[i].y = number_to_sint32(points.array_index_for_read(i).array_index_for_read(1).number());
    }
    SDL_RenderDrawLines(
        static_cast<SDL_Renderer *>(renderer),
        p.data(),
        static_cast<int>(p.size())
    );
}

void sdl$RenderDrawPoint(void *renderer, Number x, Number y)
{
    SDL_RenderDrawPoint(
        static_cast<SDL_Renderer *>(renderer),
        number_to_sint32(x),
        number_to_sint32(y)
    );
}

void sdl$RenderFillRect(void *renderer, Cell &rect)
{
    SDL_Rect r;
    SDL_RenderFillRect(static_cast<SDL_Renderer *>(renderer), pack_Rect(r, rect));
}

void sdl$RenderPresent(void *renderer)
{
    SDL_RenderPresent(static_cast<SDL_Renderer *>(renderer));
}

void sdl$SetRenderDrawColor(void *renderer, Number r, Number g, Number b, Number a)
{
    SDL_SetRenderDrawColor(
        static_cast<SDL_Renderer *>(renderer),
        number_to_uint8(r),
        number_to_uint8(g),
        number_to_uint8(b),
        number_to_uint8(a)
    );
}

} // namespace rtl
