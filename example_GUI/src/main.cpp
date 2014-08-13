#include "ofApp.h"


int main()
{
	ofSetupOpenGL(1024, 768, OF_WINDOW);
    std::shared_ptr<ofApp> app = std::shared_ptr<ofApp>(new ofApp());
    app->buildAllExamples = false;
	ofRunApp(app);
}
