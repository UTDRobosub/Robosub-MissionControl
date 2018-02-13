#include <SDL/SDL.h>
#include <iostream>
#include <thread>
#include <librobosub/robosub.h>
#include "controller.h"
#include "main.h"

using namespace std;
using namespace robosub;

void control(string,int);
void video(int);

bool running = true;

int main(int argc, char* argv[]){

    int controllerPort, videoPort;
    string addr;
    
    cout << "Address: ";
    cin >> addr;
    cout << "Video port: ";
    cin >> videoPort;
    cout << "Controller port: ";
    cin >> controllerPort;

    thread controlThread(control,addr,controllerPort);
    thread videoThread(video,videoPort);

    controlThread.join();
    videoThread.join();

    return 0;
}

//Thread for mission control window (everything but video feed)
//Display and send controller inputs,
//Recieve and display diagnostics
void control(string addr, int port){

    //start udp stuff
    Uint8 packet[36];
    Uint8* temp;
    int udpError;
    UDPS udps;
    udpError=udps.initSend(port,addr);
    cout<<"initSend err: "<<udpError<<"\n";

    //start SDL
    if(SDL_Init (SDL_INIT_JOYSTICK | SDL_INIT_VIDEO) < 0 ){
        cout << "Couldn't initialize SDL: " << SDL_GetError() << endl;
        exit(1);
    }

    Uint8 *keystate = SDL_GetKeyState(NULL);

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

        //update joystick states and send via udp
        temp = controller1.getStates();
        for(int i=0; i<18; i++)
            packet[i] = temp [i];
        temp = controller2.getStates();
        for(int i=0; i<18; i++)
            packet[i+18] = temp[i];
        udpError=udps.send(18,(char*)packet);
        if(udpError)
            cout<<"send err: "<<udpError<<"\n";

        cout << controller1.toString() << endl;
    }

    SDL_Quit();
}
