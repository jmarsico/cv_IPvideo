#H1
cv_IPvideo
==========

openframeworks ofxCv with ofxIpVideoGrabber... initial prototype for traffic cam analyzation. 

Research for Cognitive Video Class
Professor Yang Cai
Electrical and Computer Engineering Department
Carnegie Mellon University 2014


The goal of this application is to pull video from city-owned traffic cameras, analyze that video and visualize 
that data on an ultra-low-resolution display (most likely an architectural-scale strip of LED panels).

This code relies on Kyle McDonald's [ofxCV](https://github.com/kylemcdonald/ofxCv) wrapper for openCV as well as Christopher Baker's [ofxIpVideoGrabber](https://github.com/bakercp/ofxIpVideoGrabber), which
allows us to pull video from most any IP camera.

Future iterations will also communicate with lighting hardware, either through ArtNet, DMX via arduino, or possibly
Philip's Color Kinetics proprietary protocol KiNet. Possibly leveraging [this](https://github.com/Dewb/alphadep)
