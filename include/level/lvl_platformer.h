#ifndef LVL_PLATFORMER_H_INCLUDED
#define LVL_PLATFORMER_H_INCLUDED

#include "Level.h"
#include "../pathUtils.h"
#include "../sfmlUtils.h"
#include "../collider/ball_go.h"
#include "../pathMover.h"
#include "platformExtras.h"

class lvl_platformer : public Level
{
    public:
    // data
    sf::Text msg1, msgNum1;// setNum
    sf::Text msg2, msgNum2;// frame number
    sf::Text bankMsg, bankMsgNum;// frame number
    float bankBal = 0.0f;
    std::vector<sf::Text> msgVec;
    sf::Text helpMsg;
    bool showMsgs = false;
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
 //   linLeg ladderLeg;// ladder Leg
    std::vector<linLeg> ladderVec;

    float worldWidth = mvHit::wdwW, worldHeight = mvHit::wdwH;
    float old_wdwW = mvHit::wdwW, old_wdwH = mvHit::wdwH;
    vec2d iPos_viewCtr;// to find mouse position offset in handleEvent()

    ball_go B1;// spacemans collision ball
    aniMover platGuy;// the spaceman

    // for mvHits on paths
    Leg *pPath = nullptr;
 //   std::vector<mvHit*> pMH_pathVec;// mvHit storage for below
 //   std::vector< pathMover* > pMoverVec;// on paths
    std::vector< target* > pTgt_pathVec;// owning the mvHits pointed to
    std::vector< shot* > pShotVec;// owning
    size_t numLoops = 3;
    float shotVel = 40.0f;
    float shotStepSz = 0.8f;
    vec2d shotGrav;// gravity for shots
    spriteSheet ship_SS;

 //   spriteSheet fire_SS;
 //   ball fireBall;
 //   aniMover fireAM;

    spriteSheet spaceManSS;
    spriteSheet platformSS;
    std::vector<quadPack> qpVec;

    // items
    std::vector<pickItem> pickVec;

    // controls
 //   buttonList platGuyControl;// 6 click strips
 //   sf::RectangleShape PGCbkRect;
 //   buttonList shotControl;// 3 click strips: radius, speed, gravity
 //   sf::RectangleShape shotCbkRect;

    buttonList multiControl;// platGuy + shot + path
    std::vector< buttonList* > pListVec;
    std::vector< sf::RectangleShape > listBK_rectVec;

//    size_t numReps = 3;// logic loops per render. Not in use

    // functions
//    lvl_platformer():Level(), ladderLeg(0,0,1,1) {}
    lvl_platformer():Level() {}

    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void draw_map( sf::RenderTarget& RT ) const;
    virtual void cleanup();
    virtual ~lvl_platformer();

    // ladder use
    void climb_onOff();
    void PG_update( float dt );
    void PG_hit();
    void PG_step( float dt );// frame rate to running speed

    void assignMessages();
    bool loadPlatforms();
    int initPathMovers(void);
    int initShots(void);
    bool fireShot( vec2d pos, vec2d dir, float speed );
    void shots_hit_allMvHits( float dt );
    bool isNearestMvHit( shot* pShot, mvHit* pMH, mvHit*& pNearMH, float& stepsMin, float& sepMag, float& vt, vec2d shotVu );
};

#endif // TEST1_OBJ_H_INCLUDED
