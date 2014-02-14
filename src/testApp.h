#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "IPVideoGrabber.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
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
    void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	
    
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
    //int backgroundThresh;
    
    ofxIntSlider backgroundThresh;
    ofxIntSlider minArea;
    ofxIntSlider maxArea;
    ofxIntSlider contourThresh;
    ofxPanel gui;
    
};
