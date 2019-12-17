#ifndef LVLB_H_INCLUDED
#define LVLB_H_INCLUDED

#include "Level.h"

class lvlB : public Level
{
    public:
    std::vector<sf::Text> msgVec;

    vec2d pos0, dSep;
    size_t rows = 5, cols = 10;
    float Fk = 1.0f, Fd = 1.0f;// spring, damping consts
    bool isDamped = true;// both toggle w/F3
    bool isRestored = true;// toggle w/F4

    vec2d expPt;// explosion point = mseX,mseY on left click. R0 = 0 assigned too.
    float vWave = 10.0f;// and growth rate of R0
    float Reff = 30.0f;// half width of annular force region
    float Rfull = 100.0f;// force applied full until R0 > Rfull. Then inv. prop: *= Rfull/R0
    float R0 = winW + 1.0f;// expPt to wave center
    float Rmax = winW;// max distance from expPt

    float Fapp = 15.0f;// explosion force amplitude
 //   buttonValOnHit FkStrip, FdStrip;
 //   buttonValOnHit vWaveStrip, FappStrip, ReffStrip, RfullStrip;
    buttonList restoreForceControl, boomForceControl;

    // functions
    lvlB():Level() { std::cout << "lvlB ctor\n"; }// if( !init() ) std::cout << "bad init\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    virtual void cleanup();
    virtual ~lvlB() { cleanup(); }

    void init_rec( std::vector<state_ab*>& p_stateVec );
    void assignMessages();
};

#endif // LVLB_H_INCLUDED
