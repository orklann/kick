//
//  SDL2_Context.h
//  KickCPP
//
//  Created by morten on 8/10/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#pragma once
#include "context.h"


#if defined(EMSCRIPTEN)
#   include <emscripten.h>
#   include <SDL/SDL.h>
#   include <SDL/SDL_image.h>
#elif defined(_WIN32)
#   define SDL_MAIN_HANDLED
#   include <SDL.h>
#else
#   define SDL_MAIN_HANDLED
#   include <SDL2/SDL.h>
#endif

namespace kick {
    class SDL2Context : public Context {
    public:
        SDL2Context();
        SDL2Context(const SDL2Context&) = delete;
        virtual ~SDL2Context();
        virtual bool init(int &argc, char **argv) override;
        virtual bool showWindow(const WindowConfig& config = WindowConfig::plain)  override;
        virtual void swapBuffer() override;
        virtual void mainLoop()  override;
        std::string getBasePath();

        virtual bool isFullscreen() override;

        virtual void setFullscreen(bool fullscreen) override;

        virtual void setWindowTitle(std::string title) override;

        virtual std::string getWindowTitle() override;

        void step();
        bool tick();
    private:
        void handleMouseMotion(SDL_MouseMotionEvent event);
        void handleMouseButton(SDL_MouseButtonEvent event, bool buttonDown);
        void handleMouseWheel(SDL_MouseWheelEvent event);
        void handleKey(SDL_KeyboardEvent event, bool keyDown);
        void handleTouch(SDL_TouchFingerEvent event);
        Uint32 timeLeft();
        Uint32 nextTime;
        Uint32 tickInterval = 16; // 60 fps
        SDL_Window *window = nullptr;
        SDL_GLContext glContext = nullptr;
        std::string basePath;
    };
};