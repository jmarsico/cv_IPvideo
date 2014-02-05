#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetVerticalSync(true);
	ofBackground(0);
    ofSetFrameRate(30);
    loadCameras();
	
    //uncomment this if using movie on disk
	//movie.loadMovie("video.mov");
	//movie.play();
    
    // initialize connection
    /*
    for(std::size_t i = 0; i < NUM_CAMERAS; i++)
    {
        IPCameraDef& cam = getNextCamera();
        
        SharedIPVideoGrabber c = IPVideoGrabber::makeShared();
        
        // if your camera uses standard web-based authentication, use this
        // c->setUsername(cam.username);
        // c->setPassword(cam.password);
        
        // if your camera uses cookies for authentication, use something like this:
        // c->setCookie("user", cam.username);
        // c->setCookie("password", cam.password);
        
        c->setCameraName(cam.getName());
        c->setURI(cam.getURL());
        c->connect(); // connect immediately
        
        // if desired, set up a video resize listener
        //ofAddListener(c->videoResized, this, &testApp::videoResized);
        
        grabbers.push_back(c);
     
    }
    */

	IPCameraDef& cam = ipcams[0];
    grabber = IPVideoGrabber::makeShared();
    
    grabber->setCameraName(cam.getName());
    grabber->setURI(cam.getURL());
    grabber->connect();
    
	contourFinder.setMinAreaRadius(1);
	contourFinder.setMaxAreaRadius(100);
	contourFinder.setThreshold(1);
	// wait for half a frame before forgetting something
	contourFinder.getTracker().setPersistence(50);
	// an object can move up to 32 pixels per frame
	contourFinder.getTracker().setMaximumDistance(32);
	
	showLabels = true;
}



//------------------------------------------------------------------------------
IPCameraDef& testApp::getNextCamera()
{
    nextCamera = (nextCamera + 1) % ipcams.size();
    return ipcams[nextCamera];
}

//------------------------------------------------------------------------------
void testApp::loadCameras()
{
    
    // all of these cameras were found using this google query
    // http://www.google.com/search?q=inurl%3A%22axis-cgi%2Fmjpg%22
    // some of the cameras below may no longer be valid.
    
    // to define a camera with a username / password
    //ipcams.push_back(IPCameraDef("http://148.61.142.228/axis-cgi/mjpg/video.cgi", "username", "password"));
    
	ofLog(OF_LOG_NOTICE, "---------------Loading Streams---------------");
    
	ofxXmlSettings XML;
    
	if(XML.loadFile("streams.xml"))
    {
        
        XML.pushTag("streams");
        std::string tag = "stream";
		
		
        int nCams = XML.getNumTags(tag);
		
		for(std::size_t n = 0; n < nCams; n++)
        {
            
            IPCameraDef def(XML.getAttribute(tag, "name", "", n),
                            XML.getAttribute(tag, "url", "", n),
                            XML.getAttribute(tag, "username", "", n),
                            XML.getAttribute(tag, "password", "", n));
            
            
            std::string logMessage = "STREAM LOADED: " + def.getName() +
			" url: " +  def.getURL() +
			" username: " + def.getUsername() +
			" password: " + def.getPassword();
            
            ofLogNotice() << logMessage;
            
            ipcams.push_back(def);
            
		}
		
		XML.popTag();
    
	}
         
    else
    {
		ofLog(OF_LOG_ERROR, "Unable to load streams.xml.");
	}
        
    
	ofLog(OF_LOG_NOTICE, "-----------Loading Streams Complete----------");
    
    nextCamera = ipcams.size();
}

/*
//------------------------------------------------------------------------------
void testApp::videoResized(const void* sender, ofResizeEventArgs& arg)
{
    // find the camera that sent the resize event changed
    for(std::size_t i = 0; i < NUM_CAMERAS; i++)
    {
        if(sender == &grabbers[i])
        {
            std::stringstream ss;
            ss << "videoResized: ";
            ss << "Camera connected to: " << grabbers[i]->getURI() + " ";
            ss << "New DIM = " << arg.width << "/" << arg.height;
            ofLogVerbose("ofApp") << ss.str();
        }
    }
}
*/

//------------------------------------------------------------------------------
void testApp::update() {
	// update the cameras
   /* for(std::size_t i = 0; i < grabbers.size(); i++)
    {
        grabbers[i]->update();
    }
    */
    grabber->update();
    
    grabFrame = grabber->getPixelsRef();
	//if(grabbers[0].isFrameNew()) {

		blur(grabFrame, 10);
		contourFinder.findContours(grabFrame);
	//}
}


//------------------------------------------------------------------------------
void testApp::draw() {
	ofSetBackgroundAuto(showLabels);
	RectTracker& tracker = contourFinder.getTracker();
	
	if(showLabels) {
		ofSetColor(255);
		grabber->draw(0, 0);
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
	} else {
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
}


//------------------------------------------------------------------------------
void testApp::keyPressed(int key) {
	if(key == ' ') {
		showLabels = !showLabels;
	}
}