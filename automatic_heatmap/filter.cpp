#include "filter.h"

Vec3b SigmaFilter::evaluateKernel(const Mat& kernel, const Vec3b& midpoint){
	Vec<int,3> contribuitors(0,0,0);
	Vec<int,3> accumulator(0,0,0);
	
	for(int row=0; row < kernel.rows; ++row){
		const Vec3b *k = kernel.ptr<Vec3b>(row);
		for(int col=0; col < kernel.cols; ++col){
			Vec3b delta = midpoint-k[col];
			for(int i=0;i<3;++i){
				if(delta[i] <= sigma){
					contribuitors[i] += 1;
					accumulator[i] += k[col][i];
				}
			}
		}
	}
	
	for(int i=0;i<3;++i){
		contribuitors[i] = max(1,contribuitors[i]);
	}
	return Vec3b(accumulator[0]/contribuitors[0], accumulator[1]/contribuitors[1], accumulator[2]/contribuitors[2]);
}

Mat SigmaFilter::filter(const Mat image){
	Mat ans(image.size(), CV_8UC3);
	for(int row=0; row < image.rows; ++row){
		const Vec3b *i = image.ptr<Vec3b>(row);
		Vec3b *a = ans.ptr<Vec3b>(row);
		for(int col=0; col < image.cols; ++col){
			Range rangeY(max(0,row-windowK), min(image.rows-1,row+windowK));
			Range rangeX(max(0,col-windowK), min(image.cols-1,col+windowK));
			Mat kernel = Mat(image, rangeY, rangeX);
			const Vec3b& windowMid = i[col];
			a[col] = evaluateKernel(kernel, windowMid);
		}
	}
	return ans;
}

GaussianFilter::GaussianFilter(int kWidth, int kHeight, double sigmaX, double sigmaY, int borderType):kSize(kWidth,kHeight),sigmaX(sigmaX),sigmaY(sigmaY),borderType(borderType){}

Mat GaussianFilter::filter(const Mat image){
	Mat out;
	GaussianBlur(image,out,kSize,sigmaX,sigmaY,borderType);
	return out;
}