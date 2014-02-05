#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "IPVideoGrabber.h"
#include "ofxXmlSettings.h"


#if defined(TARGET_OF_IPHONE) || defined(TARGET_ANDROID) || defined(TARGET_LINUX_ARM)
#define NUM_CAMERAS 1
#define NUM_ROWS 1
#define NUM_COLS 1
#else
#define NUM_CAMERAS 9
#define NUM_ROWS 3
#define NUM_COLS 3
#endif


class IPCameraDef
{
public:
    IPCameraDef()
    {
    }
    
    IPCameraDef(const std::string& url): _url(url)
    {
    }
    
    IPCameraDef(const std::string& name,
                const std::string& url,
                const std::string& username,
                const std::string& password):
    _name(name),
    _url(url),
    _username(username),
    _password(password)
    {
    }
    
    //set and get name
    void setName(const std::string& name) { _name = name; }
    std::string getName() const { return _name; }
    
    //set and get URL
    void setURL(const std::string& url) { _url = url; }
    std::string getURL() const { return _url; }
    
    //set and get user name
    void setUsername(const std::string& username) { _username = username; }
    std::string getUsername() const { return _username; }
    
    //set and get password
    void setPassword(const std::string& password) { _password = password; }
    std::string getPassword() const { return _password; }
    
    
private:
    std::string _name;
    std::string _url;
    std::string _username;
    std::string _password;
};


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
	ofVideoPlayer movie;	
	ofxCv::ContourFinder contourFinder;
	bool showLabels;
    
    
    //IPgrabber declarations
    //std::vector<SharedIPVideoGrabber> grabbers;
    SharedIPVideoGrabber grabber;
    ofImage grabFrame;
    void loadCameras();
    IPCameraDef& getNextCamera();
    std::vector<IPCameraDef> ipcams; // a list of IPCameras
    int nextCamera;
    
    // This message occurs when the incoming video stream image size changes.
    // This can happen if the IPCamera has a single broadcast state (some cheaper IPCams do this)
    // and that broadcast size is changed by another user.
   // void videoResized(const void* sender, ofResizeEventArgs& arg);

};
