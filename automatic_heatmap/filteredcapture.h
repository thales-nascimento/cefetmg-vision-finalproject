#ifndef FILTERED_CAPTURE_H
#define FILTERED_CAPTURE_H

#include "filter.h"
#include <opencv2/videoio.hpp>

class FilteredCapture : public VideoCapture{
public:
	void attachFilter(int filterType);
	virtual bool read(OutputArray image) override;
	
	FilteredCapture(const char* videoPath):VideoCapture(videoPath){};
	~FilteredCapture(){
		if(filter != nullptr){
			delete filter;
		}
	}
	
private:
	Filter *filter = nullptr;
};

#endif