#include "shotTypes.h"

// shot stuff
float shot::minVelSq = 100.0f;

bool shot::hit( target& tgt )
{
    if( this->inUse && tgt.inUse && tgt.pMH && hit( *tgt.pMH ) )
    {
    //    tgt.strength -= damage;
    //    if( tgt.strength <= 0.0f ) tgt.inUse = false;
        inUse = false;// ?? need ??
        tgt.onHit( damage );

        if( pSS )// start death animation
        {
            frIdx = 0;
            spr.setTextureRect( pSS->getFrRect( frIdx, setNum ) );
            spr.setPosition( pos.x, pos.y );
            v = tgt.pMH->v/2.0f;// death animation trails target just hit
        }
        return true;
    }
    return false;
}

void shot::init2( spriteSheet* p_SS, size_t set_Num, float sclFctr )
{
    pSS = p_SS;
    setNum = set_Num;
    frIdx = 0;
    if( pSS )
    {
        frIdx = pSS->getFrCount( setNum );// block death animation
        spr.setTexture( pSS->txt );
        spr.setTextureRect( pSS->getFrRect( 0, setNum ) );
        spr.setScale(sclFctr,sclFctr);
        spr.setOrigin( spr.getLocalBounds().width/2.0f, spr.getLocalBounds().height/2.0f );
        spr.setPosition( pos.x, pos.y );
    }
}

void shot::update( float dt )
{
    if( inUse ){ ball::update(dt); if( v.dot(v) < minVelSq || pos.y < 0.0f ) inUse = false; }
    else if( pSS && frIdx < pSS->getFrCount( setNum ) )
    {
        pos += v*dt;
        spr.setTextureRect( pSS->getFrRect( frIdx, setNum, false ) );
        spr.setPosition( pos.x, pos.y );
        ++frIdx;
    }
}

// static function
mvHit* shot::fireShot( std::vector<shot*>& pShotVec, vec2d pos, vec2d dir, float speed )
{
    size_t bulletIdx = 0;// will iterate once through for an available shot, then give up
    do
    {
        bulletIdx = ( 1 + bulletIdx )%pShotVec.size();// std::cout << "\nfire!";
    } while( bulletIdx != 0 && !pShotVec.at(bulletIdx)->onFire() );

    if( bulletIdx != 0 )// found one!
    {
        shot* pShot = pShotVec.at(bulletIdx);
        pShot->v = speed*dir;
        pShot->setPosition( pos + + pShot->project(dir)*dir );
        return static_cast<mvHit*>(pShot);
    }

    return nullptr;
}

void guidedShot::update( float dt )
{
    if( inUse && pTgt && pTgt->pMH )
    {
        float vMag = v.mag();
        if( vMag*vMag < minVelSq ) { inUse = false; return; }
        vec2d vu = v/vMag;
   //     v += a_t*vu;
        vec2d sep = pTgt->pMH->pos - pos;
        float sepMag = sep.mag();
        if( sepMag > r )
        {
            vec2d sepu = sep/sepMag;
            vec2d vn( -v.y, v.x );
             // apply accel
            v += a_p*vn*vu.cross(sepu);// proport to sine of angle towards target
            float vMag1 = v.mag();
            v *= ( vMag + a_t )/vMag1;
        }
    }

    shot::update(dt);// support death ani
}

bool guidedShot::hit( target& tgt )
{
    bool retVal = shot::hit(tgt);
    if( retVal && pTgt == &tgt && !tgt.inUse ) pTgt = nullptr;
    return retVal;
}

// clusterShot

bool clusterShot::hit( target& tgt )
{
    if( this->inUse && tgt.inUse && tgt.pMH && shot::hit( *tgt.pMH ) )
    {
        tgt.onHit( damage );
        return true;
    }
    return false;
}

void clusterShot::update( float dt )
{
    if( inUse )
    {
        boomTime += dt;
        if( boomTime >= boomDelay )// explode!
        {
            inUse = false;
            if( pSS )// start death animation
            {
                frIdx = 0;
                spr.setTextureRect( pSS->getFrRect( frIdx, setNum ) );
                spr.setPosition( pos.x, pos.y );
                v *= 0.0f;
                if( p_FireFunc )
                {
                    vec2d pt0( 0.0f, r );// starter side
                    float dAngle = 6.283185f/numFrags;
                    for( size_t i=0; i< numFrags; ++i )
                    {
                        p_FireFunc( pos + pt0, pt0/r, vFrags );
                        pt0 = pt0.Rotate( dAngle );
                    }
                }
            }
        }
        else
        {
            ball::update(dt);
        }
    }
    else if( pSS && frIdx < pSS->getFrCount( setNum ) )
    {
        spr.setTextureRect( pSS->getFrRect( frIdx, setNum, false ) );
        spr.setPosition( pos.x, pos.y );
        ++frIdx;
    }

 //   shot::update(dt);
}

/*
void clusterShot::update( float dt )
{
    if( inUse )
    {
        boomTime += dt;
        if( boomTime >= boomDelay )
        {
            inUse = false;
            if( pSS )// start death animation
            {
                frIdx = 0;
                spr.setTextureRect( pSS->getFrRect( frIdx, setNum ) );
                spr.setPosition( pos.x, pos.y );
                v *= 0.0f;
                if( p_FireFunc )
                {
                    vec2d pt0( 0.0f, r );// starter side
                    float dAngle = 6.283185f/numFrags;
                    for( size_t i=0; i< numFrags; ++i )
                    {
                        p_FireFunc( pos + pt0, pt0/r, vFrags );
                        pt0 = pt0.Rotate( dAngle );
                    }
                }
            }
        }
    }

    shot::update(dt);
}
*/
