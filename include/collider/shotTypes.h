#ifndef SHOTTYPES_H_INCLUDED
#define SHOTTYPES_H_INCLUDED

#include "../collider/ball.h"
#include "../pathMover.h"

class shot : public ball
{
    public:
    static float minVelSq;
    bool inUse = false;
    float damage = 3.0f;

    // death animation
    spriteSheet* pSS = nullptr;
    size_t setNum = 0, frIdx = 0;
    sf::Sprite spr;

    // functions
    virtual bool hit( target& tgt );

    shot(){}
    virtual ~shot(){}
    virtual void draw( sf::RenderTarget& RT ) const
    {
        if( inUse ) ball::draw(RT);
        else if( pSS && frIdx < pSS->getFrCount( setNum ) ) RT.draw(spr);
    }
    virtual void update( float dt );
    virtual bool hit( mvHit& mh ) { if( inUse ) return ball::hit(mh); return false; }
    virtual bool onFire()
    {
        if( inUse ) return false;
        inUse = true;
        frIdx = pSS->getFrCount( setNum );// to block death animation
        return true; // true if shot is available to be fired
    }

    virtual mvHit* clone() const { return new shot(*this); }

    virtual void init( std::istream& fin ) { ball::init(fin); }
    shot( std::istream& fin ) { init(fin); }

    void init2( spriteSheet* p_SS, size_t set_Num, float sclFctr = 1.0f );
    shot( spriteSheet* p_SS, size_t set_Num, float sclFctr = 1.0f ) { init2( p_SS, set_Num, sclFctr ); }

    static mvHit* fireShot( std::vector<shot*>& pShotVec, vec2d pos, vec2d dir, float speed );
    static mvHit* fireShot( std::vector<mvHit*>& pMhVec, vec2d pos, vec2d dir, float speed );
};

class guidedShot : public shot
{
    public:
    float a_t = 0.0f, a_p = 1.0f;
    target* pTgt = nullptr;
 //   bool onFire( target* p_tgt ) { if( shot::onFire() ) { pTgt = p_tgt; return true;} return false; }
    virtual void update( float dt );
    virtual void init( std::istream& fin ) { ball::init(fin); if( fin ) fin >> a_t >> a_p; }
    virtual bool hit( target& tgt );// assign pTgt = nullptr on kill
};

class clusterShot : public shot
{
    public:
    float vFrags = 30.0f;
    size_t numFrags = 7;
    float boomDelay = 100.0f, boomTime = 0.0f;
    std::function<mvHit*( vec2d, vec2d, float )> p_FireFunc = nullptr;
 //   bool onFire( target* p_tgt ) { if( shot::onFire() ) { pTgt = p_tgt; return true;} return false; }
    virtual void update( float dt );
    virtual void init( std::istream& fin ) { ball::init(fin); if( fin ) fin >> numFrags >> vFrags >> boomDelay; }
    virtual bool hit( target& tgt );// assign pTgt = nullptr on kill
    virtual bool onFire() { if( shot::onFire() ){boomTime = 0.0f; return true; } return false; }
};

#endif // SHOTTYPES_H_INCLUDED
