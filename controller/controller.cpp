
//requires SDL 1.2
//if using a logitech controller, switch to Xinput mode


#define POLLING_INTERVAL 5
//in ms


#include <iostream>
#include <SDL/SDL.h>
using namespace std;


void updateStates(SDL_Joystick* joystick, int* states)
{
    SDL_JoystickUpdate();
    
    if(SDL_JoystickNumAxes(joystick) == 5) //windows
    {
        states[0] = SDL_JoystickGetAxis(joystick, 0);               //LS left-right
        states[1] = SDL_JoystickGetAxis(joystick, 1);               //LS up-down
        states[2] = SDL_JoystickGetAxis(joystick, 2);               //RS left-right
        states[3] = SDL_JoystickGetAxis(joystick, 3);               //RS up-down 
        states[4] = SDL_JoystickGetAxis(joystick, 4);               //Trigger Axis

        states[5] = SDL_JoystickGetHat(joystick, 0);                //HAT

        states[6] = SDL_JoystickGetButton(joystick, 0);             //A
        states[7] = SDL_JoystickGetButton(joystick, 1);             //B
        states[8] = SDL_JoystickGetButton(joystick, 2);             //X
        states[9] = SDL_JoystickGetButton(joystick, 3);             //Y
        states[10] = SDL_JoystickGetButton(joystick, 4);            //L1
        states[11] = SDL_JoystickGetButton(joystick, 5);            //R1
        states[12] = SDL_JoystickGetButton(joystick, 6);            //Back
        states[13] = SDL_JoystickGetButton(joystick, 7);            //Start
        states[14] = SDL_JoystickGetButton(joystick, 8);            //L3
        states[15] = SDL_JoystickGetButton(joystick, 9);            //R3

    }

    else if(SDL_JoystickNumAxes(joystick) == 6)  //linux
    {

        states[0] = SDL_JoystickGetAxis(joystick, 0);               //LS left-right
        states[1] = SDL_JoystickGetAxis(joystick, 1);               //LS up-down
        states[2] = SDL_JoystickGetAxis(joystick, 3);               //RS left-right 
        states[3] = SDL_JoystickGetAxis(joystick, 4);               //RS up-down 

        states[4] = (SDL_JoystickGetAxis(joystick, 2) -             //Combine triggers into 1 axis
                    SDL_JoystickGetAxis(joystick,5))/2;              //to be same as windows

        states[5] = SDL_JoystickGetHat(joystick, 0);                //HAT

        states[6] = SDL_JoystickGetButton(joystick, 0);             //A
        states[7] = SDL_JoystickGetButton(joystick, 1);             //B
        states[8] = SDL_JoystickGetButton(joystick, 2);             //X
        states[9] = SDL_JoystickGetButton(joystick, 3);             //Y
        states[10] = SDL_JoystickGetButton(joystick, 4);            //L1
        states[11] = SDL_JoystickGetButton(joystick, 5);            //R1
        states[12] = SDL_JoystickGetButton(joystick, 6);            //Back
        states[13] = SDL_JoystickGetButton(joystick, 7);            //Start

        //states[] = SDL_JoystickGetButton(joystick, 8);            //Center, windows doesn't detect, so we'll skip

        states[14] = SDL_JoystickGetButton(joystick, 9);            //L3
        states[15] = SDL_JoystickGetButton(joystick, 10);           //R3
    }   

}

void printToConsole(int* states)
{
    for(int i = 0; i < 16; i++)
    {
        cout << states[i] << ' ';
    }
    cout << endl;    
}


void loadImages(SDL_Surface** images, SDL_Rect** offsets)
{
    images[0] = SDL_LoadBMP("gui/controller.bmp");
    images[1] = SDL_LoadBMP("gui/button1.bmp");
    images[2] = SDL_LoadBMP("gui/button2.bmp");
    images[3] = SDL_LoadBMP("gui/button3.bmp");
    
    for(int i = 0; i<18; i++){
        offsets[i] = new SDL_Rect;
    }
    offsets[6]->x = 587; offsets[6]->y = 179; 
    offsets[7]->x = 640; offsets[7]->y = 126; 
    offsets[8]->x = 534; offsets[8]->y = 127; 
    offsets[9]->x = 586; offsets[9]->y = 74; 
    offsets[10]->x = 120; offsets[10]->y = 28; 
    offsets[11]->x = 561; offsets[11]->y = 27;

}

void updateGUI(SDL_Surface* screen, SDL_Surface** images, SDL_Rect** offsets, int* states)
{
    SDL_BlitSurface(images[0],NULL,screen,NULL);


    if(states[6])
        SDL_BlitSurface(images[1],NULL,screen,offsets[6]);
    if(states[7])
        SDL_BlitSurface(images[1],NULL,screen,offsets[7]);
    if(states[8])
        SDL_BlitSurface(images[1],NULL,screen,offsets[8]);
    if(states[9])
        SDL_BlitSurface(images[1],NULL,screen,offsets[9]);
    if(states[10])
        SDL_BlitSurface(images[2],NULL,screen,offsets[10]);
    if(states[11])
        SDL_BlitSurface(images[3],NULL,screen,offsets[11]);

    
    if(states[15])
        SDL_BlitSurface(images[1],NULL,screen,offsets[15]);


    SDL_Flip(screen);
}








int main(int argc, char*  argv[])
{

    if(SDL_Init ( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
    {
        cout << "Couldn't initialize SDL: " << SDL_GetError() << endl;
        exit(1);
    }
    
    if(SDL_NumJoysticks() == 0)
    {
        cout << "No joysticks found\n";
        exit(1);
    }

    SDL_Surface* screen = SDL_SetVideoMode(771,501,0,0);
    SDL_Surface** images = new SDL_Surface*[8];
    SDL_Rect** offsets = new SDL_Rect*[18];
    loadImages(images,offsets);    

    SDL_Joystick* joystick = SDL_JoystickOpen(0);
    Uint8* keystates = SDL_GetKeyState(NULL);
    int* states = new int[18];


    if(SDL_JoystickNumAxes(joystick) == 5){
        freopen( "CON", "wt", stdout );
        freopen( "CON", "wt", stderr );
    }


/*
    if(SDL_JoystickNumAxes(joystick) != 6){
        cout << "Try again with gamepad in Xinput mode\n";
        exit(1);
    }
*/
    cout << SDL_JoystickName(0) << " initialized." << endl;
    cout << SDL_JoystickNumAxes(joystick) << " axes found" << endl;
    cout << SDL_JoystickNumHats(joystick) << " hats found" << endl;
    cout << SDL_JoystickNumButtons(joystick) << " buttons found" << endl;




    Uint32 prevTime = SDL_GetTicks();    
    while(keystates[SDLK_ESCAPE] == 0)
    {
        SDL_PumpEvents();
        while(SDL_GetTicks() - prevTime < POLLING_INTERVAL)
            SDL_Delay(1);
        prevTime = SDL_GetTicks();

        updateStates(joystick, states);
        updateGUI(screen,images,offsets,states);

        printToConsole(states);
        
        ////!!! send packet
    }

    delete[] images;
    delete[] states;

    return 0;
}
