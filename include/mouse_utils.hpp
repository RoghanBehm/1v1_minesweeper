#pragma once
#include <SDL.h>

inline void getScaledMousePosition(int window_w, int window_h, int &scaled_x, int &scaled_y) {
    int raw_x, raw_y;
    SDL_GetMouseState(&raw_x, &raw_y);
    
    float scale_x = window_w / 1920.0f;
    float scale_y = window_h / 1080.0f;

    scaled_x = static_cast<int>(raw_x / scale_x);
    scaled_y = static_cast<int>(raw_y / scale_y);
}