#include <SDL/SDL.h>
#include "controller.h"

void controller(){
    Controller* controller1 = nullptr;
    Controller* controller2 = nullptr;
    Uint8* keystates = SDL_GetKeyState(NULL);
    SDL_Rect* topScreen = new SDL_Rect; topScreen->x = 0; topScreen->y = 0;
    SDL_Rect* bottomScreen = new SDL_Rect; bottomScreen->x = 0; bottomScreen->y = 501;

    //start SDL
    if(SDL_Init ( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ){
        //cout << "Couldn't initialize SDL: " << SDL_GetError() << endl;
        exit(1);
    }
    //make window
    SDL_Surface* window = SDL_SetVideoMode(771,1002,0,0);

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
    else{
        controller2 = new Controller(NULL);
    }

    while(keystates[SDLK_ESCAPE] == 0){
        SDL_PumpEvents();
        controller1->getStates();
        controller2->getStates(); //package and send these

        SDL_BlitSurface(controller1->getScreen(),NULL,window,topScreen);
        SDL_BlitSurface(controller2->getScreen(),NULL,window,bottomScreen);

        SDL_Flip(window);
    }

    SDL_Quit();
}


int main(int argc, char* argv[]){
    controller();
}
