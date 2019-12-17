#ifndef LVL_PLATFORMER_H_INCLUDED
#define LVL_PLATFORMER_H_INCLUDED

#include "Level.h"
#include "../pathUtils.h"
#include "../sfmlUtils.h"
#include "../collider/ball_go.h"
#include "../pathMover.h"

class lvl_platformer : public Level
{
    public:
    // data
    sf::Text msg1, msgNum1;// setNum
    sf::Text msg2, msgNum2;// frame number
    // map frame numbers
    size_t setRunLt = 0, setRunRt = 0, setStill = 0;
    size_t frFaceLt=0, frFaceRt=0, frJumpLt=0, frJumpRt=0;
    size_t frHitLt=0, frHitRt=0, frCrouchLt=0, frCrouchRt=0;// for later use

    // platform guy
    float maxSpeed = 0.0f;// running speed = B1.maxVel
    vec2d gravity;// declare near point of 1st use!
    bool gravity_on = false;
    // jump
    bool jump = false;
    float jumpSpeed = 0.0f;
    float bounceAmp = 1.0f;// *= Cr for side jump amplified bounce
    // climb
    int climbState = 0;// -1,0,+1 scheme. Down, stopped, up
    float climbSpeed = 0.5f;
    linLeg ladderLeg;// ladder Leg

    sf::View mapView;
    bool bigView_x = false;// scrolling to watch for?
 //   int scrollState = 0;// stationary -1=left, +1=right
    float worldWidth = mvHit::wdwW, worldHeight = mvHit::wdwH;
    float old_wdwW = mvHit::wdwW, old_wdwH = mvHit::wdwH;
    vec2d iPos_viewCtr;// to find mouse position offset in handleEvent()

    ball_go B1;// spacemans collision ball
    aniMover platGuy;// the spaceman
//    float dStep = 0.0f;// future use: advance run ani frame at rate proport. to run speed
    spriteSheet spaceManSS;
    spriteSheet platformSS;
    std::vector<quadPack> qpVec;
    std::vector<segHit*> pCBvec;

    size_t numReps = 3;// logic loops per render

    // functions
    lvl_platformer():Level(), ladderLeg(0,0,1,1) {}

    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update();
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void draw_map( sf::RenderTarget& RT ) const;
    virtual void cleanup();
    virtual ~lvl_platformer();

    // ladder use
    void climb_onOff();
    void PG_update();
    void PG_hit();
    void PG_step();// frame rate to running speed
};

#endif // TEST1_OBJ_H_INCLUDED
