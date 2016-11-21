all:
	g++ main.cpp -std=c++11 -lopencv_core -lopencv_video -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lopencv_imgcodecs -oprogram
clean:
	rm *.o program
