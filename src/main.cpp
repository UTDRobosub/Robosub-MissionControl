#include <SDL/SDL.h>
#include <iostream>
#include <thread>
#include <librobosub/robosub.h>
#include "controller.h"
using namespace std;
using namespace robosub;

void control();

int main(int argc, char* argv[]){

    thread controlThread(control);

    controlThread.join();

    return 0;
}

//Thread for mission control window (everything but video feed)
//Display and send controller inputs,
//Recieve and display diagnostics
void control(){

    //start udp stuff
    Uint8 packet[36];
    Uint8* temp;
    int udpError;
    int port;
    string addr;
    cout<<"Enter port: ";
    cin>>port;
    cout<<"Enter address: ";
    cin>>addr;
    UDPS udps;
    udpError=udps.initSend(port,addr);
    cout<<"initSend err: "<<udpError<<"\n";

    //start SDL
    if(SDL_Init ( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ){
        cout << "Couldn't initialize SDL: " << SDL_GetError() << endl;
        exit(1);
    }

    //get controllers
    Controller controller1;
    Controller controller2;
    controller1.setJoystick(SDL_JoystickOpen(0));
    controller2.setJoystick(SDL_JoystickOpen(1));

    //make window, important locations in window
    SDL_Surface* window = SDL_SetVideoMode(771,1002,0,0);
    SDL_Rect* topScreen = new SDL_Rect; topScreen->x = 0; topScreen->y = 0;
    SDL_Rect* bottomScreen = new SDL_Rect; bottomScreen->x = 0; bottomScreen->y = 501;

    SDL_Event event;
    bool running = true;
    //main loop
    while(running){

        //event loop
        SDL_PumpEvents();
        while( SDL_PollEvent( &event ) != 0 ) {
            if( event.type == SDL_QUIT )
                running = false;
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

        //update gui
        SDL_BlitSurface(controller1.getScreen(),NULL,window,topScreen);
        SDL_BlitSurface(controller2.getScreen(),NULL,window,bottomScreen);
        SDL_Flip(window);
    }

    SDL_Quit();
}
