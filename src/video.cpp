
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


<<<<<<< HEAD
void video(){

    const int border = 0;
    int port[4] = {8001, 8002, 8003, 8004};
    signal(SIGINT, catchSignal);
    int receiveTimeoutMicroseconds = 50000;
    UDPR udpr[4];
    Mat latestframe[4];
    Mat display = Mat(2*480 + 3*border, 2*640 + 3*border, CV_8UC3, Scalar(0,0,0));
    NetworkVideoFrameReceiver* framerecv[4];
    for(int i=0; i<4; i++){
    	cout<<"initRecv err "<<udpr[i].initRecv(port[i], receiveTimeoutMicroseconds)<<endl;
        framerecv[i] = new NetworkVideoFrameReceiver(udpr[i]);
        latestframe[i] = Mat(10,10, CV_8UC3, Scalar(0,0,0));
    }


    FPS fps = FPS();

    while(running){
        for(int i=0; i<2; i++){
	    framerecv[i]->updateReceiveFrame();
	    Mat* f = framerecv[i]->getLatestFrame();
            if(f)
            	latestframe[i] = *f;
	}



        latestframe[0].copyTo(display(cv::Rect(border,border,latestframe[0].cols, latestframe[0].rows)));
        latestframe[1].copyTo(display(cv::Rect(640+2*border,border,latestframe[1].cols, latestframe[1].rows)));
        latestframe[2].copyTo(display(cv::Rect(border,480+2*border,latestframe[2].cols, latestframe[2].rows)));
        latestframe[3].copyTo(display(cv::Rect(640+2*border,480+2*border,latestframe[3].cols, latestframe[3].rows)));



	imshow("Video Feeds", display);
	
	//imshow("Video Feeds", latestframe[0]);



	fps.frame();
	cout << fps.fps() << endl;

        waitKey(1);
    }
    delete(framerecv[0]);
    return;
=======
void video(int port){
	
	while(running);
	return;
>>>>>>> 904a546fc7490782594ae5e0c3e1399ef7f8a3c5
}
