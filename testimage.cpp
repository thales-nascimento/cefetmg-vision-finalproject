#include "testimage.h"

cv::Mat horizontalGradient(int rows, int cols){
	cv::Mat m(rows,cols, CV_32FC1);
	for(int row=0; row < rows; ++row){
		float *p = m.ptr<float>(row);
		for(int col = 0; col < cols; ++col){
			p[col] = (float)col/cols;
		}
	}
	return m;
}