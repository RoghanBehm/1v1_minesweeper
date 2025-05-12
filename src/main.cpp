#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>
#include <SDL_main.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>
#include <asio.hpp>
#include "imgui.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "backends/imgui_impl_sdl2.h"
#include "settings.hpp"
#include "game.hpp"
#include "render.hpp"
#include "mouseProps.hpp"
#include "process.hpp"
#include "mouse_utils.hpp"
#include "../client/client.hpp"
#include "ui.hpp"

int main() {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialise SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Minesweeper",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          config.window_width,
                                          config.window_height,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // ImGui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark(); // Optional: choose your style

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    
   

    SDL_RenderSetLogicalSize(renderer, config.window_width, config.window_height);

    if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0) {
        std::cerr << "SDL_SetWindowFullscreen Error: " << SDL_GetError() << std::endl;
    }


    Draw draw;
    GameAssets assets;

    // Load assets
    if (loadGameAssets(renderer, &assets) != 0) {
        printf("Failed to load game assets.\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }


    bool running = true;
    SDL_Event event;
    const int frameDelay = 1000 / 60;
    Uint32 frameStart = SDL_GetTicks();

    if (TTF_Init() == -1) {
        std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
        return 1;
    }

    std::string basePath(SDL_GetBasePath());
    std::string fontPath = basePath + "../fonts/Helvetica.ttf";

    TTF_Font *font = TTF_OpenFont(fontPath.c_str(), 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return 1;
    }

    int menuChoice = mainMenu(renderer);

    if (menuChoice == -1) {
    SDL_Quit();
    return 0;
}

std::string ipBuffer = "127.0.0.1";
std::string portBuffer = "8000";
std::string mineNumBuffer = "30";
bool readyToJoin = false;


if (menuChoice == 1) {
    // Player is host
    while (!readyToJoin) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                return 0;
            }
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        draw.DrawHostUI("Host Game", portBuffer, mineNumBuffer, readyToJoin);

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
    // think i need to pass portBuffer and mineNumBuffer to the spawn_process. could do a lambda capture? or just move in scope with an if (readToJoin)
    #ifdef _WIN32
    std::thread([portBuffer]{
        spawn_process("server.exe", {std::to_string(config.mine_number), portBuffer});
    }).detach();
    #else
    std::thread([portBuffer]{
        spawn_process("serverr", {std::to_string(config.mine_number),portBuffer});
    }).detach();
    
    #endif




} else if (menuChoice == 2) {
    // Show ImGui popup now
    while (!readyToJoin) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                return 0;
            }
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        draw.DrawJoinUI("Join Game", ipBuffer, portBuffer, readyToJoin);

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
}

    Uint32 menuExitTime = SDL_GetTicks();

    // ImGui input loop
    

    asio::io_context io_context;

    // Initialise client
    NetworkClient client(io_context, ipBuffer, portBuffer);
 
    std::thread io_thread([&io_context]() { io_context.run(); });

    std::vector<std::pair<int, int>> all_coords;
    
    // Initialize game objects
    while (!config.seed_received) {};
    const int rows = 16;
    const int cols = 30;
    Game game(rows, cols, config.mine_number);
    MouseProps mouseProps;

    int window_w, window_h;

    while (running) {

        // Handle window resizing
        SDL_GetWindowSize(window, &window_w, &window_h);

        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_f) {
                    Uint32 fullscreen_flag = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
                    SDL_SetWindowFullscreen(window, fullscreen_flag ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
                }
            } else {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    if (event.button.button == SDL_BUTTON_RIGHT) {
                        getScaledMousePosition(window_w, window_h, mouseProps.mouseXc, mouseProps.mouseYc);
                        mouseProps.rightClicked = true;
                    } else {
                        if (SDL_GetTicks() - menuExitTime < 300) {    // Debounce menu click
                            continue;
                        }
                        getScaledMousePosition(window_w, window_h, mouseProps.mouseX, mouseProps.mouseY);
                        mouseProps.mouseDown = true;
                    }
                } else if (event.type == SDL_MOUSEBUTTONUP) {
                    if (event.button.button == SDL_BUTTON_RIGHT) {
                        mouseProps.rightClicked = false;
                    } else if (game.popupActive) {
                        int mouseX, mouseY;
                        getScaledMousePosition(window_w, window_h, mouseX, mouseY);

                        // Check if OK button is clicked
                        SDL_Rect okButton = draw.okButtonRect;
                        if (mouseX >= okButton.x && mouseX <= okButton.x + okButton.w &&
                            mouseY >= okButton.y && mouseY <= okButton.y + okButton.h) {
                            game.popupActive = false;
        }
                    } else {
                        getScaledMousePosition(window_w, window_h, mouseProps.mouseXr, mouseProps.mouseYr);
                        mouseProps.mouseDown = false;
                        mouseProps.released = true;
                    }
                } else if (event.type == SDL_MOUSEMOTION && mouseProps.mouseDown) {
                    getScaledMousePosition(window_w, window_h, mouseProps.mouseX, mouseProps.mouseY);
                }
            }
        }

        // Draw background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
 
        // Reset game if regenerate
        if (config.regenerate) {
            client.clearEnemyData();
            all_coords.clear();
            game.reset();
            config.regenerate = false;
            config.first_click = true;
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        game.update(client);

        auto board = client.return_board();
        for (auto x : board) {
            all_coords.push_back(x);
        }

        const int btn_x = (cols + 0) * config.cell_size;
        const int btn_y = (rows + 4) * config.cell_size;
        const int btn_w = config.reset_button_width;
        const int btn_h = config.reset_button_height;

        // Reset button
        ImGui::SetNextWindowPos(ImVec2((float)btn_x, (float)btn_y));
        ImGui::SetNextWindowSize(ImVec2((float)btn_w, (float)btn_h));
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBackground;
        
        ImGui::Begin("##ResetBtnWindow", nullptr, flags);
        
        if (ImGui::Button("Reset", ImVec2((float)btn_w, (float)btn_h)))
            game.requestRestart(client);
        
        ImGui::End();
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

        // Render player grids
        game.createGrid(renderer, client, mouseProps, assets, draw);
        game.createEnemyGrid(renderer, mouseProps, assets, draw, all_coords);

        if (config.game_over && !game.win && !game.lose) {
            game.lose = true;
            game.popupActive = true;
            game.resultReturned = true;
        }
        
        // Check win/loss
        if (!game.win && !game.lose && !config.game_over) {
            game.checkWin();
        }

        int result = client.return_res();
        if (result == 2 && !game.resultReturned) { // Loss
            game.lose = true;
            game.popupActive = true;
            game.resultReturned = true;
        }
        else if (result == 1 && !game.resultReturned) { // Win
            game.win = true;
            game.popupActive = true;
            game.resultReturned = true;
        }


        if (game.win) {
            if (game.popupActive) {
                draw.blackFilter(renderer);
                draw.Popup(renderer, font, "You wonnered!");
                if (!game.resultSent) {
                    game.sendWin(client);
                    game.resultSent = true;
                }
            }

        }

        if (game.lose || config.game_over) {
            if (game.popupActive) {
                draw.blackFilter(renderer);
                draw.Popup(renderer, font, "You lose!");
                if (!game.resultSent) {
                    game.sendLoss(client);
                    game.resultSent = true;
                }
                
            }            
        }
        

        SDL_RenderPresent(renderer);
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    // Clean
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
