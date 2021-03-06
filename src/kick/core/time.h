//
// Created by morten on 18/05/14.
//

#pragma once

#include <iostream>

namespace kick {
    class Engine;

    class Time {
    public:
        // return time since startup
        static float total();
        // return time since startup
        static float delta();
        // return frame count since startup
        static long frameCount();
        friend class Engine;
    private:
        static long frame;
        static float deltaTime;
    };
}