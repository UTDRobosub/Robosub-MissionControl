#include <SDL/SDL.h>
#include <iostream>
#include <thread>
#include <librobosub/robosub.h>
#include "main.h"

using namespace std;
//using namespace robosub;

void control();
void video(int);
void network();

bool running = true;
bool refresh = false;
Controller* controller1;
Controller* controller2;
long controllerTime;

int main(int argc, char* argv[]){

    controller1 = new Controller;
    controller2 = new Controller;

    int videoPort;
    //cout << "Video port: ";
    //cin >> videoPort;
    videoPort = 12345;


    thread controlThread(control);
    //thread videoThread(video,videoPort);
    thread networkThread(network);

    controlThread.join();
    //videoThread.join();
    networkThread.join();

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
    controller1->setJoystick(SDL_JoystickOpen(0));
    controller2->setJoystick(SDL_JoystickOpen(1));


    SDL_Event event;
    //main loop
    while(running){

        //event loop
        SDL_PumpEvents();
        while( SDL_PollEvent( &event ) != 0 ) {
            if( event.type == SDL_QUIT )
                running = false;
        }

        //refresh when requested or both joysticks are disconnected
        if(refresh || (controller1->mode() == 0) && (controller2->mode() == 0)) {
            refresh = false;
            SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
            SDL_InitSubSystem(SDL_INIT_JOYSTICK);
            controller1->setJoystick(SDL_JoystickOpen(0));
            controller2->setJoystick(SDL_JoystickOpen(1));
        }


        controllerTime = robosub::Time::millis(); //add current timestamp
        robosub::Time::waitMillis(1); //prevent pinning the processor at 100%
    }

    SDL_Quit();
}
