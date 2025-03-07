#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include "render.hpp"
#include "settings.hpp"
#include "game.hpp"


void Draw::default_cell(SDL_Renderer *renderer, SDL_Rect rect) {
        // Fill colour
    SDL_SetRenderDrawColor(renderer, 189, 189, 189, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Top side (white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect top = {rect.x + inset, rect.y, rect.w - 2 * inset, thickness};
    SDL_RenderFillRect(renderer, &top);

    // Bottom side (grey)
    SDL_SetRenderDrawColor(renderer, 123, 123, 123, 255);
    SDL_Rect bottom = {rect.x + inset, rect.y + rect.h - thickness, rect.w - 2 * inset, thickness};
    SDL_RenderFillRect(renderer, &bottom);

    // Left side (white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect left = {rect.x, rect.y + inset, thickness, rect.h - 2 * inset};
    SDL_RenderFillRect(renderer, &left);

    // Right side (grey)
    SDL_SetRenderDrawColor(renderer, 123, 123, 123, 255);
    SDL_Rect right = {rect.x + rect.w - thickness, rect.y + inset, thickness, rect.h - 2 * inset};
    SDL_RenderFillRect(renderer, &right);
}

void Draw::mine_prox_cell(SDL_Renderer *renderer, const GameAssets &assets, int nearbyMines, SDL_Rect rect) {
            switch(nearbyMines) {
            case 1:
                SDL_RenderCopy(renderer, assets.prox1, NULL, &rect);
                break;
            case 2:
                SDL_RenderCopy(renderer, assets.prox2, NULL, &rect);
                break;
            case 3:
                SDL_RenderCopy(renderer, assets.prox3, NULL, &rect);
                break;
            case 4:
                SDL_RenderCopy(renderer, assets.prox4, NULL, &rect);
                break;
            case 5:
                SDL_RenderCopy(renderer, assets.prox5, NULL, &rect);
                break;
            case 6:
                SDL_RenderCopy(renderer, assets.prox6, NULL, &rect);
                break;
            case 7:
                SDL_RenderCopy(renderer, assets.prox7, NULL, &rect);
                break;
            case 8:
                SDL_RenderCopy(renderer, assets.prox8, NULL, &rect);
                break;
            default:
                SDL_SetRenderDrawColor(renderer, 189, 189, 189, 255);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 123, 123, 123, 255);
                SDL_RenderDrawRect(renderer, &rect);
                break;
        }
}

void Draw::cell(SDL_Renderer *renderer, int x, int y, bool &clicked, bool &released, Node &cell, Game &game, const GameAssets &assets, int nearbyMines, int row, int col) {
    SDL_Rect rect = {x, y, globalSettings.cell_size, globalSettings.cell_size};

    if (cell.isFlagged) {
        SDL_SetRenderDrawColor(renderer, 189, 189, 189, 255);
        SDL_RenderCopy(renderer, assets.flag, NULL, &rect);
        return;
    }

    if ((globalSettings.game_over && cell.hasMine) && !globalSettings.regenerate) {
            if (cell.exploded) {
                SDL_RenderCopy(renderer, assets.clicked_mine, NULL, &rect);
                return;
            }
            if (released) {
            game.setExploded(row, col);

        }
        SDL_RenderCopy(renderer, assets.mine, NULL, &rect);
        return;
    }

    if ((released || cell.isRevealed)) {
        if (released && !globalSettings.game_over) {
            game.revealCell(row, col);
            released = false;
        }

        if (game.checkSurrounding(row, col) && globalSettings.first_click) {
                cell.hasMine = false;
                globalSettings.regenerate = true;
                globalSettings.first_click = false;
                return;
        }

        if (cell.hasMine && !globalSettings.regenerate) {
            if (globalSettings.first_click) {
                cell.hasMine = false;
                globalSettings.regenerate = true;
                globalSettings.first_click = false;
                return;
            }
            SDL_RenderCopy(renderer, assets.mine, NULL, &rect);
            globalSettings.game_over = true;
            return;
        }
        globalSettings.first_click = false;

        if (cell.isRevealed) {
            mine_prox_cell(renderer, assets, nearbyMines, rect);  
            return;
        }
        default_cell(renderer, rect);
        return;
        
    } else if (clicked && !globalSettings.game_over) {
        SDL_SetRenderDrawColor(renderer, 189, 189, 189, 255);
        SDL_RenderFillRect(renderer, &rect);
        return;
    }
    default_cell(renderer, rect);
}


void Draw::menu(SDL_Renderer *renderer, int x, int y, bool &clicked, bool &released) {

    // Menu
    SDL_SetRenderDrawColor(renderer, 123, 123, 123, 255);
    SDL_Rect menu = {0, 0, globalSettings.menu_width, globalSettings.menu_height};
    SDL_RenderFillRect(renderer, &menu);

    // Reset button

    if (!clicked) {
        SDL_SetRenderDrawColor(renderer, 0, 10, 103, 0);
    } else {
        SDL_SetRenderDrawColor(renderer, 10, 120, 13, 40);
    }
    
    SDL_Rect reset_button = {
        x,
        y,
        globalSettings.reset_button_height,
        globalSettings.reset_button_width
    };
    SDL_RenderFillRect(renderer, &reset_button);

    if (released) {
        SDL_SetRenderDrawColor(renderer, 0, 10, 103, 0);
        SDL_RenderFillRect(renderer, &reset_button);
    }

            
}

void Draw::blackFilter(SDL_Renderer *renderer)
{
    SDL_Texture *filter = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, globalSettings.window_width, globalSettings.window_height);

    if (!filter) {
        std::cerr << "Failed to create texture for black filter: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_SetTextureBlendMode(filter, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, filter);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, filter, NULL, NULL);
    SDL_DestroyTexture(filter);
}


void Draw::Popup(SDL_Renderer *renderer, TTF_Font *font, const char *message) {
    SDL_Rect popupRect = {globalSettings.window_width / 4, globalSettings.window_height / 4, 
                          globalSettings.window_width / 2, globalSettings.window_height / 2};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &popupRect);

    // Draw message text
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, message, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_Rect textRect = {popupRect.x + (popupRect.w - textWidth) / 2, 
                         popupRect.y + (popupRect.h - textHeight) / 3, 
                         textWidth, textHeight};
    SDL_FreeSurface(textSurface);
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);

    // Draw OK button
    SDL_Rect buttonRect = {popupRect.x + (popupRect.w / 2 - 50), 
                           popupRect.y + popupRect.h - 60, 
                           100, 40}; 

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &buttonRect);

    // Draw OK text
    SDL_Surface *buttonSurface = TTF_RenderText_Solid(font, "OK", textColor);
    SDL_Texture *buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);

    int buttonTextWidth = buttonSurface->w;
    int buttonTextHeight = buttonSurface->h;
    SDL_Rect buttonTextRect = {buttonRect.x + (buttonRect.w - buttonTextWidth) / 2, 
                               buttonRect.y + (buttonRect.h - buttonTextHeight) / 2, 
                               buttonTextWidth, buttonTextHeight};
    SDL_FreeSurface(buttonSurface);
    SDL_RenderCopy(renderer, buttonTexture, NULL, &buttonTextRect);
    SDL_DestroyTexture(buttonTexture);

    okButtonRect = buttonRect;
}



