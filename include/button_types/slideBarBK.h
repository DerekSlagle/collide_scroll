#ifndef SLIDEBAR_H
#define SLIDEBAR_H

#include<sstream>
#include<string>
#include "buttonRect.h"

class slideBar : public buttonRect
{
    public:
 //   static bool held;
    static float grabX;

    float fVal, fVal_min, fVal_max;
    float iVal;
    float* pVal = nullptr;// will write val to *pVal
    std::function<void(float)> pSetFunc = nullptr;
    sf::Vertex marks[6];

    sf::RectangleShape sldButt;
    sf::Text valStr;
    sf::Text minValStr, maxValStr;
    float wButt = 40.0f;// so hButt = sz.y - 2*b
    float b = 3.0f;// button border thickness
    float L = 50.0f;// travel distance of button.

    slideBar() {}
    virtual ~slideBar() {}
    void init( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, float* p_val );

    virtual bool hitLeft();// the magic happens here
    virtual void draw( sf::RenderTarget& rRW )const;
    virtual bool MseOver();
    virtual void setPosition( sf::Vector2f Pos );

    void set_fVal();// fVal already assigned
    void set_fVal( float s );// position on strip given, with fVal also assigned
    float find_s();// from mouse position

};

float float_to_Text(sf::Text& strArg, float x);// returns position of end of strArg
bool hitR( const sf::RectangleShape& RS );

#endif // SLIDEBAR_H
