#pragma once

#include "ofMain.h"
#include <opencv2/opencv.hpp>
#include "objectFinder.h"
#include "colorhistogram.h"

enum TrackingMethod { 
	MEANSHIFT = 0, 
	CAMSHIFT = 1
};

class ofxOpenCvUtilsMeanShift {
	
public:
	
	ofxOpenCvUtilsMeanShift();
	
	cv::Mat update(IplImage* colorImage);
	cv::Mat setROI(IplImage* roiImage, cv::Rect roiRect);
	void setMinSat(int minSat);
	void setMethod(int trackingMethod);
	void toggleDebug();
	cv::Rect getRect();
	cv::RotatedRect getRotatedRect();
	
private:
	float			minSaturation;
	bool			isDebug;
	int				method;
	cv::Mat			roi;
	cv::Rect		roiRectangle;
	cv::Rect		resultRectangle;
	cv::RotatedRect rotatedRect;
	ObjectFinder	finder;
	
};
