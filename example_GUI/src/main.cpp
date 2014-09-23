#include "ofApp.h"


int main()
{
	ofSetupOpenGL(1024, 768, OF_WINDOW);
	ofRunApp(std::shared_ptr<ofApp>(new ofApp()));
}
