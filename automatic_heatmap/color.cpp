#include "color.h"

Mat applyColor(Mat m,int type){
	Mat n(m.size(), CV_32FC3);
	auto outputIt = n.begin<Vec3f>(), outputEnd = n.end<Vec3f>();
	auto inputIt = m.begin<float>();
	while(outputIt != outputEnd){
		float& r = (*outputIt)[2];
		float& g = (*outputIt)[1];
		float& b = (*outputIt)[0];
		const float& i = *inputIt;
		if(type==JET){
			const float alpha = 3.94f;
			
			if(i==0){
				r=0;
				b=0;
				g=0;
			}else if(i < 0.11f){
				b = alpha*i + 0.56f;
				g = 0.0f;
				r = 0.0f;
			} else if(i < 0.37f){
				b = 1.0f;
				g = alpha*i -0.47f;
				r = 0.0f;
			} else if(i < 0.62f){
				b = -alpha*i + 2.44f;
				g = 1.0f;
				r = alpha*i -1.46f;
			} else if(i < 0.87 ){
				b = 0;
				g = -alpha*i + 3.43f;
				r = 1.0f;
			} else {
				b = 0;
				g = 0;
				r = -alpha*i + 4.43f;
			}
			
		} else if(type==HOT){
			const float alpha = 2.5f;
			
			if(i < 0.4f){
				r=alpha*i;
				g=0.0f;
				b=0.0f;
			} else if(i < 0.8f){
				r=1.0f;
				g=alpha*i-1.0f;
				b=0.0f;
			} else {
				r=1.0f;
				g=1.0f;
				b=alpha*2.0f*i-4.0f;
			}
		}
		
		++outputIt;
		++inputIt;
	}
	return n;
}
