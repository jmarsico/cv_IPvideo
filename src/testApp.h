#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "IPVideoGrabber.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"

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
	bool bShowVideos;
    
    ofxCv::RunningBackground background;
    ofImage thresholded;
    ofImage edge;
    
    SharedIPVideoGrabber grabber;
    ofImage grabFrame;
    float* grabH;
    float* grabW;
    
   
    ofxIntSlider backgroundThresh;
    ofxIntSlider minArea;
    ofxIntSlider maxArea;
    ofxIntSlider blurAmount;
    ofxIntSlider erodeAmount;
    ofxIntSlider dilateAmount;
    ofxIntSlider learningTime;
    ofxButton reset;
    ofxIntSlider yLoc;
    ofxIntSlider boxSize;
    ofxToggle bShowGrid;
    ofxPanel gui;
    
    
    //variables for lights
    vector <int> brights;
    int numLEDs;
    int cellSize;

    
};
