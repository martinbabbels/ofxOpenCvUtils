#pragma once

#include "ofMain.h"
#include <opencv2/opencv.hpp>

class ofxOpenCvUtilsGrabCut {
	
public:
	
	ofxOpenCvUtilsGrabCut();
	cv::Mat grabcut(IplImage* colorImage, cv::Rect rectangle, int iterations);
	
private:
	cv::Mat			result;
	
};
