//controller.cpp
//reads gamepad input, displays states in gui

//!! Still needs code to send controller states over network

//requires SDL 1.2
//if using a logitech controller, set to Xinput mode

#define POLLING_INTERVAL 5

#include <iostream>
#include <SDL/SDL.h>
using namespace std;


//Read controller states to byte array states[]
void updateStates(SDL_Joystick* joystick, Uint8* states)
{
    SDL_JoystickUpdate();


    if(SDL_JoystickNumAxes(joystick) == 4) //directx, either os
    {

        states[0] = SDL_JoystickGetAxis(joystick, 0)/256 + 128;               //LS left-right
        states[1] = SDL_JoystickGetAxis(joystick, 1)/256 + 128;               //LS up-down
        states[2] = SDL_JoystickGetAxis(joystick, 2)/256 + 128;               //RS left-right
        states[3] = SDL_JoystickGetAxis(joystick, 3)/256 + 128;               //RS up-down

        states[4] = 255 * SDL_JoystickGetButton(joystick, 6);
        states[5] = 255 * SDL_JoystickGetButton(joystick, 7);

        states[6] = SDL_JoystickGetHat(joystick, 0);                //HAT

        states[7] = SDL_JoystickGetButton(joystick, 1);             //A
        states[8] = SDL_JoystickGetButton(joystick, 2);             //B
        states[9] = SDL_JoystickGetButton(joystick, 0);             //X
        states[10] = SDL_JoystickGetButton(joystick, 3);             //Y
        states[11] = SDL_JoystickGetButton(joystick, 4);            //L1
        states[12] = SDL_JoystickGetButton(joystick, 5);            //R1
        states[13] = SDL_JoystickGetButton(joystick, 8);            //Back
        states[14] = SDL_JoystickGetButton(joystick, 9);            //Start
        states[15] = SDL_JoystickGetButton(joystick, 10);            //L3
        states[16] = SDL_JoystickGetButton(joystick, 11);            //R3
        
        states[17] = 0; //input mode
    }

    if(SDL_JoystickNumAxes(joystick) == 5) //windows, xinput
    {
        states[0] = SDL_JoystickGetAxis(joystick, 0)/256 + 128;               //LS left-right
        states[1] = SDL_JoystickGetAxis(joystick, 1)/256 + 128;               //LS up-down
        states[2] = SDL_JoystickGetAxis(joystick, 4)/256 + 128;               //RS left-right
        states[3] = SDL_JoystickGetAxis(joystick, 3)/256 + 128;               //RS up-down

        if(SDL_JoystickGetAxis(joystick, 2) > 0)
        {
            states[4] = SDL_JoystickGetAxis(joystick, 2)/128;
            states[5] = 0;
        }
        else if(SDL_JoystickGetAxis(joystick, 2) < 0)
        {
            states[4] = 0;
            states[5] = -SDL_JoystickGetAxis(joystick, 2)/128;
        }
        else
        {
            states[4] = 0;
            states[5] = 0;
        }

        states[6] = SDL_JoystickGetHat(joystick, 0);                //HAT

        states[7] = SDL_JoystickGetButton(joystick, 0);             //A
        states[8] = SDL_JoystickGetButton(joystick, 1);             //B
        states[9] = SDL_JoystickGetButton(joystick, 2);             //X
        states[10] = SDL_JoystickGetButton(joystick, 3);             //Y
        states[11] = SDL_JoystickGetButton(joystick, 4);            //L1
        states[12] = SDL_JoystickGetButton(joystick, 5);            //R1
        states[13] = SDL_JoystickGetButton(joystick, 6);            //Back
        states[14] = SDL_JoystickGetButton(joystick, 7);            //Start
        states[15] = SDL_JoystickGetButton(joystick, 8);            //L3
        states[16] = SDL_JoystickGetButton(joystick, 9);            //R3

        states[17] = 1; //input mode

    }

    else if(SDL_JoystickNumAxes(joystick) == 6)  //linux, xinput
    {

        states[0] = SDL_JoystickGetAxis(joystick, 0)/256 + 128;               //LS left-right
        states[1] = SDL_JoystickGetAxis(joystick, 1)/256 + 128;               //LS up-down
        states[2] = SDL_JoystickGetAxis(joystick, 3)/256 + 128;               //RS left-right
        states[3] = SDL_JoystickGetAxis(joystick, 4)/256 + 128;               //RS up-down

        if(SDL_JoystickGetAxis(joystick, 2) - SDL_JoystickGetAxis(joystick, 5) > 0)
        {
            states[4] = (Uint8)((SDL_JoystickGetAxis(joystick, 2)-SDL_JoystickGetAxis(joystick,5))/256 + 256);
            states[5] = 0;
        }
        else if(SDL_JoystickGetAxis(joystick, 2) - SDL_JoystickGetAxis(joystick, 5) < 0)
        {
            states[4] = 0;
            states[5] = (Uint8)((SDL_JoystickGetAxis(joystick, 5)-SDL_JoystickGetAxis(joystick,2))/256 + 256);
        }
        else
        {
            states[4] = 0;
            states[5] = 0;
        }


        states[6] = SDL_JoystickGetHat(joystick, 0);                //HAT

        states[7] = SDL_JoystickGetButton(joystick, 0);             //A
        states[8] = SDL_JoystickGetButton(joystick, 1);             //B
        states[9] = SDL_JoystickGetButton(joystick, 2);             //X
        states[10] = SDL_JoystickGetButton(joystick, 3);            //Y
        states[11] = SDL_JoystickGetButton(joystick, 4);            //L1
        states[12] = SDL_JoystickGetButton(joystick, 5);            //R1
        states[13] = SDL_JoystickGetButton(joystick, 6);            //Back
        states[14] = SDL_JoystickGetButton(joystick, 7);            //Start
        states[15] = SDL_JoystickGetButton(joystick, 9);            //L3
        states[16] = SDL_JoystickGetButton(joystick, 10);           //R3

        states[17] = 2; //input mode
    }

}

//Output controller states to console
void printToConsole(Uint8* states)
{
    for(int i = 0; i < 18; i++)
    {
        cout << (int)states[i] << ' ';
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
void updateGUI(SDL_Surface* screen, SDL_Surface** images, SDL_Rect** offsets, Uint8* states)
{

    //background
    SDL_BlitSurface(images[0],NULL,screen,NULL);

    //buttons
    if(states[7])
        SDL_BlitSurface(images[1],NULL,screen,offsets[6]);                  //A
    if(states[8])
        SDL_BlitSurface(images[1],NULL,screen,offsets[7]);                  //B
    if(states[9])
        SDL_BlitSurface(images[1],NULL,screen,offsets[8]);                  //X
    if(states[10])
        SDL_BlitSurface(images[1],NULL,screen,offsets[9]);                  //Y
    if(states[11])
        SDL_BlitSurface(images[2],NULL,screen,offsets[10]);                 //L1
    if(states[12])
        SDL_BlitSurface(images[3],NULL,screen,offsets[11]);                 //R1
    if(states[13])
        SDL_BlitSurface(images[4],NULL,screen,offsets[12]);                 //Select
    if(states[14])
        SDL_BlitSurface(images[4],NULL,screen,offsets[13]);                 //Start
    if(states[15])
        SDL_BlitSurface(images[5],NULL,screen,offsets[14]);                 //L3
    if(states[16])
        SDL_BlitSurface(images[5],NULL,screen,offsets[15]);                 //R3


    //D-Pad
    if(states[6] == 1 || states[6] == 9 || states[6] == 3)
        SDL_BlitSurface(images[6],NULL,screen,offsets[16]);                 //D-Pad Up
    if(states[6] == 2 || states[6] == 3 || states[6] == 6)
        SDL_BlitSurface(images[7],NULL,screen,offsets[17]);                 //D-Pad Right
    if(states[6] == 4 || states[6] == 6 || states[6] == 12)
        SDL_BlitSurface(images[8],NULL,screen,offsets[18]);                 //D-Pad Down
    if(states[6] == 8 || states[6] == 12 || states[6] == 9)
        SDL_BlitSurface(images[9],NULL,screen,offsets[19]);                 //D-Pad Left



    //Left Analog Stick
    offsets[0]->x += (((int)states[0])-128) * 40 / 128;
    offsets[0]->y += (((int)states[1])-128) * 40 / 128;
    SDL_BlitSurface(images[10],NULL,screen,offsets[0]);
    offsets[0]->x -= (((int)states[0])-128) * 40 / 128;
    offsets[0]->y -= (((int)states[1])-128) * 40 / 128;

    //Right Analog Stick
    offsets[1]->x += (((int)states[2])-128) * 40 / 128;
    offsets[1]->y += (((int)states[3])-128) * 40 / 128;
    SDL_BlitSurface(images[10],NULL,screen,offsets[1]);
    offsets[1]->x -= (((int)states[2])-128) * 40 / 128;
    offsets[1]->y -= (((int)states[3])-128) * 40 / 128;

    //L2
    offsets[2]->x -= ((int)states[4]) * 100 / 256;
    SDL_BlitSurface(images[11],NULL,screen,offsets[2]);
    offsets[2]->x += ((int)states[4]) * 100 / 256;

    //R2
    offsets[2]->x += ((int)states[5]) * 100 / 256;
    SDL_BlitSurface(images[11],NULL,screen,offsets[2]);
    offsets[2]->x -= ((int)states[5]) * 100 / 256;

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
    Uint8* states = new Uint8[18];


    //if windows, direct output to console
    if(SDL_JoystickNumAxes(joystick) == 5)
        freopen( "CON", "wt", stdout );


    //catch unexpected controller layouts
    //if(SDL_JoystickNumAxes(joystick) != 6 && SDL_JoystickNumAxes(joystick) != 5){
    //    cout << "Unrecognized controller type. Make sure you are in xinput mode\n";
    //    exit(1);
    //}


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
