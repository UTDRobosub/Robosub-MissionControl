//controller.cpp

#include <SDL/SDL.h>
#include "controller.h"
using namespace std;

Controller::Controller(){
    //array to hold controller states, fill with 0s
    states = new Uint8[18];
    for(int i=0;i<18;i++){states[i]=0;}
}

Controller::~Controller(){
    delete[] states;
}

//update states array to current controller state
void Controller::updateStates(){
  SDL_JoystickUpdate();

  if(joystick == nullptr)
    return;

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

//return array with controller states
Uint8* Controller::getStates(){
    updateStates();
    return states;
}

string Controller::toString(){
    string s = "";
    s += to_string(states[0]);
    for(int i = 1; i<18; i++){
        s += ' ';
        s += to_string(states[i]);
    }
    return s;
}

void Controller::setJoystick(SDL_Joystick* j){
    joystick = j;
}

