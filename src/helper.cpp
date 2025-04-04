#include "helper.hpp"
#include <iostream>

SDL_Texture* RenderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int &textWidth, int &textHeight) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) {
        std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    textWidth = surface->w;
    textHeight = surface->h;
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "Failed to create text texture: " << SDL_GetError() << std::endl;
    }
    return texture;
}
