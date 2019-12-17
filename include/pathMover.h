#ifndef PATHMOVER_H_INCLUDED
#define PATHMOVER_H_INCLUDED

#include "Leg_types/Leg.h"
#include "collider/mvHit.h"
#include "spriteSheet.h"

struct velPeriod
{
    vec2d v0;
    vec2d a;
    float g = 1.0f;// sliding acceleration
    float period;
    velPeriod* next = nullptr;
    char moveType = 'L';// 'l'=linear, 'C'=circular

    velPeriod(){}
    void init( vec2d v_0, vec2d acc, float Period, char MoveType, velPeriod* pNext = nullptr ) { v0 = v_0; a = acc; period = Period; moveType = MoveType; next = pNext; }
    velPeriod( vec2d v_0, vec2d acc, float Period, char MoveType, velPeriod* pNext = nullptr ) { init( v_0, acc, Period, MoveType, pNext ); }
    void init( std::ifstream& fin ) { fin >> moveType >> v0.x >> v0.y >> a.x >> a.y >> g >> period; }
    velPeriod( std::ifstream& fin ) { init(fin); }

    velPeriod* updateVel( vec2d& v_curr, vec2d& v_asg, float& timer, float dt );
};

velPeriod* makeVelPath( std::ifstream& fin );
int destroyVelPath( velPeriod*& pv );

class pathMover
{
    public:
    static float Fk, Fd;// restore and damping force coeffs

    const Leg* pLeg = nullptr;
    mvHit* pMH = nullptr;
    float s = 0.0f;// position on Leg
    float v = 0.0f;// speed on Leg
    vec2d oldPos;// position last frame. For finding velocity
    char mode = 'r';// r = rigid, 'd' = damped oscillator

    velPeriod* pVelPer = nullptr;

    pathMover(){}
    virtual ~pathMover(){}
    void init( Leg* p_Leg, mvHit* p_MH, float vel, float s0, bool matchVel = true );
    pathMover( Leg* p_Leg, mvHit* p_MH, float vel, float s0, bool matchVel = true ) { init( p_Leg, p_MH, vel, s0, matchVel ); }
    virtual void setState( float _s, float _v, void* ptr )
    { s = _s; v = _v; pLeg = (Leg*)ptr; pMH->setPosition( vec2d(pLeg->x(s), pLeg->y(s)) ); oldPos = pMH->pos; }
    virtual void update( float dt );
    virtual void update( float dt, vec2d grav );
    virtual state_ab* newState() { return new state_ffvp( &s, &v, (void**)(&pLeg) ); }
    virtual void draw( sf::RenderTarget& RT ) const { if( pMH ) pMH->draw(RT); }
};

class aniMover : public pathMover
{
    public:
    spriteSheet* pSS = nullptr;
    size_t setNum = 0, frIdx = 0;
    bool loopSet = true;// new
    bool showMvHit = true;
    sf::Sprite spr;

    aniMover(){}
    aniMover( Leg* p_Leg, mvHit* p_MH, float vel, float s0, bool matchVel = true ): pathMover( p_Leg, p_MH, vel, s0, matchVel ) {}
    virtual ~aniMover(){}
    virtual void init2( spriteSheet* p_SS, size_t set_Num, float sclFctr = 1.0f  );
    virtual void setState( float _s, float _v, void* ptr )
    { s = _s; v = _v; pLeg = (Leg*)ptr; pMH->setPosition( vec2d(pLeg->x(s), pLeg->y(s)) ); spr.setPosition( pMH->pos.x, pMH->pos.y ); oldPos = pMH->pos; }
    virtual void update( float dt );
    virtual void update( float dt, vec2d grav );
    virtual state_ab* newState() { return new state_ffvp( &s, &v, (void**)(&pLeg), std::bind( &aniMover::setState, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ) ); }
    virtual void draw( sf::RenderTarget& RT ) const { if( showMvHit ) pathMover::draw(RT); if( pSS ) RT.draw(spr); }

    // special animation funcs
    void setFrames( size_t SetNum, bool LoopSet, size_t FrIdx = 0 );
    bool incrFrame();// increment
    bool decrFrame();// decrement
    bool incrSet();
    bool decrSet();
    sf::IntRect getFrRect()const { return pSS->getFrRect( frIdx, setNum ); }
    void setPosition( float X, float Y );
};

class target : public aniMover
{
    public:
    static int* pKillCount;
    bool inUse = true;
    float strength = 20.0f, i_strength = 20.0f;
    size_t setN, setH, setK;// normal hit and kill animation frame sets
    float tLaunch = -1.0f;// if > 0 will update to inUse = true

    // health bar
    sf::Vector2f HBofst;
    float HBwidth = 20.0f, HBheight = 6.0f, HBborder = 2.0f;
    static float HBtimeLimit;
    float HBtime;// HB display time
    sf::Vertex HBvtx[8];// 2 Quads

    target(){}
    target( Leg* p_Leg, mvHit* p_MH, float vel, float s0, bool matchVel = true ): aniMover( p_Leg, p_MH, vel, s0, matchVel )
    { HBtime = HBtimeLimit+1.0f; HBofst.x = 0.0f; HBofst.y = -20.0f; for( auto& vtx : HBvtx ) vtx.color = sf::Color::Red; }
    virtual void init2( spriteSheet* p_SS, size_t set_Num, float sclFctr = 1.0f  )
    { aniMover::init2( p_SS, set_Num, sclFctr ); setN = setH = set_Num; setK = p_SS->numSets(); }
    virtual target* clone() const { target* pTgt = new target(*this); pTgt->pMH = pTgt->pMH->clone(); return pTgt; }
    virtual ~target(){}
    virtual void draw( sf::RenderTarget& RT ) const
    { if( inUse || (pSS && frIdx < pSS->getFrCount( setNum )) ) { aniMover::draw(RT); if( HBtime < HBtimeLimit && strength > 0.0f ) RT.draw( HBvtx, 4, sf::Quads ); } }
 //   virtual void draw( sf::RenderTarget& RT ) const { if( inUse ) aniMover::draw(RT); }
    virtual void update( float dt );// { if( inUse ) aniMover::update(dt); }
    virtual void update( float dt, vec2d grav );// { if( inUse ) aniMover::update(dt,grav); }
  //  void reset( float Power ) { strength = i_strength = Power; inUse = true; pMH->v *= 0.0f; setNum = setN; frIdx = 0; HBtime = HBtimeLimit+1.0f; }
    void reset( Leg* p_path, float t_launch = -1.0f );
    void reset( Leg* p_path, velPeriod* vp, vec2d pos, float t_launch = -1.0f );
    void onHit( float damage );
    void setHBpos();
};

struct firePattern1
{
    std::vector<vec2d> velVec;// velocity for each shot
    std::vector<float> delayVec;// time until next shot

    bool update( float dt, size_t& idx, float& timer, vec2d& vShot, vec2d* pTgtDir = nullptr );// fire shot when true
    bool init( std::ifstream& fin );// pattern data from file
};

void makeCircleFP( firePattern1& FP, size_t Nshots, float vMag, float dt, float dtLong );

class ship : public target
{
    public:
    vec2d shotOfst;
    firePattern1* pFirePat = nullptr;
    static mvHit* pFireMh;
    size_t fp_idx = 0;
    float fp_time = 0.0f;
    bool fireOnTarget = false;
    std::function<mvHit*( vec2d, vec2d, float )> pFireFunc = nullptr;

    ship(){}
    ship( Leg* p_Leg, mvHit* p_MH, float vel, float s0, bool matchVel = true ): target( p_Leg, p_MH, vel, s0, matchVel ) {}
    virtual target* clone() const { ship* pShip = new ship(*this); pShip->pMH = pShip->pMH->clone(); return pShip; }
    virtual ~ship(){}

    virtual void update( float dt );// shots fired
};

#endif // PATHMOVER_H_INCLUDED
