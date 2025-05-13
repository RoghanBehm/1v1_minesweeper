#include "backends/imgui_impl_sdlrenderer2.h"
#include "backends/imgui_impl_sdl2.h"
#include "ui.hpp"

int mainMenu(SDL_Renderer* renderer) {
    bool menuRunning = true;
    SDL_Event event;
    int result = 0;

    while (menuRunning) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                return -1;
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Main Menu", nullptr,
                     ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoBackground);

        
        float window_w = ImGui::GetWindowSize().x;
        float window_h = ImGui::GetWindowSize().y;
        float button_w = 200.0f;
        float button_h = 50.0f;
        float spacing = 20.0f;

        float x = (window_w - button_w) / 2;
        float y1 = (window_h - (2 * button_h + spacing)) / 2;
        float y2 = y1 + button_h + spacing;

        ImGui::SetCursorPos(ImVec2(x, y1));

        if (ImGui::Button("Host Game", ImVec2(button_w, button_h)))
            result = 1;
    
        ImGui::SetCursorPos(ImVec2(x, y2));

        if (ImGui::Button("Join Game", ImVec2(button_w, button_h)))
            result = 2;

        ImGui::End();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);

        if (result != 0)
            return result;

        SDL_Delay(10);
    }

    return 0;
}

void PresentLogical(SDL_Renderer* renderer, SDL_Texture* logical_tex, SDL_Window* window) {

SDL_SetRenderTarget(renderer, logical_tex);
SDL_RenderClear(renderer);
ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

SDL_SetRenderTarget(renderer, nullptr);
SDL_RenderClear(renderer);
int w, h;
SDL_GetWindowSize(window, &w, &h);
SDL_Rect dst = {0, 0, w, h};
SDL_RenderCopy(renderer, logical_tex, nullptr, &dst);
SDL_RenderPresent(renderer);
}
