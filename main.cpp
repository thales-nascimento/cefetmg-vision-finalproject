#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "testimage.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace cv;
using namespace std;

const string ORIGINAL_WINDOW_NAME = "Original video";
const string FOREGROUND_WINDOW_NAME = "Foreground";
const string HEATMAP_WINDOW_NAME = "Heatmap";

enum scaleTypes{HOT,JET};

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

Mat applyColor(Mat m,int type){
	Mat n;
	
	cvtColor(m,n,CV_GRAY2RGB);
	for(auto it = n.begin<Vec3f>(), end = n.end<Vec3f>(); it != end; ++it){
		Vec3f& pixel = *it;
		float& r = pixel[2];
		float& g = pixel[1];
		float& b = pixel[0];
		if(type==JET){
			float alpha = 3.94f;
			
			if(b==0) ;	
			else if(b < 0.11f) b = alpha*b + 0.56f;
			else if(b <= 0.37f) b = 1.0f;
			else if(b <= 0.62f) b = -alpha*b + 2.44f;
			else b = 0;
	
			if(g==0) ;	
			else if( g < 0.12f) g = 0.0f;
			else if(g < 0.37f) g = alpha*g -0.47f;
			else if(g <= 0.62f) g = 1.0f;
			else if(g <= 0.87f) g = -alpha*g + 3.43f;
			else g = 0;
	
			if(r==0) ;	
			else if( r < 0.37f) r = 0.0f;
			else if(r < 0.62f) r = alpha*r -1.46f;
			else if(r <= 0.87f) r = 1.0f;
			else r = -alpha*r + 4.43f;
		}
		else if(type==HOT){
			float alpha = 2.5f;
				
			if(r<=0.4f) r=alpha*r;
			else r=1.0f;

			if(g<=0.4f) g=.0f;
			else if(g<=0.8f) g=alpha*g-1.0f;
			else	g=1.0f;
			
			if(b<=0.8f) b=.0f;
			else b=alpha*2.0f*b-4.0f;	

		}
		
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
	
	
	Mat fromsilent = processVideoSilently(capture, createBackgroundSubtractorMOG2());
	imshow("Escala cinza",fromsilent);
	Mat colorful = applyColor(fromsilent,JET);
	imshow(HEATMAP_WINDOW_NAME, colorful);
	//Mat grayscale = horizontalGradient(100,400);
	//imshow(ORIGINAL_WINDOW_NAME, grayscale);
	//imshow(HEATMAP_WINDOW_NAME, applyColor(grayscale,HOT));
	
	keyboard = waitKey(0);
	destroyAllWindows();
	return EXIT_SUCCESS;
}
