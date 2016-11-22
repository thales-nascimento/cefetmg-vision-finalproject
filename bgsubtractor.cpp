#include "bgsubtractor.h"
#include <time.h>

Mat processVideoSilently(VideoCapture *capture, Ptr<BackgroundSubtractor> bgSubtractor){
	Mat frame;
	Mat fgMask;
	Mat acumulator;
	Mat heatmap;
	unsigned maxValue = 0;
	int i = 0;

	capture->read(frame);
	acumulator = Mat::zeros(frame.rows, frame.cols, CV_32SC1);
	
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
		printf("\r%d", ++i);
		fflush(stdout);
		printf("\r      ");
	}while(capture->read(frame));
	
	acumulator.convertTo(heatmap, CV_32FC1, 1.0/maxValue);
	return heatmap;

}

Mat processVideo(VideoCapture *capture, Ptr<BackgroundSubtractor> bgSubtractor, string originalName, string foregroundName, string heatmapName) {
	Mat frame;
	Mat fgMask;
	Mat acumulator;
	Mat heatmap;
	unsigned maxValue = 0;
	int keyboard;
	bool first = true;
	clock_t start, end;
	double elapsed;
	
	bool printed = false;
	while( (char)keyboard != 'q' && (char)keyboard != 27 && (char)keyboard != 'p' && capture->read(frame)){
		start = clock();
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
		
		imshow(originalName, frame);
		imshow(foregroundName, fgMask);
		imshow(heatmapName, heatmap);
		
		end = clock();
		elapsed = double(end - start)/ CLOCKS_PER_SEC;
		keyboard = waitKey(max(1.0,30.0-elapsed));
	}
	
	if( (char)keyboard == 'q' || (char)keyboard == 27){
		exit(0);
	}
	
	return heatmap;
}