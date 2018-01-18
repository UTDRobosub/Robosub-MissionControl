#include <SDL/SDL.h>
#include "controller.h"

void controller(){
    Controller* controller1 = nullptr;
    Controller* controller2 = nullptr;
    Uint8* keystates = SDL_GetKeyState(NULL);

    //start SDL
    if(SDL_Init ( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
    {
        //cout << "Couldn't initialize SDL: " << SDL_GetError() << endl;
        exit(1);
    }

    if(SDL_NumJoysticks() > 0)
    {
        controller1 = new Controller(SDL_JoystickOpen(0));
    }
    else{
        controller1 = new Controller(NULL);
    }
    if(SDL_NumJoysticks() > 1)
    {
        controller2 = new Controller(SDL_JoystickOpen(1));
    }

    while(keystates[SDLK_ESCAPE] == 0){
        SDL_PumpEvents();
        controller1->updateStates();
        controller1->updateGUI();
        if(controller2 != nullptr){
            controller2->updateStates();
            controller2->updateGUI();
            //not working yet
            //SDL won't allow multiple windows in same thread
            //will change to write both surfaces to same window
        }
    }

    SDL_Quit();
}


int main(int argc, char* argv[]){
    controller();
}
