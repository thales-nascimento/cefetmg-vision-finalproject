#ifndef BG_SUBTRACTOR_H
#define BG_SUBTRACTOR_H

#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <algorithm>

using namespace cv;
using namespace std;

Mat processVideoSilently(VideoCapture *capture, Ptr<BackgroundSubtractor> bgSubtractor);
Mat processVideo(VideoCapture *capture, Ptr<BackgroundSubtractor> bgSubtractor, string originalName, string foregroundName, string heatmapName);

#endif
 
