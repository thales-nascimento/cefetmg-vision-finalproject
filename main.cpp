#include "color.h"
#include "bgsubtractor.h"
#include "filteredcapture.h"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

const char *ORIGINAL_WINDOW_NAME = "Original video";
const char *FOREGROUND_WINDOW_NAME = "Foreground";
const char *HEATMAP_WINDOW_NAME = "Heatmap";

void erro(){
	cerr <<"Usage: ./program [-colorscale hot|jet ] [-play] videofile" << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]){
	int colorscale = JET;
	char *videoPath;
	bool slowPlay = false;
	
	if(argc < 2) {
		erro();
	}
	videoPath = argv[argc-1];
	
	for(int i=1;i<argc-1;++i){
		if(strcmp(argv[i], "-colorscale") == 0){
			if(i == argc-1){
				erro();
			}
			if(strcmp(argv[i+1],"hot") == 0 || strcmp(argv[i+1],"HOT") == 0){
				colorscale = HOT;
			} else if(strcmp(argv[i+1],"jet") == 0 || strcmp(argv[i+1],"JET") == 0){
				colorscale = JET;
			} else {
				erro();
			}
			i+=1;
		
		} else if(strcmp(argv[i], "-play") == 0){
			slowPlay = true;
		
		} else {
			erro();
		}
	}
	
	namedWindow(HEATMAP_WINDOW_NAME);


	FilteredCapture capture(videoPath);
	if(!capture.isOpened()){
		printf("Unable to open video file: %s\n", videoPath);
		exit(EXIT_FAILURE);
	}
	if(slowPlay){
		namedWindow(ORIGINAL_WINDOW_NAME);
		namedWindow(FOREGROUND_WINDOW_NAME);
		Mat fromvideo = processVideo(&capture, createBackgroundSubtractorMOG2(), ORIGINAL_WINDOW_NAME, FOREGROUND_WINDOW_NAME, HEATMAP_WINDOW_NAME); 
		capture.release();
		cvDestroyWindow(ORIGINAL_WINDOW_NAME);
		cvDestroyWindow(FOREGROUND_WINDOW_NAME);
		Mat colorful = applyColor(fromvideo, JET);
		imshow(HEATMAP_WINDOW_NAME, colorful);
	
	} else {
		capture = FilteredCapture(videoPath);
		capture.attachFilter(GAUSSIAN);
		Mat fromsilent = processVideoSilently(&capture, createBackgroundSubtractorMOG2());
		capture.release();
		Mat colorful = applyColor(fromsilent, colorscale);
		imshow(HEATMAP_WINDOW_NAME, colorful);
	}
	
	waitKey(0);
	destroyAllWindows();
	return EXIT_SUCCESS;
}
