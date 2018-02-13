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

//thread for receiving and displaying video feeds
void video(int port){
    int mode = 2;

	//catch signal
	signal(SIGINT, catchSignal);

    const int cols = 1280;
	const int rows = 720;

    Size screenRes;
    //Camera cam(0);
        screenRes = Util::getDesktopResolution();

    const int vals = 1;
    const int size = sizeof(uchar)*3;
    const int len=rows*cols*vals*size;
    char raw2[len];
    char raw2final[len];
    int len2;
    int len2total;
    int raw2start;

    UDPR udpr;
    udpr.initRecv(port);

    //Size s(cols,rows);

    //VideoWriter writer;
    //writer.open("out.avi",VideoWriter::fourcc('X','2','6','4'),30,s);
        
    FPS fps = FPS();
    int droppedFrames = 0;

	while (running){


        udpr.recv(len*2-len2total,len2,raw2+len2total);

        len2total+=len2;

        if(len2total==len*2){
            len2total=0;
            cout << "overflow" << endl;
        }
        else{
            for(int i =len2total-len2; i<len2total;i++){
                if(raw2[i] & 0x01){
                    raw2start=i;
                    cout << "start " << i << endl;
                }
                if(raw2[i] & 0x02){
                    if(raw2start != -1 && i-raw2start == len - 1){
														//complete frame received

                            memcpy(raw2final,raw2+raw2start,i-raw2start);
                            // memcpy(raw2,raw2+i+1,len2total-i-1);
                            len2total=len2total-i-1;

                            raw2start=-1;
                            cout<<"end "<<i<<endl;
														fps.frame();
                        }else{
														//drop frame
                            len2total=0;
														droppedFrames++;
                        }
                    }
                }


         }
        Mat frame2(rows,cols,CV_8UC3,raw2final);

						Drawing::text(frame2,
                String(Util::toStringWithPrecision(fps.fps())) + String(" FPS"),
                Point(16, 16), Scalar(255, 255, 255), Drawing::Anchor::BOTTOM_LEFT, 0.5
            );
						Drawing::text(frame2,
                String(Util::toStringWithPrecision(droppedFrames, 2)) + String(" Dropped Frames"),
                Point(16, 60), Scalar(255, 255, 255), Drawing::Anchor::BOTTOM_LEFT, 0.5
            );

						//ImageTransform::scale(frame2, Size(cols,rows));

            imshow("Receiving Frame", frame2);

        //writer.write(frame2);

        waitKey(1);
		//if (waitKey(1) >= 0) break;
	}
}
