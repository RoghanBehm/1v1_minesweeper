
#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "game.hpp"
#include "settings.hpp"


constexpr int thickness = 3;
constexpr int inset = 1;

struct Node;

class Draw {
     
private:
    void default_cell(SDL_Renderer *renderer, SDL_Rect rect);
    void mine_prox_cell(SDL_Renderer *renderer, const GameAssets &assets, int nearbyMines, SDL_Rect rect);

public:
    SDL_Rect okButtonRect;
    void cell(SDL_Renderer *renderer, int x, int y, bool &clicked, bool &released, Node &cell, Game &game, const GameAssets &assets, int nearbyMines, int row, int col, bool isPlayer);
    void menu(SDL_Renderer *renderer, int x, int y, bool &clicked, bool &released);
    void blackFilter(SDL_Renderer *renderer);
    void Popup(SDL_Renderer *renderer, TTF_Font *font, const char *message);
    void DrawJoinUI(const char* title, std::string& ipBuffer, std::string& portBuffer, bool& readyFlag);
    void DrawHostUI(const char* title, std::string& portBuffer, std::string& numMinesBuffer, bool& readyFlag);

};