#include "ofxOpenCvUtilsMeanShift.h"


/*
 * ofxOpenCvUtilsMeanShift
 *
 */
ofxOpenCvUtilsMeanShift::ofxOpenCvUtilsMeanShift()
	:minSaturation(5)
	,isDebug(false)
	,resultRectangle(cv::Rect(0, 0, 0, 0)) 
	,method(CAMSHIFT)
{
	
}

/*
 * setROI
 *
 * @param	IplImage*	The ROI image
 * @param	cv::Rect	The ROI rect
 * @return	cv::Mat		The backprojection image
 *
 */
cv::Mat ofxOpenCvUtilsMeanShift::setROI(IplImage* roiImage, cv::Rect roiRect) {

	// Read reference image
	IplImage* src_image = cvCreateImage(cvGetSize(roiImage), 8, 3);
	src_image->origin = roiImage->origin;
	
	cv::Mat image;
	cvCopy( roiImage, src_image, 0 );
	cvtColor(src_image, image, CV_RGB2BGR);
	
	if (!image.data)
		return image; 
	
	// Define ROI
	cv::Mat imageROI = image(roiRect);
	
	// Get the Hue histogram
	ColorHistogram hc;
	cv::MatND colorhist = hc.getHueHistogram(imageROI);

	finder.setHistogram(colorhist);
	finder.setThreshold(0.2f);
	
	// Convert to HSV space
	cv::Mat hsv;
	cv::cvtColor(image, hsv, CV_BGR2HSV);
	
	// Split the image
	vector<cv::Mat> v;
	cv::split(hsv, v);
	
	// Eliminate pixels with low saturation
	cv::threshold(v[1], v[1], minSaturation, 255, cv::THRESH_BINARY);
	
	// Get back-projection of hue histogram
	int ch[1] = {0};
	cv::Mat result = finder.find(hsv, 0.0f, 180.0f, ch, 1);
	cv::bitwise_and(result, v[1], result);
	
	// Set the ROI rectangle
	roiRectangle = roiRect;
	
	return result;
}

/*
 * getRect
 *
 * @return	cv::Rect	The result rectangle
 *
 */
cv::Rect ofxOpenCvUtilsMeanShift::getRect() {
	return resultRectangle;
}

/*
 * getRotatedRect
 *
 * @return	cv::Rect	The rotated result rectangle
 *
 */
cv::RotatedRect ofxOpenCvUtilsMeanShift::getRotatedRect() {
	return rotatedRect;
}

/*
 * setMinSat
 *
 * @param int	The min saturation
 *
 */
void ofxOpenCvUtilsMeanShift::setMinSat(int minSat) { 
	minSaturation = minSat;
}

/*
 * setMethod
 *
 * @param int	MEANSHIFT or CAMSHIFT
 *
 */
void ofxOpenCvUtilsMeanShift::setMethod(int trackingMethod) { 
	method = trackingMethod;
}

/*
 * update
 *
 * @param	IplImage*	The current image
 * @return	cv::Mat		The backprojection image
 *
 */
cv::Mat ofxOpenCvUtilsMeanShift::update(IplImage* colorImage) {
	
	// Read reference image
	IplImage* src_image = cvCreateImage(cvGetSize(colorImage), 8, 3);
	src_image->origin = colorImage->origin;
	
	cv::Mat image;
	cvCopy( colorImage, src_image, 0 );
	cvtColor(src_image, image, CV_RGB2BGR);
	
	// Convert to HSV space
	cv::Mat hsv;
	cv::cvtColor(image, hsv, CV_BGR2HSV);
	
	// Split the image
	vector<cv::Mat> v;
	cv::split(hsv, v);
	
	// Eliminate pixels with low saturation
	cv::threshold(v[1], v[1], minSaturation, 255, cv::THRESH_BINARY);

	// Get back-projection of hue histogram
	int ch[1] = {0};
	cv::Mat result;
	result = finder.find(hsv, 0.0f, 180.0f, ch, 1);

	// Eliminate low stauration pixels
	cv::bitwise_and(result, v[1], result);
	
	// Get back-projection of hue histogram
	finder.setThreshold(-1.0f);

	result = finder.find(hsv, 0.0f, 180.0f, ch, 1);
	cv::bitwise_and(result, v[1], result);

	resultRectangle = roiRectangle;
	
	// MeanShift
	if(method==MEANSHIFT) {
		cv::TermCriteria criteria(cv::TermCriteria::MAX_ITER, 10, 0.01);
		cv::meanShift(result, resultRectangle, criteria);
	} else if (method==CAMSHIFT) {
		cv::TermCriteria criteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 );
		rotatedRect = cv::CamShift( result, resultRectangle, criteria);
	}
	
	return result;
}