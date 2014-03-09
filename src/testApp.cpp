/*
 <streams>
 
 <stream url="http://148.61.142.228/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 **<stream url="http://82.79.176.85:8081/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 <stream url="http://81.8.151.136:88/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 <stream url="http://216.8.159.21/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 **<stream url="http://kassertheatercam.montclair.edu/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
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
    
    
    //initialize connection
    grabber = IPVideoGrabber::makeShared();
    
    while(grabber->isConnected()==false)
    {
        ofLog() << "connecting...";
        grabber->setCameraName("cam1");
        grabber->setURI("http://82.79.176.85:8081/axis-cgi/mjpg/video.cgi?resolution=320x240");
        grabber->connect();
        
    }
    
    ofLog() << "connection made!";
    ofLog() << "width: " << grabber->getWidth();
    ofLog() << "height: " << grabber->getHeight();
    
        
    gui.setup("panel");
    gui.add(backgroundThresh.setup("bgThresh", 21, 0, 255));
    gui.add(minArea.setup("minArea", 10,1, 100));
    gui.add(maxArea.setup("maxArea", 100, 101, 200));
    gui.add(blurAmount.setup("blur", 10, 1, 100));
    gui.add(erodeAmount.setup("erode", 0, 0, 10));
    gui.add(dilateAmount.setup("dilate", 0, 0, 10));
    gui.add(learningTime.setup("learnTime", 50, 30, 2000));
    gui.add(reset.setup("reset background"));
    gui.add(yLoc.setup("yLoc", 270, 0, 288));
    gui.add(boxSize.setup("box size", 58, 10, 100));
    gui.add(bShowGrid.setup("showGrid",true));
    
    gui.setPosition(ofGetWidth()-275, ofGetHeight()-375);
    
    thresholded.allocate(grabber->getWidth(), grabber->getHeight(), OF_IMAGE_COLOR);
	
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
	
	bShowVideos = false;
    
    numLEDs = 20;
    
    for(int i = 0; i < numLEDs; i++)
    {
        brights.push_back(0);
    }
    
    
     
}



//------------------------------------------------------------------------------
void testApp::update() {
	// update the cameras
    grabber->update();
    ofLog() << "width: " << grabber->getWidth();
    ofLog() << "height: " << grabber->getHeight();
    
    contourFinder.setMinAreaRadius(minArea);
    contourFinder.setMaxAreaRadius(maxArea);
    background.setLearningTime(learningTime);
    background.setThresholdValue(backgroundThresh);
    
    
    int erodeNum = erodeAmount;
    int dilateNum = dilateAmount;
    
    if(reset)
    {
        background.reset();
    }
    
    
    
    grabFrame.setFromPixels(grabber->getPixels(), grabber->getWidth(), grabber->getHeight(), OF_IMAGE_COLOR);
    grabFrame.resize(384, 288);
    
    
    // backgroundThresh = ofMap(mouseX, 0, ofGetWidth(), 0, 255);
    if(backgroundThresh < 1){
        backgroundThresh = 0;
    }if(backgroundThresh > 254){
        backgroundThresh = 255;
    }
    
    //get the thresholded frame from background function
    background.update(grabFrame, thresholded);
    thresholded.update();
    
    //manipulate thresholded image before contour finder
    dilate(thresholded, dilateNum);
    erode(thresholded, erodeNum);
    blur(thresholded, blurAmount);
    
    
    contourFinder.findContours(thresholded);
    ofLog() << "number contours: " << contourFinder.size();
    
    
    numLEDs = 20;
    cellSize = grabber->getWidth()/numLEDs;
    
    
    if(grabber->isFrameNew())
    {
        for(int i = 0; i < numLEDs; i++)
        {
            int shift = i*cellSize;
            int total = 0;
            for(int x = shift; x < cellSize+shift; x++)
            {
                for(int y = yLoc; y < cellSize+yLoc; y++)
                {
                    total = total + thresholded.getColor(x, y).getBrightness();
                    
                }
            }
            
            brights[i]  = (total / (cellSize * cellSize)) * 2;
            brights[i] = ofClamp(brights[i], 0, 255);
            ofLog() << i << ": " << brights[i];
        }
    }

    
  
}


//------------------------------------------------------------------------------
void testApp::draw() {
    
    
    if(bShowVideos){
        gui.draw();
        RectTracker& tracker = contourFinder.getTracker();
        ofSetColor(255);
        
        
        ofPushMatrix();
            ofTranslate(10, 10);
            grabFrame.draw(0,0); //draw the video from the ofImage
            thresholded.draw(grabFrame.getWidth()+10, 0);  //draw the binary mask next to it
        
            if(bShowGrid)
            {
                ofSetColor(0,128);
                for(int i = 0; i< numLEDs*cellSize; i+=cellSize)
                {
                    ofLine(i,0,i,grabber->getHeight());
                    if(i < grabber->getHeight())
                    {
                        ofLine(0,i,grabber->getWidth(),i);
                    }
                }
                ofSetColor(255,0,0,40);
                ofRect(0, yLoc, grabber->getWidth(), cellSize);
            }
            ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, grabber->getHeight()+20);
        ofPopMatrix();

        
        
        ofSetColor(0, 244, 0);
        ofPushMatrix();
            ofTranslate(0,10);
            contourFinder.draw();
            for(int i = 0; i < contourFinder.size(); i++) {
                ofPoint center = toOf(contourFinder.getCenter(i));
                ofPushMatrix();
                    ofSetColor(255,0,0);
                    ofTranslate(center.x, center.y);
                    int label = contourFinder.getLabel(i);
                    string msg = ofToString(i) + ":" + ofToString(tracker.getAge(label));
                    ofDrawBitmapString(msg, 0, 0);
                    ofVec2f velocity = toOf(contourFinder.getVelocity(i));
                    ofScale(5, 5);
                    ofLine(0, 0, velocity.x, velocity.y);
                ofPopMatrix();
            
        }
        ofPopMatrix();
           
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
            ofDrawBitmapString(reportStr.str(), 20, 600);
        
        ofDrawBitmapString("Feed + Contour Tracking", 0, 500);
        ofDrawBitmapString("Background Subtraction", 650, 500);
    }
    
    
    ofPushMatrix();
    ofTranslate(boxSize, ofGetHeight()/2 - boxSize);
    for(int i = 0; i < numLEDs; i++)
    {
        ofPushMatrix();
        ofTranslate(boxSize*i, ofGetHeight()/2 - boxSize);
        ofSetColor(brights[i]);
        ofRect(0,0,boxSize, boxSize);
        ofDrawBitmapString(ofToString(i), 0,0);
        ofPopMatrix();
        
    }
    ofPopMatrix();
    
    
}


//------------------------------------------------------------------------------
void testApp::keyPressed(int key) {
	if(key == ' ')
    {
        bShowVideos = !bShowVideos;
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

