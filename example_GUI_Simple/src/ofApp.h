#pragma once

//#define COMMAND_LINE_ONLY

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxPG.h"
#include "ofxXmlSettings.h"
#include "TextButton.h"


class ofApp: public ofBaseApp
{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);


    string sketchName;
    string sketchPath;
    string addons;
    string platform;

    ofxPanel panelCoreAddons;
    ofxPanel panelOtherAddons;
    bool bHaveNonCoreAddons;

    ofxPanel panelPlatforms;

    ofxToggle osxToggle, iosToggle, wincbToggle, winvsToggle, linuxcbToggle, linux64cbToggle;

    ofTrueTypeFont font;
    ofTrueTypeFont titleFont;
    ofTrueTypeFont secondFont;

    int mode;
    enum { MODE_NORMAL, MODE_ADDON, MODE_PLATFORM };

    ofx::PG::BaseProject* project;

    string setupForTarget(int targ);

    void generateProject();

    string addonsPath;
    string status;

    ofxXmlSettings XML;
    string appToRoot;
    string defaultLoc;
    
    float uiLeftX; 


    TextButton  button;
    TextButton  generateButton;
    TextButton  addonButton;
    std::vector<TextButton> buttons;

    bool isAddonCore(string addon);
    bool bInited;
    vector < string  > coreAddons;

    
    float statusSetTime;
    float statusEnergy;
    void setStatus(string newStatus);
    
    ofImage logo;
    void addAddon(string addon);
    
};
