#include "settings.hpp"
#include <cstdio>

Settings globalSettings = {
    .window_width = 1920,
    .window_height = 1080,
    .cell_size = 30,
    .menu_height = 90,
    .menu_width = 900,
    .reset_button_height = 40,
    .reset_button_width = 40,
    .game_over = false,
    .first_click = true,
    .regenerate = false,
    .seed_received = false,
    .coords_received = false,
};


int loadGameAssets(SDL_Renderer *renderer, GameAssets *assets) {
    assets->mine = IMG_LoadTexture(renderer, "assets/Mine.png");
    if (!assets->mine) {
        printf("Failed to load mine texture: %s\n", SDL_GetError());
        return 1;
    }

    assets->clicked_mine = IMG_LoadTexture(renderer, "assets/Clicked_Mine.png");
    if (!assets->clicked_mine) {
        printf("Failed to load mine texture: %s\n", SDL_GetError());
        return 1;
    }

    assets->flag = IMG_LoadTexture(renderer, "assets/Minesweeper_flag.png");
    if (!assets->flag) {
        printf("Failed to load flag texture: %s\n", SDL_GetError());
        return 1;
    }

    assets->prox1 = IMG_LoadTexture(renderer, "assets/Minesweeper_1.png");
    if (!assets->prox1) {
        printf("Failed to load prox1 texture: %s\n", SDL_GetError());
        return 1;
    }

    assets->prox2 = IMG_LoadTexture(renderer, "assets/Minesweeper_2.png");
    if (!assets->prox2) {
        printf("Failed to load prox2 texture: %s\n", SDL_GetError());
        return 1;
    }

    assets->prox3 = IMG_LoadTexture(renderer, "assets/Minesweeper_3.png");
    if (!assets->prox3) {
        printf("Failed to load prox3 texture: %s\n", SDL_GetError());
        return 1;
    }

    assets->prox4 = IMG_LoadTexture(renderer, "assets/Minesweeper_4.png");
    if (!assets->prox4) {
        printf("Failed to load prox4 texture: %s\n", SDL_GetError());
        return 1;
    }

    assets->prox5 = IMG_LoadTexture(renderer, "assets/Minesweeper_5.png");
    if (!assets->prox5) {
        printf("Failed to load prox5 texture: %s\n", SDL_GetError());
        return 1;
    }

        assets->prox6 = IMG_LoadTexture(renderer, "assets/Minesweeper_6.png");
    if (!assets->prox6) {
        printf("Failed to load prox6 texture: %s\n", SDL_GetError());
        return 1;
    }

    assets->prox7 = IMG_LoadTexture(renderer, "assets/Minesweeper_7.png");
    if (!assets->prox7) {
        printf("Failed to load prox7 texture: %s\n", SDL_GetError());
        return 1;
    }

        assets->prox8 = IMG_LoadTexture(renderer, "assets/Minesweeper_8.png");
    if (!assets->prox8) {
        printf("Failed to load prox8 texture: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
}
