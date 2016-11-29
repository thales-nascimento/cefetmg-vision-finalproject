#include "color.h"
#include "bgsubtractor.h"
#include "filteredcapture.h"
#include "filter.h"
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
	cerr <<"Usage: ./program [-colorscale hot|jet ] [-filter sigma|gaussian ] [-play] videofile" << endl;
	exit(EXIT_FAILURE);
}

void toLower(char* str){
	while(*str){
		char c = *str;
		if(c >= 'A' && c <= 'Z'){
			*str -= 'A'-'a';
		}
		++str;
	}
}

int main(int argc, char* argv[]){
	int colorscale = JET;
	int filter = NONE;
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
			i+=1;
			toLower(argv[i]);
			if(strcmp(argv[i],"hot") == 0){
				colorscale = HOT;
			} else if(strcmp(argv[i],"jet") == 0){
				colorscale = JET;
			} else {
				erro();
			}
		
		} else if(strcmp(argv[i], "-filter") == 0){
			if(i == argc-1){
				erro();
			}
			i+=1;
			toLower(argv[i]);
			if(strcmp(argv[i], "sigma") == 0){
				filter = SIGMA;
			} else if(strcmp(argv[i], "gaussian") == 0){
				filter = GAUSSIAN;
			} else {
				erro();
			}
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
	if(filter != NONE){
		capture.attachFilter(filter);
	}
	
	Ptr<BackgroundSubtractorMOG2> bgSubtractor = createBackgroundSubtractorMOG2();
	
	if(slowPlay){
		namedWindow(ORIGINAL_WINDOW_NAME);
		namedWindow(FOREGROUND_WINDOW_NAME);
		Mat fromvideo = processVideo(&capture, bgSubtractor, ORIGINAL_WINDOW_NAME, FOREGROUND_WINDOW_NAME, HEATMAP_WINDOW_NAME); 
		capture.release();
		cvDestroyWindow(ORIGINAL_WINDOW_NAME);
		cvDestroyWindow(FOREGROUND_WINDOW_NAME);
		Mat colorful = applyColor(fromvideo, JET);
		imshow(HEATMAP_WINDOW_NAME, colorful);
	
	} else {
		Mat fromsilent = processVideoSilently(&capture, bgSubtractor);
		capture.release();
		Mat colorful = applyColor(fromsilent, colorscale);
		imshow(HEATMAP_WINDOW_NAME, colorful);
	}
	
	waitKey(0);
	return EXIT_SUCCESS;
}
