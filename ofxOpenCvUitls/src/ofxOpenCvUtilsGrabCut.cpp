#include "ofxOpenCvUtilsGrabCut.h"

/*
 * ofxOpenCvUtilsGrabCut
 *
 */
ofxOpenCvUtilsGrabCut::ofxOpenCvUtilsGrabCut () {
}


/*
 * grabcut
 *
 * @param IplImage*	The image
 * @param cv::Rect	The rectangle defining the search window
 *
 */
cv::Mat ofxOpenCvUtilsGrabCut::grabcut(IplImage* colorImage, cv::Rect rectangle, int iterations=5) {
	
	cv::Mat image = colorImage;
	cv::Mat bgModel,fgModel; // the models (internally used)

	cv::grabCut(image,				// input image
				result,				// segmentation result 
				rectangle,			// rectangle containing foreground 
				bgModel,fgModel,	// models 
				iterations,			// number of iterations 
				cv::GC_INIT_WITH_RECT); // use rectangle
	
	
	// Get the pixels marked as likely foreground 
	cv::compare(result,cv::GC_PR_FGD,result,cv::CMP_EQ); 

	// Generate output image 
	cv::Mat foreground(image.size(),CV_8UC3, cv::Scalar(255,255,255)); 
	image.copyTo(foreground, result); 

	result = result&1; // will be 1 if FG
	
	// Generate output image 
	foreground = cv::Mat(image.size(),CV_8UC3, cv::Scalar(255,255,255)); // all white image 
	image.copyTo(foreground,result); // bg pixels not copied
	
	return foreground;
}