#include <SDL/SDL.h>
#include <iostream>
#include <thread>
#include <librobosub/robosub.h>
#include "controller.h"
#include "main.h"

using namespace std;
using namespace robosub;

void control();
void video(int);
void server();

bool running = true;
int controllerData[36];

int main(int argc, char* argv[]){
    

    int videoPort;
    //cout << "Video port: ";
    //cin >> videoPort;
    videoPort = 12345;
    

    thread controlThread(control);
    thread videoThread(video,videoPort);
    thread serverThread(server);

    controlThread.join();
    videoThread.join();
    serverThread.join();

    return 0;
}

//Thread for mission control window (everything but video feed)
//Display and send controller inputs,
//Recieve and display diagnostics
void control(){

    //start SDL
    if(SDL_Init (SDL_INIT_JOYSTICK | SDL_INIT_VIDEO) < 0 ){
        cout << "Couldn't initialize SDL: " << SDL_GetError() << endl;
        exit(1);
    }

    //Uint8 *keystate = SDL_GetKeyState(NULL);

    //get controllers
    Controller controller1;
    Controller controller2;
    controller1.setJoystick(SDL_JoystickOpen(0));
    controller2.setJoystick(SDL_JoystickOpen(1));


    SDL_Event event;
    //main loop
    while(running){

        //event loop
        SDL_PumpEvents();
        while( SDL_PollEvent( &event ) != 0 ) {
            if( event.type == SDL_QUIT )
                running = false;

            /*
            else if( event.type == SDL_KEYDOWN ) {
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
                //if r is pressed, restart joystick subsystem
                else if(event.key.keysym.sym == SDLK_r){
                    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
                    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
                    controller1.setJoystick(SDL_JoystickOpen(0));
                    controller2.setJoystick(SDL_JoystickOpen(1));
                }
            }
            */


        }

        controller1.getStates(&controllerData[0]);
        controller2.getStates(&controllerData[18]);
    }

    SDL_Quit();
}
