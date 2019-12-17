#ifndef SLIDEBAR_H
#define SLIDEBAR_H

//#include<sstream>
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
 //   sf::Vertex marks[6];
    sf::Vertex valBox[5];// for linesStrip around current value

    sf::RectangleShape sldButt;
    sf::Text valStr;
    sf::Text minValStr, maxValStr;
    float wButt = 40.0f;// so hButt = sz.y - 2*b
    float grabFactor = 1.7f;// new. width of hold as multiple of wButt
    float b = 3.0f;// button border thickness
    float L = 50.0f;// travel distance of button.
    bool isHoriz = true;// false if W < H
    bool showNums = true;

    slideBar() {}
    virtual ~slideBar() {}
    void init( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, float* p_val, float Wbutt = -1.0f );
    void init( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, std::function<void(float)> p_SetFunc, float Wbutt = -1.0f  );

    slideBar( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, float* p_val, float Wbutt = -1.0f )
    { init( x, y, W, H, Title, val_min, val_max, init_val, p_val, Wbutt ); }
    slideBar( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, std::function<void(float)> p_SetFunc, float Wbutt = -1.0f  )
    { init( x, y, W, H, Title, val_min, val_max, init_val, p_SetFunc, Wbutt ); }


    virtual bool hitLeft();// the magic happens here
    virtual void draw( sf::RenderTarget& rRW )const;
    virtual bool MseOver();
    virtual bool hit()const;// grab width applied
    virtual void setPosition( sf::Vector2f Pos );

    void set_fVal( float f_val );

 //   void set_fVal();// fVal already assigned
 //   void set_fVal( float s );// position on strip given, with fVal also assigned
 //   float find_s();// from mouse position

};

float float_to_Text(sf::Text& strArg, float x);// returns position of end of strArg
bool hitR( const sf::RectangleShape& RS );

#endif // SLIDEBAR_H
