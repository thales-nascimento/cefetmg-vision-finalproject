#include "color.h"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <queue>

using namespace cv;
using namespace std;

enum ClickFunction{SET, CLEAR, FLOOD, SIZE};
int clickFunction = SET;

const char *FRAME_WINDOW_NAME = "Frame";
const char *HEATMAP_WINDOW_NAME = "Heatmap";
const uchar OVERLAY_COLOR = 128;

int frameNumber=1;
VideoCapture capture;
Mat display;	//first channel: image. Second channel: overlay. Third channel: nothing
Mat frame;
vector<Mat> displayBuilder(3);
Mat& frameIntensity = displayBuilder[2];
Mat& frameOverlay = displayBuilder[0];
Mat& frameUnnused = displayBuilder[1];

Mat heatmap;
Mat accumulator;
unsigned maxValue = 1;
int floodTolerance = 1;
int lado=0;

bool *visitado;

void endApplication(){
	destroyWindow(FRAME_WINDOW_NAME);
	capture.release();
	exit(0);
}

void redraw(bool updateHeatmap){
	if(updateHeatmap){
		Mat grayHeatmap;
		accumulator.convertTo(grayHeatmap, CV_32FC1, 1.0/(maxValue));
		heatmap = applyColor(grayHeatmap, HOT);
		imshow(HEATMAP_WINDOW_NAME, heatmap);
	}
	merge(displayBuilder, display);
	imshow(FRAME_WINDOW_NAME, display);
}

void clearOverlay(int state, void*){
	frameOverlay.forEach<uchar>([](uchar &pixel, const int* position) -> void {
		pixel = 0;
	});
	if(state == 0){
		redraw(false);
	}
}

void setClickFunction(int state, void*){
	clickFunction = (clickFunction+1)%SIZE;
	switch(clickFunction){
		case SET: printf("Click set: set\n");break;
		case CLEAR: printf("Click set: clear\n");break;
		case FLOOD: printf("Click set: flood\n");break;
	}
}

void commit(int state, void*){
	for(int row=0; row < accumulator.rows; ++row){
		uchar *f = frameOverlay.ptr(row);
		unsigned *a = (unsigned*)accumulator.ptr<int>(row);
		for(int col = 0; col < accumulator.cols; ++col){
			a[col] += f[col];
			maxValue = max(maxValue, a[col]);
		}
	}
	if(capture.read(frame)){
		cvtColor(frame, frameIntensity, CV_BGR2GRAY);
		clearOverlay(1,nullptr);
		redraw(true);
		printf("current frame: %d\n",++frameNumber);
	} else {
		endApplication();
	}
}

void selectObject(CvPoint center, float tolerance){
	const static int adjy[4] = {-1,0,1,0};
	const static int adjx[4] = {0,-1,0,1};
	uchar firstPixel = frameIntensity.at<uchar>(center.x,center.y);
	uchar minValue = max(0,firstPixel-int(nearbyint(tolerance*255.0)));
	uchar maxValue = min(255, firstPixel+int(nearbyint(tolerance*255.0)));
	queue<CvPoint> fronteira;
	
	memset(visitado,0,frame.rows*frame.cols);
	fronteira.push(center);
	while(!fronteira.empty()){
		CvPoint current = fronteira.front(); 
		fronteira.pop();
		for(int i=0;i<4;++i){
			int y = current.y + adjy[i];
			int x = current.x + adjx[i];
			if(y < 0 || y >= frame.rows-1 || x < 0 || x >= frame.cols-1 || visitado[x + y*frame.cols]){
				continue;
			}
			uchar adj = frameIntensity.ptr(y)[x];
			if(adj >= minValue && adj <= maxValue){
				fronteira.push(CvPoint(x,y));
				visitado[x + y*frame.cols] = true;
				frameOverlay.ptr(y)[x] = OVERLAY_COLOR;
			}
		}
	}
}

void mouseCallBack(int event, int x,int y,int flags,void *userdata){
	static bool drawing = 0;
	string overlay;
	
	if(event == EVENT_LBUTTONDOWN){
		drawing = true;
	} else if(event == EVENT_LBUTTONUP ){
		drawing = false;
	}
	
	if(drawing){
		CvPoint topLeft(x-lado,y-lado);
		CvPoint bottomRight(x+lado,y+lado);
		switch(clickFunction){
			case ClickFunction::SET:
				rectangle(frameOverlay, topLeft, bottomRight, Scalar(OVERLAY_COLOR), CV_FILLED);
				break;
			case ClickFunction::CLEAR:
				rectangle(frameOverlay, topLeft, bottomRight, Scalar(0), CV_FILLED);
				break;
			case ClickFunction::FLOOD:
				selectObject(CvPoint(x,y),floodTolerance*0.01f);
				break;
		}
		overlay = "drawing ";
		redraw(true);
	}
	
	overlay += "y="+to_string(y) + " x="+to_string(x);
	displayStatusBar(FRAME_WINDOW_NAME, overlay);
}

int main(int argc, char* argv[]){
	int keyboardInput=0;
	int colorscale = HOT;
	char *videoPath;
		
	if(argc < 2){
		cerr <<"Usage: ./program videofile" << endl;
		exit(EXIT_FAILURE);
	}
	videoPath = argv[argc-1];
	
	capture = VideoCapture(videoPath);
	if(!capture.isOpened()){
		printf("Unable to open video file: %s\n", videoPath);
		exit(EXIT_FAILURE);
	}
	
	capture.read(frame);
	cvtColor(frame, frameIntensity, CV_BGR2GRAY);
	frameOverlay = Mat::zeros(frame.size(), frameIntensity.type());
	frameUnnused = Mat::zeros(frame.size(), frameIntensity.type());
	accumulator = Mat::zeros(frame.size(), CV_32SC1);
	visitado = new bool[frame.rows*frame.cols];
	
	namedWindow(FRAME_WINDOW_NAME);
	namedWindow(HEATMAP_WINDOW_NAME);

	createButton("clear", clearOverlay);
	createButton("next frame", commit);
	createButton("change mode", setClickFunction);
	createTrackbar("flood tolerance (%)", nullptr, &floodTolerance, 100, nullptr);
	createTrackbar("lado (px)", nullptr, &lado, 10, nullptr);
	redraw(true);
	
	cvSetMouseCallback(FRAME_WINDOW_NAME, mouseCallBack, nullptr);
	
	while(1){
		keyboardInput = waitKey(0);
		if(keyboardInput == '+' || keyboardInput == '-' || (keyboardInput >= '0' && keyboardInput <= '9')){
			if(keyboardInput == '+'){
				lado += 1;
			}else if(keyboardInput == '-'){
				lado = max(0,lado-1);
			} else {
				lado = keyboardInput-'0';
			}
			printf("lado: %d\n",lado);
		}else if(keyboardInput == '*' || keyboardInput == '/'){
			if(keyboardInput == '*'){
				floodTolerance = min(100, floodTolerance+1);
			} else {
				floodTolerance = max(0, floodTolerance-1);
			}
			printf("tolerance: %d%%\n", floodTolerance);
		}else if(keyboardInput == 27){
			endApplication();
		}else if(keyboardInput == 'n'){
			commit(0,nullptr);
		}else if(keyboardInput == 'c' || keyboardInput == 'e'){
			clickFunction = CLEAR;
			printf("Click mode: clear\n");
		}else if(keyboardInput == 's'){
			clickFunction = SET;
			printf("Click mode: clear\n");
		}else if(keyboardInput == 'f'){
			clickFunction = FLOOD;
			printf("Click mode: flood\n");
		}
	}
	
	return EXIT_SUCCESS;
}
