#include "main.h"
#include "textures.h"
#include <functional>

using namespace std;

namespace gui
{
    bool fullscreen = false;

    void handleKeyboardEvents(SDL_Event &event)
    {
        if(event.type == SDL_KEYDOWN && engineState==S_Initialization) { engineState=S_MainMenu; } // exit splash screen
        else if (event.key.keysym.sym == SDLK_F11 && event.type == SDL_KEYDOWN) // toggle fullscreen
        {
            fullscreen = !fullscreen;
            if(fullscreen) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            else SDL_SetWindowFullscreen(window, 0);
            SDL_GetWindowSize(window, &screenw, &screenh);
        }
    }

    SDL_Rect newGameRect;
    SDL_Rect optionsRect;
    SDL_Rect exitRect;

    void handleMouseEvents(SDL_Event &event)
    {
        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Point mousePoint = {mouseX, mouseY};

            switch(engineState)
            {
                case S_MainMenu:
                    if(SDL_PointInRect(&mousePoint, &newGameRect))
                    {
                        engineState = S_LoadingScreen;
                        game::initGame();
                    }
                    else if(SDL_PointInRect(&mousePoint, &optionsRect))
                    {
                    }
                    else if(SDL_PointInRect(&mousePoint, &exitRect))
                    {
                        quit();
                    }
                    break;

                case S_InGame:
                    game::handleGameEvents(event, mousePoint);
                    break;

                default: break;
            }
        }
        else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) // window resize
        {
            screenw = event.window.data1; screenh = event.window.data2;
        }
    }

    void renderSplashScreen(string text) // showing splash screen
    {
        TextureManager& textureManager = TextureManager::getInstance();
        textureManager.draw("GameLogo", (screenw - 500) / 2, (screenh - 500) / 2, 500, 500, renderer);

        int tw, th; // text width, text height
        int textSize = 3;

        sdl::getTextSize(text, tw, th, textSize);

        int x = (screenw - tw) / 2;
        int y = (screenh - th) / 1.05f;

        sdl::renderText(text, x, y, textSize);

    }

    void renderMenu() // yeah yeah: arrays for menu items, loops and shit, let me make a playable game first
    {
        int textSize = 3;

        int x = 100, y = 200;
        std::string newGameText = "New Game";
        sdl::renderText(newGameText, x, y, textSize);
        newGameRect = {x, y, static_cast<int>(newGameText.length()) * cw * textSize, ch * textSize}; // Play

        y+=75;
        std::string optionsText = "Options";
        sdl::renderText(optionsText, x, y, textSize);
        optionsRect = {x, y, static_cast<int>(optionsText.length()) * cw * textSize, ch * textSize}; // Options

        y+=75;
        std::string exitText = "Exit";
        sdl::renderText(exitText, x, y, textSize);
        exitRect = {x, y, static_cast<int>(exitText.length()) * cw * textSize, ch * textSize}; // Exit
    }
}

