#ifndef LVL_INTERSECT_H_INCLUDED
#define LVL_INTERSECT_H_INCLUDED

#include "Level.h"
#include "..\Leg_types\moveAdapter.h"
#include "..\Leg_types\linLeg.h"

class lvl_Intersect : public Level
{
    public:
    size_t launchIdx = 0;// to support launching a mvHit
    vec2d gravity;
    bool gravity_on = false;
    bool doClone = false, doErase = false;

    buttonValOnHit dragStrip;
    float floorDrag = 0.4f;// friction arrests free target motion
    expandPolygon *pXpgA = nullptr;
    buttonRect hitButt;// toggle mvHit collision testing
    sf::Text isectMsg, isectIdx1Msg, isectIdx2Msg;
    bool anyIntersecting = false;

    buttonRect goButt;
    buttonValOnHit kStrip, tStrip;
    vec2d flingPos0;
    float kTime = 0.0f;

    // using moveAdapter + linLeg
    buttonRect goButt_2;
    linLeg maLeg[2];
    moveAdapter ma[2];
    moveAdapter* pMA = nullptr;// animating if valid
    // using ma w/o Leg to spin
    buttonRect spinButt;

    // functions
    lvl_Intersect():Level() { std::cout << "Hello from lvl_Intersect ctor\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    virtual void cleanup();
    virtual ~lvl_Intersect() { cleanup(); }

    void resizeHistory( std::vector<bool>& isVec, mvHit* pMH );
//    void kMove( float dt );
    void maMove( float dt );
    void maSpin( float dt );
};

#endif // LVL_INTERSECT_H_INCLUDED
