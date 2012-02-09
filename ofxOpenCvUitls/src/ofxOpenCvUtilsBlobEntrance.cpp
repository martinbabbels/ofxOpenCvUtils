#include "ofxOpenCvUtilsBlobEntrance.h"

/*
 * ofxOpenCvUtilsBlobEntrance
 *
 */
ofxOpenCvUtilsBlobEntrance::ofxOpenCvUtilsBlobEntrance():
	 trainBackground(true)
	,nextBlobID(1)
{
	
	blobDetect = cvCreateBlobDetectorCC(); //or cvCreateBlobDetectorSimple();
}

void ofxOpenCvUtilsBlobEntrance::setBackground(IplImage* colorImage) {
	bgModel = cvCreateGaussianBGModel(colorImage,NULL);
}

/*
 * update
 *
 * @param IplImage*	The image
 *
 */
void ofxOpenCvUtilsBlobEntrance::update(IplImage* colorImage) {
	if(bgModel==NULL){
		bgModel = cvCreateGaussianBGModel(colorImage,NULL);
	}
	
	if(trainBackground) {
		cvUpdateBGStatModel(colorImage,bgModel);
		return;
	}
	
	
	blobDetect->DetectNewBlob(colorImage, bgModel->foreground, &newBlobList, &blobList);
	

	int i = 0;
	//Loop on the new blob found.
	for(i=0; i<newBlobList.GetBlobNum(); ++i) {
		CvBlob* pBN = newBlobList.GetBlob(i);
		pBN->ID = nextBlobID;
		
		//Check if the size of the new blob is big enough to be inserted in the blobList.
		if(pBN && pBN->w >= 20 && pBN->h >= 20)
		{
			cout << "Add blob #" << nextBlobID << endl; 
			blobList.AddBlob(pBN);
			nextBlobID++;    
			ofSendMessage("newBlob");
		}
	}
}