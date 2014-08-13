#pragma once


#include <string>
#include "ofTrueTypeFont.h"
#include "ofColor.h"
#include "ofRectangle.h"
#include "ofPoint.h"


class TextButton
{
public:
    TextButton();

    void setText(std::string newText);
	void setColor(const ofColor& newOnColor);
	void updateOffColor();
    void calculateRect();
    void checkMousePressed(const ofPoint& mouse);
    void update();
    void draw();

    ofRectangle bounds;
    ofPoint topLeftAnchor;
    string myDisplayText;
    string myText;
    string prefix;
    ofTrueTypeFont * font;
    ofTrueTypeFont * secondFont;
    ofRectangle rect;
    bool bSelectable;
    bool bDrawLong;

    string secondaryText;
    string deliminater;
    int maxWidth;

    bool bMouseOver;
    ofColor off;
    ofColor on;

};
