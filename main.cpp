#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace cv;
using namespace std;

const string ORIGINAL_WINDOW_NAME = "Original video";
const string FOREGROUND_WINDOW_NAME = "Foreground";
const string HEATMAP_WINDOW_NAME = "Heatmap";

int keyboard;

Mat processVideo(VideoCapture& capture, Ptr<BackgroundSubtractor> bgSubtractor) {
	Mat frame;
	Mat fgMask;
	Mat acumulator;
	Mat heatmap;
	unsigned maxValue = 0;
	bool first = true;
	
	bool printed = false;
	while( (char)keyboard != 'q' && (char)keyboard != 27 && capture.read(frame)){
		bgSubtractor->apply(frame, fgMask);
		if(first){
			acumulator = Mat::zeros(fgMask.rows, fgMask.cols, CV_32SC1);
			first = false;
		}
		
		for(int row=0; row < acumulator.rows; ++row){
			uchar *f = fgMask.ptr(row);
			unsigned *a = (unsigned*)acumulator.ptr(row);
			for(int col = 0; col < acumulator.cols; ++col){
				a[col] += f[col];
				maxValue = max(maxValue, a[col]);
			}
		}
		
		acumulator.convertTo(heatmap, CV_32FC1, 1.0/(maxValue));
		
		imshow(ORIGINAL_WINDOW_NAME, frame);
		imshow(FOREGROUND_WINDOW_NAME, fgMask);
		imshow(HEATMAP_WINDOW_NAME, heatmap);
		
		keyboard = waitKey(0);
	}
	return heatmap;
}

Mat processVideoSilently(VideoCapture& capture, Ptr<BackgroundSubtractor> bgSubtractor){
	Mat frame;
	Mat fgMask;
	Mat acumulator;
	Mat heatmap;
	unsigned maxValue = 0;

	capture.read(frame);
	bgSubtractor->apply(frame, fgMask);
	acumulator = Mat::zeros(fgMask.rows, fgMask.cols, CV_32SC1);
	
	do{
		bgSubtractor->apply(frame, fgMask);
		
		for(int row=0; row < acumulator.rows; ++row){
			uchar *f = fgMask.ptr(row);
			unsigned *a = (unsigned*)acumulator.ptr(row);
			for(int col = 0; col < acumulator.cols; ++col){
				a[col] += f[col];
				maxValue = max(maxValue, a[col]);
			}
		}
		
	}while( (char)keyboard != 'q' && (char)keyboard != 27 && capture.read(frame));
	
	acumulator.convertTo(heatmap, CV_32FC1, 1.0/maxValue);
	return heatmap;
}

Mat applyColor(Mat m){
	uchar r, g, b;
	Mat n;
	
	cvtColor(m,n,CV_GRAY2RGB);
	int x = 2;
	for(auto it = n.begin<Vec3f>(), end = n.end<Vec3f>(); it != end; ++it){
		Vec3f& pixel = *it;
		pixel[2] = min(pixel[0]*2, 1.0f);
		pixel[1] = max(0.0f, pixel[1]*x - x*0.5f);
		pixel[0] = 0;
	}
	return n;
}

int main(int argc, char* argv[]){
	if(argc != 2) {
		cerr <<"Usage: ./program videofile" << endl;
		return EXIT_FAILURE;
	}
   
	namedWindow(ORIGINAL_WINDOW_NAME);
	namedWindow(FOREGROUND_WINDOW_NAME);
	namedWindow(HEATMAP_WINDOW_NAME);
   

	VideoCapture capture(argv[1]);
	if(!capture.isOpened()){
		printf("Unable to open video file: %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	//Mat fromvideo = processVideo(capture, createBackgroundSubtractorMOG2()); 
	//capture.release();
	//Mat colorful = applyColor(fromvideo);
	//imshow(HEATMAP_WINDOW_NAME, colorful);
	
	capture = VideoCapture(argv[1]);
	Mat fromsilent = processVideoSilently(capture, createBackgroundSubtractorMOG2());
	Mat colorful = applyColor(fromsilent);
	imshow(HEATMAP_WINDOW_NAME, colorful);
	
	
	keyboard = waitKey(0);
	destroyAllWindows();
	return EXIT_SUCCESS;
}