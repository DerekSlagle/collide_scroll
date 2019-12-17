#ifndef LVL_PATH_H_INCLUDED
#define LVL_PATH_H_INCLUDED

#include "Level.h"

#include "../pathMover.h"
#include "../pathUtils.h"
#include "../sfmlUtils.h"

struct hitSpot
{
    sf::CircleShape dot;// so you can see it
    Leg* pLeg = nullptr;
    float s0;
    vec2d pt;// hit point for mvHit
    hitSpot( Leg* path, float s, sf::Color dotClr, float Rdot );
    void place( pathMover* pMover ) const;
};

class lvl_path : public Level
{
    public:
    size_t launchIdx = 0;// to support launching a mvHit
    vec2d gravity;
    bool gravity_on = false;

    // for mvHits on paths
    Leg *pPath = nullptr, *pPath2 = nullptr;// a new one for mvHits used with pathMovers
    std::vector<mvHit*> pMH_pathVec;// path for mvHits used with pathMovers
    std::vector< pathMover* > pMoverVec;// a new one for mvHits used with pathMovers
    std::vector<sf::Vertex> pathVtxVec, pathVtxVec2;
    std::vector<hitSpot> hitSpotVec;
    buttonList pathControl;
    buttonRect hitSpotButt;
    std::vector<sf::Text> msgVec;

    spriteSheet bkgd_SS;
    spriteSheet ship_SS;
    sf::Vertex Qvtx[4];
    quadPack QdPack1;

    // functions
    lvl_path():Level() { std::cout << "Hello from lvl_path ctor\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    virtual void cleanup();
    virtual ~lvl_path() { cleanup(); }

    // special
    int initPathMovers(void);
    void init_rec( std::vector<state_ab*>& p_stateVec );
    void assignMessages();
};

#endif // LVL_PATH_H_INCLUDED
