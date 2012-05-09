#include "ofxOpenCvUtilsForeground.h"

/*
 * ofxOpenCvForeground
 *
 */
ofxOpenCvUtilsForeground::ofxOpenCvUtilsForeground():
	method(ACCUMULATE_WEIGHTED)
	,learningRate(0.01)
	,threshold(10)
	,blockSize(6)
	,blur(0)
	,backgroundSet(false)
{
}

/*
 * setMethod
 *
 * @param int	ACCUMULATE_WEIGHTED or MIXTURE_OF_GUASSIAN
 *
 */
void ofxOpenCvUtilsForeground::setMethod(int trackingMethod) { 
	method = trackingMethod;
}

/*
 * setBackground
 *
 * @param	IplImage*	The image
 *
 */
void ofxOpenCvUtilsForeground::setBackground(IplImage* colorImage) { 
	CvSize imgSize = cvGetSize(colorImage);
	cv::Mat float_image;
	cv::Mat image = colorImage;
	
	cv::cvtColor(image, float_image, CV_32F);
	ic.setReferenceImage(float_image);
	
	hsvBack = cvCreateImage(imgSize, colorImage->depth, colorImage->nChannels);
	cvCvtColor(colorImage, hsvBack, CV_BGR2HSV);
	
	cv::cvtColor(colorImage, gray, CV_BGR2GRAY); 
	gray.convertTo(background, CV_32F);
	backgroundSet = true;
}

/*
 * removeShadows
 *
 * @param	IplImage*	The image
 * @param	float		The saturation threshold
 *
 */
void ofxOpenCvUtilsForeground::removeShadows(cv::Mat &image, int w, int h, float satThresh=10.f) {
	if(!backgroundSet)
		return;

	vector<cv::Mat> slicesFront, slicesBack;
	cv::Mat hsvFrontMat, hsvBackMat, valFront, valBack, satFront, satBack;
	
	hsvBackMat = hsvBack;
	
	// conver raw image to hsv
	cvtColor(image, hsvFrontMat, CV_BGR2HSV);
	
	// split image to H,S and V images
	split(hsvFrontMat, slicesFront);
	split(hsvBackMat, slicesBack);
	
	slicesFront[2].copyTo(valFront); // get the value channel
	slicesFront[1].copyTo(satFront); // get the sat channel
	
	slicesBack[2].copyTo(valBack); // get the value channel
	slicesBack[1].copyTo(satBack); // get the sat channel
	
	
	int x,y;
	for(x=0; x<w; ++x) {
		for(y=0; y<h; ++y) {
			bool sat = ((satFront.at<cv::Vec3b>(y,x)[0] > satBack.at<cv::Vec3b>(y,x)[0]-shadowThreshold) &&
						(satFront.at<cv::Vec3b>(y,x)[0] < satBack.at<cv::Vec3b>(y,x)[0]+shadowThreshold));
			
			if(sat && (valFront.at<cv::Vec3b>(y,x)[0] < valBack.at<cv::Vec3b>(y,x)[0])) {
				hsvFrontMat.at<cv::Vec3b>(y,x)[0]= hsvBackMat.at<cv::Vec3b>(y,x)[0]; 
				hsvFrontMat.at<cv::Vec3b>(y,x)[1]= hsvBackMat.at<cv::Vec3b>(y,x)[1]; 
				hsvFrontMat.at<cv::Vec3b>(y,x)[2]= hsvBackMat.at<cv::Vec3b>(y,x)[2];
			}
			
		}
	}
	
	cvtColor(hsvFrontMat, image, CV_HSV2BGR);
}


/*
 * compareHist
 *
 * @param	IplImage*	The image
 * @return	double		The histogram comparison result
 *
 */
double ofxOpenCvUtilsForeground::compareHist(IplImage* colorImage) {
	cv::Mat image = colorImage;
	cv::Mat float_image;
	cv::cvtColor(image, float_image, CV_32F); 
	double result = ic.compare(float_image);
	
	return result;
}

/*
 * update
 *
 * @param	IplImage*	The image
 * @return	cv::Mat		The extracted foreground
 *
 */
cv::Mat ofxOpenCvUtilsForeground::update(IplImage* colorImage) {
	CvSize imgSize = cvGetSize(colorImage);
	cv::Mat image = colorImage;
	cv::Mat output, result;

	// Shadow removal
	if(backgroundSet && shadowThreshold>0) {
		removeShadows(image, imgSize.width, imgSize.height, shadowThreshold);
	}

	if(method==ACCUMULATE_WEIGHTED) {
		// convert to gray-level image
		cv::cvtColor(image, gray, CV_BGR2GRAY); 
		
		// initialize background to 1st frame
		if (background.empty())
			gray.convertTo(background, CV_32F);
		
		// convert background to 8U
		background.convertTo(backImage, CV_8U);
		
		// compute difference between current image and background
		cv::absdiff(backImage, gray, foreground);
		
		// apply threshold to foreground image
		cv::threshold(foreground, output, threshold, 255, cv::THRESH_BINARY_INV);
		
		// accumulate background
		cv::accumulateWeighted(gray, background, learningRate, output);
			
	}
	else if(method==MIXTURE_OF_GUASSIAN) {
	
		// update the background
		// and return the foreground
		mog(image, foreground, 0.008);
		
		// Complement the image
		cv::threshold(foreground, output, threshold, 255, cv::THRESH_BINARY_INV);
	}
	
	// Blur
	cv::medianBlur(output, output, blur);
	
	// Eliminate noise and smaller objects
	cv::erode(output, erodedForeground, cv::Mat(), cv::Point(-1,-1), blockSize);
	
	// Identify image pixels without objects
	cv::dilate(output, erodedBackground, cv::Mat(), cv::Point(-1,-1), blockSize);
	cv::threshold(erodedBackground, erodedBackground, 1, 128, cv::THRESH_BINARY_INV);
	
	
	// Set markers image
	cv::Mat markerImage(output.size(), CV_8U, cv::Scalar(0));
	markerImage = erodedForeground+erodedBackground;
	markerImage.convertTo(markers,CV_32S);

	// Watershed
	cv::watershed(image, markers);
	markers.convertTo(result, CV_8U, 255, 255);
	
	
	return erodedForeground;
}