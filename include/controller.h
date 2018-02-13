//controller.h

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL/SDL.h>
#include <string>

class Controller{
    private:
        SDL_Joystick* joystick = nullptr;
        Uint8* states = nullptr; //18 elements
        void updateStates();

    public:
        Controller();
        ~Controller();
        Uint8* getStates();
        std::string toString();
        void setJoystick(SDL_Joystick*);
};

#endif
