/*
 <streams>
 
 <stream url="http://148.61.142.228/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 <stream url="http://82.79.176.85:8081/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 <stream url="http://81.8.151.136:88/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 <stream url="http://216.8.159.21/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 <stream url="http://kassertheatercam.montclair.edu/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 <stream url="http://213.77.33.2:8080/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 <stream url="http://81.20.148.158/anony/mjpg.cgi"/>
 <stream url="http://173.167.157.229/anony/mjpg.cgi"/>
 <stream url="http://67.181.87.150/anony/mjpg.cgi"/>
 <stream url="http://72.227.87.110/anony/mjpg.cgi"/>
 <stream url="http://69.205.126.54/anony/mjpg.cgi"/>
 <stream url="http://173.196.179.29/anony/mjpg.cgi"/>
 <stream url="http://208.105.17.62/anony/mjpg.cgi"/>
 <stream url="http://67.208.104.218/anony/mjpg.cgi"/>
 <stream url="http://212.42.63.190/anony/mjpg.cgi"/>
 <stream url="http://94.246.211.222/anony/mjpg.cgi"/>
 <stream url="http://213.251.201.196/anony/mjpg.cgi"/>
 <stream url="http://208.100.33.174/anony/mjpg.cgi"/>
 <stream url="http://85.186.35.67/anony/mjpg.cgi"/>
 <stream url="http://98.189.188.232/anony/mjpg.cgi"/>
 <stream url="http://209.119.5.4/anony/mjpg.cgi"/>
 <stream url="http://24.155.150.53/anony/mjpg.cgi"/>
 <stream url="http://98.235.174.112/anony/mjpg.cgi"/>
 <stream url="http://88.170.122.125/anony/mjpg.cgi"/>
 -->
 </streams>*/

#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetVerticalSync(true);
	ofBackground(100);
    //ofSetFrameRate(30);
    
    gui.setup("panel");
    gui.add(backgroundThresh.setup("thresh", 100, 0, 255));
    
    gui.setPosition(650, 680);
    thresholded.allocate(640, 480, OF_IMAGE_COLOR);
	
    //cam.initGrabber(320, 240);
    
    //initialize connection
    grabber = IPVideoGrabber::makeShared();

    grabber->setCameraName("cam1");
    grabber->setURI("http://82.79.176.85:8081/axis-cgi/mjpg/video.cgi?resolution=320x240");
    grabber->connect();
    
	contourFinder.setMinAreaRadius(1);
    contourFinder.setMaxAreaRadius(100);
	contourFinder.setThreshold(200);
    contourFinder.setAutoThreshold(false);
	 //wait for half a frame before forgetting something
	contourFinder.getTracker().setPersistence(50);
	// an object can move up to 32 pixels per frame
	contourFinder.getTracker().setMaximumDistance(32);
    
    background.setLearningTime(50);
    background.setThresholdValue(200);

	
	showLabels = true;
    
    
     
}



//------------------------------------------------------------------------------
void testApp::update() {
	// update the cameras
    grabber->update();
    
    
    
    
    grabFrame.setFromPixels(grabber->getPixels(), grabber->getWidth(), grabber->getHeight(), OF_IMAGE_COLOR);
    
    grabFrame.resize(640, 480);
    
    
   // backgroundThresh = ofMap(mouseX, 0, ofGetWidth(), 0, 255);
    if(backgroundThresh < 1)
    {
        backgroundThresh = 0;
    }
    if(backgroundThresh > 254)
    {
        backgroundThresh = 255;
    }
    
    background.setThresholdValue(backgroundThresh);
    background.update(grabFrame, thresholded);
    thresholded.update();
    
    blur(thresholded, 10);
    contourFinder.setThreshold(ofMap(mouseX, 0, ofGetWidth(), 0, 255));
    contourFinder.findContours(thresholded);
    ofLog() << "number contours: " << contourFinder.size();
    
  
}


//------------------------------------------------------------------------------
void testApp::draw() {
    
    gui.draw();
	ofSetBackgroundAuto(showLabels);
	RectTracker& tracker = contourFinder.getTracker();
	ofSetColor(255);
    
	if(showLabels) {
        grabFrame.draw(0,10);
        thresholded.draw(grabFrame.getWidth()+10, 10);
        //grabber->draw(grabFrame.getWidth()+10, grabFrame.getHeight()+20);
		ofSetColor(0, 244, 0);
        ofPushMatrix();
            ofTranslate(0,10);
            contourFinder.draw();
            for(int i = 0; i < contourFinder.size(); i++) {
                ofPoint center = toOf(contourFinder.getCenter(i));
                ofPushMatrix();
                ofTranslate(center.x, center.y);
                int label = contourFinder.getLabel(i);
                string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
                ofDrawBitmapString(msg, 0, 0);
                ofVec2f velocity = toOf(contourFinder.getVelocity(i));
                ofScale(5, 5);
                ofLine(0, 0, velocity.x, velocity.y);
                ofPopMatrix();
            
		}
        ofPopMatrix();
         
         
	}
    else {
		for(int i = 0; i < contourFinder.size(); i++) {
			unsigned int label = contourFinder.getLabel(i);
			// only draw a line if this is not a new label
			if(tracker.existsPrevious(label)) {
				// use the label to pick a random color
				ofSeedRandom(label << 24);
				ofSetColor(ofColor::fromHsb(ofRandom(255), 255, 255));
				// get the tracked object (cv::Rect) at current and previous position
				const cv::Rect& previous = tracker.getPrevious(label);
				const cv::Rect& current = tracker.getCurrent(label);
				// get the centers of the rectangles
				ofVec2f previousPosition(previous.x + previous.width / 2, previous.y + previous.height / 2);
				ofVec2f currentPosition(current.x + current.width / 2, current.y + current.height / 2);
				ofLine(previousPosition, currentPosition);
			}
		}
	}
	
	// this chunk of code visualizes the creation and destruction of labels
	const vector<unsigned int>& currentLabels = tracker.getCurrentLabels();
	const vector<unsigned int>& previousLabels = tracker.getPreviousLabels();
	const vector<unsigned int>& newLabels = tracker.getNewLabels();
	const vector<unsigned int>& deadLabels = tracker.getDeadLabels();
	ofSetColor(cyanPrint);
	for(int i = 0; i < currentLabels.size(); i++) {
		int j = currentLabels[i];
		ofLine(j, 0, j, 4);
	}
	ofSetColor(magentaPrint);
	for(int i = 0; i < previousLabels.size(); i++) {
		int j = previousLabels[i];
		ofLine(j, 4, j, 8);
	}
	ofSetColor(yellowPrint);
	for(int i = 0; i < newLabels.size(); i++) {
		int j = newLabels[i];
		ofLine(j, 8, j, 12);
	}
	ofSetColor(ofColor::white);
	for(int i = 0; i < deadLabels.size(); i++) {
		int j = deadLabels[i];
		ofLine(j, 12, j, 16);
	}
    
    
    // finally, a report:
	ofSetColor(0,255,0);
	stringstream reportStr;
        reportStr
        << "press ' ' to show label viz" << endl
        << "threshold " << backgroundThresh << " move mouse" << endl
        << "num blobs found " << contourFinder.size() << ", fps: " << ofGetFrameRate() << endl
        << "width of IP feed: " << grabber->getWidth() << endl
        << "height of IP feed: " << grabber->getHeight() << endl
        << "IP cam framerate: " << grabber->getFrameRate() << endl
        << "URL: " << grabber->getURI() << endl;
        ofDrawBitmapString(reportStr.str(), 20, 680);
    
    ofDrawBitmapString("Feed + Contour Tracking", 0, 500);
    ofDrawBitmapString("Background Subtraction", 650, 500);
    
    
}


//------------------------------------------------------------------------------
void testApp::keyPressed(int key) {
	if(key == ' ') {
		showLabels = !showLabels;
	}
}


//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

