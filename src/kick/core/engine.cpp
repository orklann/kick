//
//  Engine.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 5/21/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/core/engine.h"
#include "kick/core/time.h"
#include "kick/context/sdl2_context.h"

namespace kick {
    Engine* Engine::instance = nullptr;
    
    Engine::Engine(int &argc, char **argv,const WindowConfig& config)
    :context(new SDL2Context()), tickStartTime{Time::getTime()}
    {
        context->init(argc, argv);
        context->showWindow(config);
        printf("%s (%s)\n",
                glGetString(GL_RENDERER),  // e.g. Intel HD Graphics 3000 OpenGL Engine
                glGetString(GL_VERSION)    // e.g. 3.2  INTEL-8.0.61
        );
        createScene("defaultScene");
        context->setStartFrameCallback([&](){startFrame();});
        context->setUpdateCallback([&](){update();});
        context->setRenderCallback([&](){render();});
        context->contextSurfaceSize.registerSyncValue(engineUniforms.viewportDimension);
        context->setKeyInput(&keyInput);
        context->setMouseInput(&mouseInput);
        engineUniforms.viewportDimension.setValue(context->getContextSurfaceDim());
        
        instance = this;
    }

    void Engine::startMainLoop(){
        Time::getTime(); // start timer
        context->mainLoop();
    }

    void Engine::update(){
        float now = Time::getTime();
        Time::deltaTime = now - tickStartTime;
        tickStartTime = now;
        Time::frame++;
        defaultKeyHandler.handleKeyPress(this, keyInput);
        activeScene->update();
    }
    
    void Engine::render(){
        activeScene->render(&engineUniforms);
        context->swapBuffer();
    }
    
    Scene * Engine::createScene(const char* name){
        scenes.push_back(Scene{name});
        Scene * scene = &(scenes.back());;
        if (!activeScene){
            activeScene = scene;
        }
        return scene;
    }
    
    std::vector<Scene>::const_iterator Engine::begin() const {
        return scenes.begin();
    }
    std::vector<Scene>::const_iterator Engine::end() const {
        return scenes.end();
    }
    
    std::vector<Scene>::iterator Engine::begin() {
        return scenes.begin();
    }
    std::vector<Scene>::iterator Engine::end() {
        return scenes.end();
    }
    
    Context* Engine::getContext(){
        return context;
    }

    const MouseInput& Engine::getMouseInput(){
        return mouseInput;
    }

    void Engine::startFrame() {
        keyInput.reset();
        mouseInput.reset();
    }

    const KeyInput &Engine::getKeyInput() {
        return keyInput;
    }

    DefaultKeyHandler &Engine::getDefaultKeyHandler() {
        return defaultKeyHandler;
    }
}