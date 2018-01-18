//controller.h

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL/SDL.h>

class Controller{
    private:
        SDL_Joystick* joystick = nullptr;
        SDL_Surface* screen = nullptr;
        Uint8* states = nullptr; //18 elements
        static SDL_Surface** images;
        static SDL_Rect** offsets;

        SDL_Surface** loadImages();
        SDL_Rect** loadOffsets();
        void updateGUI();
        void updateStates();
    public:
        Controller();
        ~Controller();
        Uint8* getStates();
        SDL_Surface* getScreen();
        void setJoystick(SDL_Joystick*);
};

#endif