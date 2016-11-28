#include "filteredcapture.h"

bool FilteredCapture::read(OutputArray image){
	bool ans = VideoCapture::read(image);
	if(filter != nullptr){
		Mat _image = filter->filter(image.getMat());
		image.assign(_image);
	}
	return ans;
}

void FilteredCapture::attachFilter(int filterType){
	if(filterType == SIGMA){
		filter = new SigmaFilter();
		printf("attaching sigma filter\n");
	} else if(filterType == GAUSSIAN){
		filter = new GaussianFilter();
		printf("attaching gaussian filter\n");
	}
}