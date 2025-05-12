#include <SDL.h>
#include <SDL_ttf.h>
SDL_Texture* RenderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int &textWidth, int &textHeight);
