all: color.o main.cpp
	g++ -std=c++11 main.cpp color.o -lopencv_core -lopencv_video -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lm -oprogram
	
color.o: color.cpp color.h
	g++ -std=c++11 -c color.cpp

clean:
	rm *.o program 2>/dev/null
