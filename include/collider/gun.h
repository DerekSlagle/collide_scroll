#ifndef GUN_H_INCLUDED
#define GUN_H_INCLUDED

#include "block.h"

class gun : public block
{
    public:
    static sf::Texture* pTxtr;
    vec2d wDir, hDir;// unit vectors in direction of width (firing axis) and height
    float circPos;// 1st to have on regPolygon side. Will maintain for now in setRotation()
//    std::vector<mvHit*>* ppBulletVec = nullptr;
    std::function<mvHit*( vec2d, vec2d, float )> p_FireFunc = nullptr;
    sf::Vertex mzlFlash[6];// 3 lines
    bool is_firing = false;
    bool was_firing = false;
    int fireDelay = 3;
    int fireCnt = 0;// counts to fireDelay
    size_t bulletIdx = 0;// place is bullet vector
    float bulletSpeed = 50.0f;
//    float r;// length of rays
 //   size_t nSides;// = ptVec.size()
 //   std::vector<vec2d> ptVec;// official
 //   std::vector<sf::Vertex> vtxVec;// visual

    virtual void init( std::istream& is );
    gun( std::istream& is ) { init(is); }
    gun() {}
    virtual mvHit* clone() const { return new gun(*this); }
    virtual ~gun() {}
    virtual const char* myName() const { return "gun"; }

    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& rRW )const;
    void setAim( vec2d aimDir );
    void setFlashPosition( vec2d flashPos );
     virtual void setRotation( float angle );
    // all other inherited functions should do
    void setState_bk( vec2d Pos, vec2d Vel, float R, int growSt );
    virtual state_ab* newState();
};

#endif // GUN_H_INCLUDED
