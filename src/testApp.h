#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "IPVideoGrabber.h"
#include "ofxXmlSettings.h"
#include "ofxOpenCv.h"
//#include "cv.h"
//#include "highgui.h"
//#include <opencv2/core/core.hpp>





using ofx::Video::IPVideoGrabber;
using ofx::Video::SharedIPVideoGrabber;




class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
    
    //ofxCV declarations
	float threshold;
	//ofVideoPlayer movie;
	ofxCv::ContourFinder contourFinder;
	bool showLabels;
    
    ofxCv::RunningBackground background;
    ofImage thresholded;
    ofImage edge;
    
    SharedIPVideoGrabber grabber;
    ofImage grabFrame;
    int backgroundThresh;
    //ofPixels greyFrame;
    
    
    
    // This message occurs when the incoming video stream image size changes.
    // This can happen if the IPCamera has a single broadcast state (some cheaper IPCams do this)
    // and that broadcast size is changed by another user.
   // void videoResized(const void* sender, ofResizeEventArgs& arg);

};
