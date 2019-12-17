#ifndef BUTTONVALONHIT_H_INCLUDED
#define BUTTONVALONHIT_H_INCLUDED

#include "buttonRect.h"
//#include<sstream>
#include<iostream>
//#include<functional>

class buttonValOnHit : public buttonRect
{
    private:
    void setFval();// fVal already assigned
    void setFval( float s );// position on strip given, with fVal also assigned
    float find_fVal();// from mouse position
    float find_fVal( float& s );// from mouse position. Position along strip written to s
    public:
        // inherited
   //     sf::Vector2f sz;
   //     sf::RectangleShape R[4];
   //     sf::Text title;
        // static members
        static bool mseOverAny_delay;
   //     static sf::Text moValtxt;// real time value
    //    static sf::Vertex moValBox[4];// Quad as bkgd
        static sf::Vector2f moValBoxSz;
    //    const  static sf::Font* pFont;// about time?
     //   static int moDelay;// # frames to trigger drawing this.
     //   static int delayCnt;
    //    static void update();// { static int delayCnt = 0; if( ++delayCnt > moDelay ) { delayCnt = 0; /* mo dependent stuff */ } return; }
    //    static void init( const sf::Font& rFont, int mo_delay );
        // unique
        sf::Vertex marks[6];// Marks at left, right ends and current value
        sf::Vertex valBox[5];// for linesStrip around current value
        bool drawValBox = true;
        sf::Text txtVal, txtVal_min, txtVal_max;// numeric labels
  //      bool is_float;// also supporting discrete int values
        bool is_horiz = true;// work for vertical too!
        float fVal = 0.0f, fVal_min = 0.0f, fVal_max = 0.0f;
   //     int iVal = 0, iVal_min = 0, iVal_max = 0;

        float capLen = 5.0f;
        float valLen = 1.0f;// valLen + 2*capLen = long button dimension ie max( sz.x, sz.y )
   //     void* pVal = nullptr;// if to write value chosen directly - cast acrdng to is_float
        float* pVal = nullptr;// if to write value chosen directly - cast acrdng to is_float
        std::function<void(float)> pSetFunc;

        // funcs
        buttonValOnHit() {}// default = invalid object
        virtual ~buttonValOnHit() {}

        virtual bool hitLeft();// the magic happens here
        virtual void draw( sf::RenderTarget& rRW )const;
        virtual bool MseOver();
        virtual void setPosition( sf::Vector2f Pos );
        virtual void init_delayBox();
        void set_fVal( float f_val );
        virtual void update();

     //   void init( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, bool isFloat, void* p_val = nullptr );
        void init( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, float* p_val );

        void init( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, std::function<void(float)> p_SetFunc )
        { pSetFunc = p_SetFunc; init( x, y, W, H, Title, val_min, val_max, init_val, nullptr ); }
     //   buttonValOnHit( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, bool isFloat, void* p_val = nullptr )
        buttonValOnHit( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, float* p_val )
        { init( x, y, W, H, Title, val_min, val_max, init_val, p_val ); }

        buttonValOnHit( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, std::function<void(float)> p_SetFunc )
        { init( x, y, W, H, Title, val_min, val_max, init_val, p_SetFunc ); }
    //    { init( x, y, W, H, Title, val_min, val_max, init_val, isFloat, p_val ); }

};

/*    template<class T>
    float to_SF_string(sf::Text& strArg, T x)// returns position of end of strArg
    {
        std::stringstream ss;
        ss.precision(4);
        ss << x;
        std::string tempStr;
        ss >> tempStr;
        strArg.setString( tempStr.c_str() );
        sf::FloatRect fr = strArg.getLocalBounds();
        return fr.left + fr.width;
    }   */

#endif // BUTTONVALONHIT_H_INCLUDED
