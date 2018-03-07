//controller.h
#pragma once

#include <SDL/SDL.h>
#include <string>

class Controller{
    private:
        SDL_Joystick* joystick = nullptr;

    public:
        void getStates(int*);
        void setJoystick(SDL_Joystick*);
};
