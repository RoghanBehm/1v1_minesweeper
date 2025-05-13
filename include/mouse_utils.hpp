#pragma once
#include <SDL.h>
#include <algorithm>

inline void getScaledMousePosition(int window_w, int window_h, int &scaled_x, int &scaled_y) {
    int raw_x, raw_y;
    SDL_GetMouseState(&raw_x, &raw_y);

    float scale_x = static_cast<float>(window_w) / 1920.0f;
    float scale_y = static_cast<float>(window_h) / 1080.0f;
    float scale = std::min(scale_x, scale_y);

    int offset_x = static_cast<int>((window_w - 1920 * scale) / 2.0f);
    int offset_y = static_cast<int>((window_h - 1080 * scale) / 2.0f);

    int game_right = static_cast<int>(offset_x + 1920 * scale);
    int game_bottom = static_cast<int>(offset_y + 1080 * scale);

    if (raw_x < offset_x || raw_x >= game_right ||
        raw_y < offset_y || raw_y >= game_bottom) {
        scaled_x = -1;
        scaled_y = -1;
        return;
    }

    scaled_x = static_cast<int>((raw_x - offset_x) / scale + 0.5f);
    scaled_y = static_cast<int>((raw_y - offset_y) / scale + 0.5f);
}
