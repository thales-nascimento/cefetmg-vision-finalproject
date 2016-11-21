#include "color.h"
#include "bgsubtractor.h"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

Mat horizontalGradient(int rows, int cols){
	Mat m(rows,cols, CV_32FC1);
	for(int row=0; row < rows; ++row){
		float *p = m.ptr<float>(row);
		for(int col = 0; col < cols; ++col){
			p[col] = (float)col/cols;
		}
	}
	return m;
}


const char *GRAYSCALE_WINDOW_NAME = "grayscale";
const char *COLOR_WINDOW_NAME = "colorscale";

void erro(){
	cerr <<"Usage: ./testcolorscale hot|jet" << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]){
	int colorscale = JET;
	
	if(argc != 2) {
		erro();
	}
	
	if(strcmp(argv[1],"hot") == 0 || strcmp(argv[1],"HOT") == 0){
		colorscale = HOT;
	} else if(strcmp(argv[1],"jet") == 0 || strcmp(argv[1],"JET") == 0){
		colorscale = JET;
	} else {
		erro();
	}
	

	Mat grayscale = horizontalGradient(100,500);
	Mat colorful = applyColor(grayscale, colorscale);
	
	namedWindow(GRAYSCALE_WINDOW_NAME);
	namedWindow(COLOR_WINDOW_NAME);
	imshow(GRAYSCALE_WINDOW_NAME, grayscale);
	imshow(COLOR_WINDOW_NAME, colorful);
	
	waitKey(0);
	destroyAllWindows();
	return EXIT_SUCCESS;
}
