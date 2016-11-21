#include "color.h"

Mat applyColor(Mat m,int type){
	Mat n;
	
	cvtColor(m,n,CV_GRAY2RGB);
	for(auto it = n.begin<Vec3f>(), end = n.end<Vec3f>(); it != end; ++it){
		Vec3f& pixel = *it;
		float& r = pixel[2];
		float& g = pixel[1];
		float& b = pixel[0];
		if(type==JET){
			float alpha = 3.94f;
			
			if(b==0) ;	
			else if(b < 0.11f) b = alpha*b + 0.56f;
			else if(b <= 0.37f) b = 1.0f;
			else if(b <= 0.62f) b = -alpha*b + 2.44f;
			else b = 0;
	
			if(g==0) ;	
			else if( g < 0.12f) g = 0.0f;
			else if(g < 0.37f) g = alpha*g -0.47f;
			else if(g <= 0.62f) g = 1.0f;
			else if(g <= 0.87f) g = -alpha*g + 3.43f;
			else g = 0;
	
			if(r==0) ;	
			else if( r < 0.37f) r = 0.0f;
			else if(r < 0.62f) r = alpha*r -1.46f;
			else if(r <= 0.87f) r = 1.0f;
			else r = -alpha*r + 4.43f;
		}
		else if(type==HOT){
			float alpha = 2.5f;
				
			if(r<=0.4f) r=alpha*r;
			else r=1.0f;

			if(g<=0.4f) g=.0f;
			else if(g<=0.8f) g=alpha*g-1.0f;
			else	g=1.0f;
			
			if(b<=0.8f) b=.0f;
			else b=alpha*2.0f*b-4.0f;	

		}
		
	}
	return n;
} 
