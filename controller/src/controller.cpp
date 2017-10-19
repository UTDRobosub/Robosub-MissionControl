//controller.cpp
//reads gamepad input, displays states in gui

//!! Still needs code to send controller states over network
      
//requires SDL 1.2
//if using a logitech controller, set to Xinput mode

#define POLLING_INTERVAL 5

#include <iostream>
#include <SDL/SDL.h>
using namespace std;


//Read controller states to int array states[]
void updateStates(SDL_Joystick* joystick, int* states)
{
    SDL_JoystickUpdate();

    if(SDL_JoystickNumAxes(joystick) == 5) //windows
    {
        states[0] = SDL_JoystickGetAxis(joystick, 0);               //LS left-right
        states[1] = SDL_JoystickGetAxis(joystick, 1);               //LS up-down
        states[2] = SDL_JoystickGetAxis(joystick, 4);               //RS left-right
        states[3] = SDL_JoystickGetAxis(joystick, 3);               //RS up-down
        states[4] = SDL_JoystickGetAxis(joystick, 2);               //Trigger Axis

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
                    SDL_JoystickGetAxis(joystick,5))/2;             //to be consistent with windows

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

//Output controller states to console
void printToConsole(int* states)
{
    for(int i = 0; i < 16; i++)
    {
        cout << states[i] << ' ';
    }
    cout << endl;
}


//Loop until POLLING_INTERVAL ms have passed since its previous call
void delay()
{
    static Uint32 prevTime = 0;
    while(SDL_GetTicks() - prevTime < POLLING_INTERVAL)
        SDL_Delay(1);
    prevTime = SDL_GetTicks();
}


//Load bitmap files to array of surfaces
SDL_Surface** loadImages()
{
    SDL_Surface** images = new SDL_Surface*[12];
    
    images[0] = SDL_LoadBMP("../images/controller.bmp");                          //Background + Controller
    images[1] = SDL_LoadBMP("../images/button1.bmp");                             //A,B,X,Y
    images[2] = SDL_LoadBMP("../images/button2.bmp");                             //L1
    images[3] = SDL_LoadBMP("../images/button3.bmp");                             //R1
    images[4] = SDL_LoadBMP("../images/button4.bmp");                             //Start,Select
    images[5] = SDL_LoadBMP("../images/button5.bmp");                             //L3,R3
    images[6] = SDL_LoadBMP("../images/up.bmp");                                  //D-Pad Up
    images[7] = SDL_LoadBMP("../images/right.bmp");                               //D-Pad Right
    images[8] = SDL_LoadBMP("../images/down.bmp");                                //D-Pad Down
    images[9] = SDL_LoadBMP("../images/left.bmp");                                //D-Pad Left
    images[10] = SDL_LoadBMP("../images/dot.bmp");                                //Analog Stick Dot
    images[11] = SDL_LoadBMP("../images/bar.bmp");                                //L2-R2 Axis Bar

    return images;
}


//create array of rectangles that hold xy pairs where
//bitmaps should be blitted on screen
SDL_Rect** loadOffsets()
{ 
    SDL_Rect** offsets = new SDL_Rect*[20]; 
    for(int i = 0; i<20; i++){
        offsets[i] = new SDL_Rect;
    }

    offsets[0]->x = 266; offsets[0]->y = 245;                               //left analog stick point
    offsets[1]->x = 497; offsets[1]->y = 244;                               //right analog stick point
    offsets[2]->x = 383; offsets[2]->y = 21;                                //L2-R2 axis bar

    //offsets 3 through 5 empty,
    //started buttons at 6 to match index of states[]

    offsets[6]->x = 587; offsets[6]->y = 179;                               //A button
    offsets[7]->x = 640; offsets[7]->y = 126;                               //B
    offsets[8]->x = 534; offsets[8]->y = 127;                               //X
    offsets[9]->x = 586; offsets[9]->y = 74;                                //Y
    offsets[10]->x = 120; offsets[10]->y = 28;                              //L1
    offsets[11]->x = 561; offsets[11]->y = 27;                              //L2
    offsets[12]->x = 293; offsets[12]->y = 125;                             //Select
    offsets[13]->x = 436; offsets[13]->y = 126;                             //Start
    offsets[14]->x = 226; offsets[14]->y = 203;                             //L3
    offsets[15]->x = 457; offsets[15]->y = 202;                             //R3

    offsets[16]->x = 142; offsets[16]->y = 94;                              //D-Pad Up
    offsets[17]->x = 170; offsets[17]->y = 134;                             //D-Pad Right
    offsets[18]->x = 142; offsets[18]->y = 163;                             //D-Pad Down
    offsets[19]->x = 101; offsets[19]->y = 136;                             //D-Pad Left

    return offsets;
}


//Blit images to screen at proper offset, based on controller states
void updateGUI(SDL_Surface* screen, SDL_Surface** images, SDL_Rect** offsets, int* states)
{

    //background
    SDL_BlitSurface(images[0],NULL,screen,NULL);

    //buttons
    if(states[6])
        SDL_BlitSurface(images[1],NULL,screen,offsets[6]);                  //A
    if(states[7])
        SDL_BlitSurface(images[1],NULL,screen,offsets[7]);                  //B
    if(states[8])
        SDL_BlitSurface(images[1],NULL,screen,offsets[8]);                  //X
    if(states[9])
        SDL_BlitSurface(images[1],NULL,screen,offsets[9]);                  //Y
    if(states[10])
        SDL_BlitSurface(images[2],NULL,screen,offsets[10]);                 //L1
    if(states[11])
        SDL_BlitSurface(images[3],NULL,screen,offsets[11]);                 //R1
    if(states[12])
        SDL_BlitSurface(images[4],NULL,screen,offsets[12]);                 //Select
    if(states[13])
        SDL_BlitSurface(images[4],NULL,screen,offsets[13]);                 //Start
    if(states[14])
        SDL_BlitSurface(images[5],NULL,screen,offsets[14]);                 //L3
    if(states[15])
        SDL_BlitSurface(images[5],NULL,screen,offsets[15]);                 //R3


    //D-Pad
    if(states[5] == 1 || states[5] == 9 || states[5] == 3)
        SDL_BlitSurface(images[6],NULL,screen,offsets[16]);                 //D-Pad Up
    if(states[5] == 2 || states[5] == 3 || states[5] == 6)
        SDL_BlitSurface(images[7],NULL,screen,offsets[17]);                 //D-Pad Right
    if(states[5] == 4 || states[5] == 6 || states[5] == 12)
        SDL_BlitSurface(images[8],NULL,screen,offsets[18]);                 //D-Pad Down
    if(states[5] == 8 || states[5] == 12 || states[5] == 9)
        SDL_BlitSurface(images[9],NULL,screen,offsets[19]);                 //D-Pad Left



    //Left Analog Stick
    offsets[0]->x += states[0] * 40 / 32767;
    offsets[0]->y += states[1] * 40 / 32767;
    SDL_BlitSurface(images[10],NULL,screen,offsets[0]);
    offsets[0]->x -= states[0] * 40 / 32767;
    offsets[0]->y -= states[1] * 40 / 32767;

    //Right Analog Stick
    offsets[1]->x += states[2] * 40 / 32767;
    offsets[1]->y += states[3] * 40 / 32767;
    SDL_BlitSurface(images[10],NULL,screen,offsets[1]);
    offsets[1]->x -= states[2] * 40 / 32767;
    offsets[1]->y -= states[3] * 40 / 32767;

    //L2-R2 Axis
    offsets[2]->x -= states[4] * 100 / 32767;
    SDL_BlitSurface(images[11],NULL,screen,offsets[2]);
    offsets[2]->x += states[4] * 100 / 32767;

    SDL_Flip(screen);
}






int main(int argc, char*  argv[])
{

    //start SDL
    if(SDL_Init ( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
    {
        cout << "Couldn't initialize SDL: " << SDL_GetError() << endl;
        exit(1);
    }

    //open first connected controller found
    if(SDL_NumJoysticks() == 0)
    {
        cout << "No joysticks found\n";
        exit(1);
    }
    SDL_Joystick* joystick = SDL_JoystickOpen(0);


    //load bitmaps and their offset data
    SDL_Surface** images = loadImages();
    SDL_Rect** offsets = loadOffsets();

    //arrays to hold keyboard and controller states, respectively
    Uint8* keystates = SDL_GetKeyState(NULL);
    int* states = new int[16];


    //if windows, direct output to console
    if(SDL_JoystickNumAxes(joystick) == 5)
        freopen( "CON", "wt", stdout );
    

    //catch unexpected controller layouts
    if(SDL_JoystickNumAxes(joystick) != 6 && SDL_JoystickNumAxes(joystick) != 5){
        cout << "Unrecognized controller type. Make sure you are in xinput mode\n";
        exit(1);
    }


    //print controller info
    cout << SDL_JoystickName(0) << " initialized." << endl;
    cout << SDL_JoystickNumAxes(joystick) << " axes found" << endl;
    cout << SDL_JoystickNumHats(joystick) << " hats found" << endl;
    cout << SDL_JoystickNumButtons(joystick) << " buttons found" << endl;


    //create GUI window 
    SDL_Surface* screen = SDL_SetVideoMode(771,501,0,0);


    //MAIN LOOP
    while(keystates[SDLK_ESCAPE] == 0) //Press Esc to exit
    {
        //limit to 1 loop per POLLING_INTERVAL
        delay();

        //update controller state array
        updateStates(joystick, states);

        //update keyboard state array
        SDL_PumpEvents(); 

        //output
        updateGUI(screen,images,offsets,states);
        printToConsole(states);


        //~~~~~~~~~~~~~~~~~~~~~~~~~~
    
        //  send data in states[]
     
        //~~~~~~~~~~~~~~~~~~~~~~~~~~


    }

    return 0;
}
