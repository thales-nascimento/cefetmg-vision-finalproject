all: color.o bgsubtractor.o filteredcapture.o filter.o
	g++ -std=c++11 main.cpp color.o bgsubtractor.o filteredcapture.o filter.o -lopencv_core -lopencv_video -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lopencv_imgcodecs -oprogram
	
testcolorscale: color.o
	g++ -std=c++11 testcolorscale.cpp color.o -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -otestcolorscale

color.o:
	g++ -std=c++11 -c color.cpp

bgsubtractor.o: 
	g++ -std=c++11 -c bgsubtractor.cpp 

filteredcapture.o:
	g++ -std=c++11 -c filteredcapture.cpp

filter.o:
	g++ -std=c++11 -c filter.cpp

clean:
	rm *.o program testcolorscale 2>/dev/null
