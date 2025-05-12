#pragma once
#include <SDL.h>
#include <SDL_image.h>
struct Settings {
    int window_width;
    int window_height;
    int cell_size;
    int menu_height;
    int menu_width;
    int reset_button_height;
    int reset_button_width;
    bool game_over;
    bool first_click;
    bool regenerate;
    bool seed_received;
    bool coords_received;
    int mine_number;
};

extern Settings config;
typedef struct {
    SDL_Texture *mine;
    SDL_Texture *clicked_mine;
    SDL_Texture *flag;
    SDL_Texture *prox1;
    SDL_Texture *prox2;
    SDL_Texture *prox3;
    SDL_Texture *prox4;
    SDL_Texture *prox5;
    SDL_Texture *prox6;
    SDL_Texture *prox7;
    SDL_Texture *prox8;

} GameAssets;
int loadGameAssets(SDL_Renderer *renderer, GameAssets *assets);