#pragma once

#include "ofMain.h"
#include <opencv2/opencv.hpp>
#include <opencv/cvaux.h>

class ofxOpenCvUtilsBlobEntrance {
	
public:
	
	ofxOpenCvUtilsBlobEntrance();
	void update(IplImage* colorImage);
	void setBackground(IplImage* colorImage);
	bool trainBackground;
	
private:
	CvBGStatModel* bgModel;
	CvBlobDetector* blobDetect;
	CvBlobSeq newBlobList;
	CvBlobSeq blobList;
	int nextBlobID;
};
