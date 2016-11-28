#ifndef COLOR_H
#define COLOR_H

#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

enum scaleTypes{HOT,JET};

Mat applyColor(Mat m,int type); 

#endif