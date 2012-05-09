#pragma once

#include "ofMain.h"
#include "imageComparator.h"
#include <opencv2/opencv.hpp>

typedef auto_ptr<vector<cv::Point> > vector_ptr;

enum SubtractorMethod { 
	ACCUMULATE_WEIGHTED = 0, 
	MIXTURE_OF_GUASSIAN = 1
};

class ofxOpenCvUtilsForeground {
	
public:
	
	ofxOpenCvUtilsForeground();
	cv::Mat update(IplImage* colorImage);
	double compareHist(IplImage* colorImage);
	void setMethod(int trackingMethod);
	void setBackground(IplImage* colorImage);
	
	double learningRate;    // learning rate in background accumulation
	int threshold;			// threshold for foreground extraction
	int method;				// Subtraction method
	int blockSize;			// The watershed blockSize
	float blur;				// Blur
	float shadowThreshold;	// Shadow removal threshold
	bool backgroundSet;		// is background set
	
private:
	
	void removeShadows(cv::Mat &image, int w, int h, float satThresh);

protected:
	
	cv::BackgroundSubtractorMOG mog;	// MoG
	
	cv::Mat foreground;					// foreground image
	cv::Mat background;					// accumulated background
	cv::Mat gray;						// current gray-level image
	cv::Mat backImage;					// background image
	cv::Mat erodedForeground;			// eroded foreground
	cv::Mat erodedBackground;			// eroded background
	cv::Mat markers;					// markers
	
	IplImage* hsvBack;					// hsv background image
	ImageComparator ic;
	
};
