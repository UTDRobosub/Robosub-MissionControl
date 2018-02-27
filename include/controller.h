//controller.h

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL/SDL.h>
#include <string>

class Controller{
    private:
        SDL_Joystick* joystick = nullptr;

    public:
        void getStates(int*);
        void setJoystick(SDL_Joystick*);
};

#endif
