#include "robotState.h"



Mat RobotState::motorValues(double tx, double ty, double r){


    double _b[1][4] = { { tx, ty, r, 0 } };
    Mat b = Mat(1, 4, CV_64FC1, &_b);
    transpose(b, b);


    //solve system of equations
    Mat x = Minv * b;

    //normalize solution
    double minVal, maxVal;
    cv::minMaxIdx(abs(x), &minVal, &maxVal);
    if (maxVal > 0) {
        x /= maxVal;
    }
    double bNorm = sqrt(tx*tx + ty*ty + r*r);
    x *= bNorm;

    return x;
}
