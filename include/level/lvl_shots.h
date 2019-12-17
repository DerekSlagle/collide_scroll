#ifndef LVL_SHOTS_INCLUDED
#define LVL_SHOTS_INCLUDED

#include "Level.h"

#include "platformExtras.h"
#include "../pathUtils.h"
#include "../sfmlUtils.h"
#include "../collider/laser.h"
#include "../utility/flipDigit.h"


class lvl_shots : public Level
{
    public:
    std::vector< target* > pTargetVec;// owning the mvHits pointed to
    std::vector< shot* > pRegShotVec;// owning
    std::vector< shot* > pXtraShotVec;// owning
    std::vector< shot* > pShotVec;// all good shots
    std::vector< shot* > pBadShotVec;// all bad shots - owning

    std::vector<Leg*> pathVec;
    std::vector<Leg*> blockPathVec;// all 1 block path
    std::vector<size_t> NshipsVec;// # of ships on each path sz = pathVec.size()
    std::vector<std::string> pathNameVec;// # of ships on each path sz = pathVec.size()
    std::vector<linLeg> linLegVec;// singles for target entry
    buttonList pathControl;
    firePattern1 firePatt1;
    firePattern1 firePatt1b;
    buttonRect firePattButt;
    buttonRect circFPButt;
    buttonValOnHit dragStrip;
 //   buttonRect gotoMMButt;
 //   velPeriod* velPath = nullptr;
    std::vector< velPeriod* > velPathVec;
    std::vector<sf::Vertex> pathVtxVec;

    gun theGun;
    buttonList gunControl;
    buttonList pathSelect;
    size_t XtraShotIdx = 0;
    radioButton shotTypeList;
    // guided shot
    guidedShot* pGuidedShot = nullptr;// F6 cycles targets
    laser theLaser;

    spriteSheet ship_SS;// and shot boom animation

    float floorDrag = 0.4f;// friction arrests free target motion
    size_t numLoops = 3;
    // star field
    std::vector<std::vector<sf::Vertex>> starLayerVec;
    int starLayerPop[3];
    float starLayerVel[3];
    sf::FloatRect starArea;
 //   bool showStars = true;
    buttonRect starButt;// toggle star field display

    // body count
    static const size_t bodyCntDigits = 3;
    flipDigit bodyCnt[bodyCntDigits];
    spriteSheet flipDigit_SS;
    int numFlips = 0;// temp use. May be local static in update(). Using inhanleEvent to trigger multiple flips
 //   int lastTgtCnt = 0;// used to id kill events.

    // functions
    lvl_shots():Level() { std::cout << "Hello from lvl_shots ctor\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    virtual void cleanup();
    virtual ~lvl_shots() { cleanup(); }

 //   bool fireRegShot( vec2d pos, vec2d dir, float speed );
    int initTargetPath();
    void update_hit_shot( shot* pShot, float dt );
};

// regular funcs
bool willHitMvHit( shot* pShot, mvHit* pMh, float& stepFrac );


#endif // LVL_SHOTS_INCLUDED
