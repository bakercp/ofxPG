#include "TextButton.h"
#include "ofGraphics.h"
#include "ofAppRunner.h"


TextButton::TextButton():
    bSelectable(true),
    bMouseOver(false),
    bDrawLong(true),
    deliminater("/")
{
}


void TextButton::setText(std::string newText)
{

    myText = newText;

    newText = prefix + newText;

    cout << newText << endl;
    vector < string > breakUp;
    breakUp = ofSplitString(newText, deliminater);

    ofPoint pos;
    pos.set(0,0);

    myDisplayText = "";

    for (int i = 0; i < breakUp.size(); i++){
        string text = breakUp[i];
        if (i != breakUp.size() -1) text += deliminater;
        //if (breakUp[i].length() == 0) text += deliminater;

        ofRectangle rect = font->getStringBoundingBox(text, pos.x, pos.y);
        if ((pos.x + rect.width) > 500){

            myDisplayText += "\n";
            myDisplayText += text;
            pos.x = rect.width;
        } else {

            myDisplayText+= text;
            pos.x += rect.width;
        }
        
        
    }
}


void TextButton::setColor(const ofColor& newOnColor)
{
    on = newOnColor;
    on.set(0,0,0);
    off.set(0,0,0);
    //updateOffColor();
}


void TextButton::updateOffColor()
{
    off.setHsb(0,0,0);
}


void TextButton::calculateRect()
{
    rect = font->getStringBoundingBox(myDisplayText, topLeftAnchor.x, topLeftAnchor.y);

    rect.x -= 12;
    rect.y -= 12;
    rect.width += 22;
    rect.height += 23;

}

void TextButton::checkMousePressed(const ofPoint& mouse)
{
    if (bSelectable == false)
    {
        bMouseOver = false;
        return;
    }

    if (rect.inside(mouse))
    {
        bMouseOver = true;
    }
    else
    {
        bMouseOver = false;
    }
}


void TextButton::draw()
{
    ofPushStyle();
    ofFill();

    if (bDrawLong == true)
    {
        ofSetColor(220,220,220);
        ofFill();
        ofRect(rect.x, rect.y, ofGetWidth() - rect.x*2, rect.height);

        ofRectangle rectString = secondFont->getStringBoundingBox(secondaryText, 0, 0);
        float h = rectString.height;
        float y = (rect.y + rect.height/2) + (rectString.height)/2;
        float x = (rect.x + ofGetWidth() - rect.x*2) - rectString.width - 10;

        ofFill();
        ofSetColor(0,0,0);
        secondFont->drawString(secondaryText, x,y);
        //button.secondaryText;

    }


    if( bSelectable ){
        if (bMouseOver == true) ofSetColor(140,140,140);
        else ofSetColor(0,0,0);
    }else{
        ofSetColor(160, 160, 160);
    }
    ofRect(rect);

    ofColor darkOutline = on;
    darkOutline *= 0.65;

    //ofSetColor(darkOutline);
    //ofNoFill();
    //ofRect(rect);

    //ofSetColor(ofColor::gray);
    //font->drawString(myText, topLeftAnchor.x+1, topLeftAnchor.y+1);

    ofSetColor(74,255,203);
    font->drawString(myDisplayText, topLeftAnchor.x, topLeftAnchor.y);


    ofPopStyle();
}
