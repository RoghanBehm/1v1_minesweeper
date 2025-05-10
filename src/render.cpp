#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include "render.hpp"
#include "settings.hpp"
#include "game.hpp"
#include "helper.hpp"
#include "imgui.h"


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
void Draw::cell(SDL_Renderer *renderer, int x, int y, bool &clicked, bool &released, Node &cell, Game &game, const GameAssets &assets, int nearbyMines, int row, int col, bool isPlayer) {
    SDL_Rect rect = {x, y, globalSettings.cell_size, globalSettings.cell_size};

    // If this is the safe start cell, fill it with gold
    if (globalSettings.first_click && row == game.safeStartCell.first && col == game.safeStartCell.second) {
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Gold
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 123, 123, 123, 255);
        SDL_RenderDrawRect(renderer, &rect);



    } else {
        default_cell(renderer, rect); // Otherwise, draw the default cell
    }

    if (cell.isFlagged) {
        SDL_RenderCopy(renderer, assets.flag, NULL, &rect);
        return;
    }

    if ((globalSettings.game_over && cell.hasMine) && !globalSettings.regenerate) {
        if (cell.exploded) {
            SDL_RenderCopy(renderer, assets.clicked_mine, NULL, &rect);
            return;
        }
        SDL_RenderCopy(renderer, assets.mine, NULL, &rect);
        return;
    }

    if ((released || cell.isRevealed)) {
        if (released && !globalSettings.game_over) {
            game.revealCell(row, col);
            released = false;
        }

        if (cell.hasMine && !globalSettings.regenerate) {
            SDL_RenderCopy(renderer, assets.mine, NULL, &rect);
            globalSettings.game_over = true;
            return;
        }

        // If this is the player grid, set first click to false
        if (isPlayer) {
         globalSettings.first_click = false;
        }
        

        if (cell.isRevealed) {
            mine_prox_cell(renderer, assets, nearbyMines, rect);
            return;
        }
    } else if (clicked && !globalSettings.game_over) {
        SDL_SetRenderDrawColor(renderer, 189, 189, 189, 255);
        SDL_RenderFillRect(renderer, &rect);
        return;
    }
}

void Draw::menu(SDL_Renderer *renderer, int x, int y, bool &clicked, bool &released) {

    // Menu
    SDL_SetRenderDrawColor(renderer, 123, 123, 123, 255);
    SDL_Rect menu = {30, 0, globalSettings.menu_width, globalSettings.menu_height};
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

void Draw::titleButton(SDL_Renderer* renderer, TTF_Font* font, const char* label, int x, int y, int w, int h) {
    // Draw the button background
    SDL_Rect buttonRect = { x, y, w, h };
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // Blue background
    SDL_RenderFillRect(renderer, &buttonRect);

    // Set the text color (SDL_Color is a real type; it takes RGBA values)
    SDL_Color textColor = { 255, 255, 255, 255 };  // White

    // Render the label text
    int textWidth = 0, textHeight = 0;
    SDL_Texture* textTexture = RenderText(renderer, font, label, textColor, textWidth, textHeight);
    if (!textTexture)
        return;  // Early exit on error

    // Center the text in the button
    SDL_Rect textRect = {
        x + (w - textWidth) / 2,
        y + (h - textHeight) / 2,
        textWidth,
        textHeight
    };

    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
}


// Returns 1 for "Host Game", 2 for "Join Game", and -1 for quit.
int Draw::mainMenu(SDL_Renderer* renderer, TTF_Font* font) {
    bool menuRunning = true;
    SDL_Event event;
    
    while (menuRunning) {
        SDL_RenderClear(renderer);

        
        int button_width = 200;
        int button_height = 50;
        int spacing = 20;

        int x = (globalSettings.window_width - button_width) / 2;
        int y1 = (globalSettings.window_height - (2 * button_height + spacing)) / 2;
        int y2 = y1 + button_height + spacing;

        // Draw menu items
        titleButton(renderer, font, "Host Game", x, y1, button_width, button_height);
        titleButton(renderer, font, "Join Game", x, y2, button_width, button_height);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                return -1;

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // Check if menu items are clicked
                if (mouseX >= x && mouseX <= x + button_width) {
                    if (mouseY >= y1 && mouseY <= y1 + button_height)
                        return 1;
                    if (mouseY >= y2 && mouseY <= y2 + button_height)
                        return 2;
                }

            }
        }
        SDL_Delay(10);
    }
    return 0;
}


void Draw::DrawJoinHostUI(const char* title, std::string& ipBuffer, std::string& portBuffer, bool& readyFlag) {
    float winX = static_cast<float>(globalSettings.window_width);
    float winY = static_cast<float>(globalSettings.window_height);
    ImVec2 centerPos = ImVec2(winX / 2.0f - 200.0f, winY / 2.0f - 100.0f);

    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Always);
    ImGui::SetNextWindowPos(centerPos, ImGuiCond_Always);

    ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    static char ipInput[64];
    static char portInput[16];

    strncpy(ipInput, ipBuffer.c_str(), sizeof(ipInput));
    strncpy(portInput, portBuffer.c_str(), sizeof(portInput));

    ImGui::InputText("IP Address", ipInput, IM_ARRAYSIZE(ipInput));
    ImGui::InputText("Port", portInput, IM_ARRAYSIZE(portInput));

    if (ImGui::Button("Connect")) {
        ipBuffer = ipInput;
        portBuffer = portInput;
        readyFlag = true;
    }

    ImGui::End();
}

