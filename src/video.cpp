#include <opencv2/opencv.hpp>
#include <librobosub/robosub.h>
#include <signal.h>
#include <opencv2/ximgproc.hpp>
#include "main.h"

using namespace std;
using namespace robosub;



void catchSignal(int signal) {
	running = false;
}

const int MODE_RECEIVE = 0;
const int MODE_SEND = 1;

void video(int port){
    
    while(running);
	return;
}
