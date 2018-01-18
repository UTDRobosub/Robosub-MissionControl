#include <SDL/SDL.h>
#include "controller.h"
#include <iostream>
using namespace std;

void controller(){

    //start SDL
    if(SDL_Init ( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ){
        cout << "Couldn't initialize SDL: " << SDL_GetError() << endl;
        exit(1);
    }

    //create controllers
    Controller* controller1 = new Controller;
    Controller* controller2 = new Controller;
    controller1->setJoystick(SDL_JoystickOpen(0));
    controller2->setJoystick(SDL_JoystickOpen(1));

    //for event handling
    SDL_Event e;
    bool running = true;

    //make window, important locations in window
    SDL_Surface* window = SDL_SetVideoMode(771,1002,0,0);
    SDL_Rect* topScreen = new SDL_Rect; topScreen->x = 0; topScreen->y = 0;
    SDL_Rect* bottomScreen = new SDL_Rect; bottomScreen->x = 0; bottomScreen->y = 501;

    while(running){

        //event queue for key events
        SDL_PumpEvents();
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT )
                running = false;
            else if( e.type == SDL_KEYDOWN ) {
                if(e.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
                else if(e.key.keysym.sym == SDLK_r){
                  SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
                  SDL_InitSubSystem(SDL_INIT_JOYSTICK);
                  controller1->setJoystick(SDL_JoystickOpen(0));
                  controller2->setJoystick(SDL_JoystickOpen(1));
                }
            }
        }

        //update and return joystick states
        controller1->getStates();
        controller2->getStates(); //package and send these

        //update gui
        SDL_BlitSurface(controller1->getScreen(),NULL,window,topScreen);
        SDL_BlitSurface(controller2->getScreen(),NULL,window,bottomScreen);
        SDL_Flip(window);
    }

    SDL_Quit();
}


int main(int argc, char* argv[]){
    controller();
}
