#ifndef LVL1_H_INCLUDED
#define LVL1_H_INCLUDED

#include "Level.h"

//#include "include/pathMover.h"
//#include "include/pathUtils.h"

#include "../pathMover.h"
#include "../pathUtils.h"

class lvl1 : public Level
{
    public:
    size_t launchIdx = 0;// to support launching a mvHit
    vec2d gravity;
    bool gravity_on = false;

    gun* pGun_1 = nullptr;// gun
    expandPolygon *pXpgA = nullptr;
//    waveSeg* p_wvSeg = nullptr;
//    lineSegConveyor* pLSconvey = nullptr;

    slideBar viewX, viewY;
    buttonList gunControl;
    floatSpot fSpot1, fspot2, fspot3;// 2x conveyor + pipe
    std::vector<floatSpot> fSpotVec;
    buttonValOnHit gravityStrip;
    // more colliders
    std::vector<mvHit*> pBulletVec;
    std::vector<waveTank> wvTankVec;
    std::vector<buttonList> wvTankControlVec;
    std::vector<waterPipe*> pPipeVec;

    // functions
    lvl1():Level() { std::cout << "Hello from lvl1 ctor\n"; }// if( !init() ) std::cout << "bad init\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void draw_map( sf::RenderTarget& RT ) const;

    virtual void cleanup();
    virtual ~lvl1() { cleanup(); }

    void init_rec( std::vector<state_ab*>& p_stateVec );
    mvHit* fireShot( vec2d pos, vec2d dir, float speed );
};

#endif // LVL1_H_INCLUDED
