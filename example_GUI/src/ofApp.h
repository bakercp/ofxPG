#pragma once

//#define COMMAND_LINE_ONLY

#include "ofMain.h"
#include "ofxPG.h"

#ifndef COMMAND_LINE_ONLY
#include "ofxGui.h"
#endif

class ofApp: public ofBaseApp
{
public:
    void setup();
    void draw();

    void keyPressed(int key);
    void windowResized(int w, int h);

    void setupForTarget(int targ);
    void generateExamplesCB();
    void generateExamples();
    
    ofFileDialogResult makeNewProjectViaDialog();
    ofFileDialogResult updateProjectViaDialog();

    void createProjectPressed();
    void updateProjectPressed();
    void createAndOpenPressed();
    void changeOFRootPressed();
    
    void setupDrawableOFPath();
    
    ofx::PG::BaseProject* project;

    string projectPath;
    string target;
    vector <int> targetsToMake;
    bool buildAllExamples;

#ifndef COMMAND_LINE_ONLY
    string drawableOfPath;
    ofRectangle ofPathRect;
    ofPoint ofPathDrawPoint;

    ofxPanel panelAddons, panelOptions;
    ofxButton createProject, updateProject, createAndOpen, changeOFRoot;

    ofxPanel examplesPanel;
    ofxToggle osxToggle, iosToggle, wincbToggle, winvsToggle, linuxcbToggle, linux64cbToggle,linuxarmv6lcbToggle,linuxarmv7lcbToggle;
    ofxButton generateButton;
#endif
};
