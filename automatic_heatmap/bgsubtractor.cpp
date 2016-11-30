#include "bgsubtractor.h"
#include <time.h>
#include <iostream>

Mat openStructuringElement = getStructuringElement(MORPH_RECT, Size(2,2));
Mat closeStructuringElement = getStructuringElement(MORPH_RECT, Size(5,7));

Mat processVideoSilently(VideoCapture *capture, Ptr<BackgroundSubtractor> bgSubtractor){
	Mat frame;
	Mat fgMask;
	Mat binary;
	Mat accumulator;
	Mat heatmap;
	unsigned maxValue = 0;
	int i = 0;
	
	capture->read(frame);
	accumulator = Mat::zeros(frame.size(), CV_32SC1);
	
	do{
		bgSubtractor->apply(frame, fgMask);
		threshold(fgMask, binary, 128, 255, THRESH_BINARY);
		morphologyEx(binary, binary, MORPH_OPEN, openStructuringElement);
		morphologyEx(binary, binary, MORPH_CLOSE, closeStructuringElement);
		for(int row=0; row < accumulator.rows; ++row){
			uchar *f = binary.ptr(row);
			unsigned *a = (unsigned*)accumulator.ptr(row);
		for(int col = 0; col < accumulator.cols; ++col){
			a[col] += f[col];
			maxValue = max(maxValue, a[col]);
		}
	}
	printf("\r%d", ++i);
	fflush(stdout);
	printf("\r	  ");
	} while(capture->grab() && capture->retrieve(frame));
	
	accumulator.convertTo(heatmap, CV_32FC1, 1.0/maxValue);
	return heatmap;

}

Mat processVideo(VideoCapture *capture, Ptr<BackgroundSubtractor> bgSubtractor, string originalName, string foregroundName, string heatmapName) {
	Mat frame;
	Mat fgMask;
	Mat binary;
	Mat accumulator;
	Mat heatmap;
	unsigned maxValue = 0;
	int keyboard;
	clock_t start, end;
	double elapsed;
	
	capture->read(frame);
	accumulator = Mat::zeros(frame.size(), CV_32SC1);
	
	do{
		start = clock();
		bgSubtractor->apply(frame, fgMask);
		threshold(fgMask, binary, 128, 255, THRESH_BINARY);
		morphologyEx(binary, binary, MORPH_CLOSE, closeStructuringElement);
		morphologyEx(binary, binary, MORPH_OPEN, openStructuringElement);
		
		for(int row=0; row < accumulator.rows; ++row){
			uchar *f = binary.ptr(row);
			unsigned *a = (unsigned*)accumulator.ptr(row);
			for(int col = 0; col < accumulator.cols; ++col){
				a[col] += f[col];
				maxValue = max(maxValue, a[col]);
			}
		}
		
		accumulator.convertTo(heatmap, CV_32FC1, 1.0/(maxValue));
		
		imshow(originalName, frame);
		imshow(foregroundName, binary);
		imshow(heatmapName, heatmap);
		
		end = clock();
		elapsed = double(end - start)/ CLOCKS_PER_SEC;
		keyboard = waitKey(max(1.0,30.0-elapsed));
	}while( (char)keyboard != 'q' && (char)keyboard != 27 && (char)keyboard != 'p' && capture->grab() && capture->retrieve(frame));
	
	if( (char)keyboard == 'q' || (char)keyboard == 27){
		exit(0);
	}
	
	return heatmap;
}
