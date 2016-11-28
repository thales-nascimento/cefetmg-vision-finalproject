#ifndef FILTER_H
#define FILTER_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

enum filters{SIGMA, GAUSSIAN, NONE};

struct Filter{
	virtual Mat filter(const Mat image) = 0;
};

class SigmaFilter : public Filter{
public:
	SigmaFilter(int windowK=2, uchar sigma=50):windowK(windowK), sigma(sigma){};
	virtual Mat filter(const Mat image) override;
	
private:
	int windowK;
	uchar sigma;
	Vec3b evaluateKernel(const Mat& kernel, const Vec3b& midpoint);
};

class GaussianFilter : public Filter{
public:
	GaussianFilter(int kWidth=3, int kHeight=3, double sigmaX=30, double sigmaY=0, int borderType=BORDER_DEFAULT);
	virtual Mat filter(const Mat image) override;
private:
	Size kSize;
	double sigmaX;
	double sigmaY;
	int borderType;
};

#endif