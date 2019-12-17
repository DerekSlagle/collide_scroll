#include "pathMover.h"

velPeriod* velPeriod::updateVel( vec2d& v_curr, vec2d& v_asg, float& timer, float dt )
{
    if( moveType == 'L' )
    {
        v_asg += a*dt;
    }
    else if( moveType == 'C' )// circular
    {
        float vMag = v_asg.mag();
        vec2d vu = v_asg/vMag;
        vec2d vNorm( -vu.y, vu.x );
        v_asg += ( vu*a.x + vNorm*(vMag*vMag/a.y) )*dt;
    }

    vec2d vRel = v_asg - v_curr;
    float vRelMag = vRel.mag();
    if( vRelMag > g )// slide
    {
        vec2d vRelU = vRel/vRelMag;
        v_curr += g*vRelU*dt;
    }
    else// equate vels
        v_curr = v_asg;

    timer += dt;
    if( timer >= period )
    {
        if( next ) v_asg = next->v0;
        else v_asg *= 0.0f;
        timer = 0.0f;
        return next;
    }

    return this;
}

velPeriod* makeVelPath( std::ifstream& fin )
{
    velPeriod* pHead = new velPeriod(fin);
    velPeriod* pCurr = pHead;

    while( fin )// rest of map
    {
        pCurr->next = new velPeriod(fin);
        if( !fin )
        {
            delete pCurr->next;
            break;
        }
        pCurr = pCurr->next;
    }

    pCurr->next = pHead;// closing the list

    return pHead;
}

int destroyVelPath( velPeriod*& pv )
{
    if( !pv ) return 0;

    velPeriod* pHead = pv;
    int killCount = 0;

    do
    {
        velPeriod* pCurr = pv;
        pv = pv->next;
        delete pCurr;
        ++killCount;
    } while( pv && pv != pHead );

    pv = nullptr;

 /*   while( pv && pv != pHead )
    {
        velPeriod* pCurr = pv;
        pv = pv->next;
        delete pCurr;
        ++killCount;
    }   */

    return killCount;
}

float pathMover::Fk = 4.0f, pathMover::Fd = 5.0f;

void pathMover::init( Leg* p_Leg, mvHit* p_MH, float vel, float s0, bool matchVel )
{
    pLeg = p_Leg; pMH = p_MH; v = vel; s = s0;
    if( pLeg && pMH )
    {
        oldPos.x = pLeg->x(s);
        oldPos.y = pLeg->y(s);
        pMH->setPosition( oldPos );
        if( matchVel ) pMH->v = vec2d( pLeg->x(s+v), pLeg->y(s+v) ) - oldPos;
    }
}

void pathMover::update( float dt, vec2d grav )
{
    if( pMH ) pMH->v += grav*dt;
    update(dt);
}

void pathMover::update( float dt )
{
 //   if( !( pLeg && pMH ) ) return;
    if( !pMH ) return;
    if( !pLeg )// use mvHit
    {
        if( pVelPer )
        {
            pVelPer = pVelPer->updateVel( pMH->v, oldPos, s, dt );
        }
        pMH->update(dt);
        return;
    }

    vec2d newPos;
    pLeg = pLeg->update( newPos.x, newPos.y, s, v, dt );

    if( mode == 'r' )
    {
        if( pLeg ) pMH->v = newPos - pMH->pos;
        pMH->setPosition( newPos );
    }
    else
    {
        vec2d vLeg = newPos - oldPos;
        vec2d Ftot = Fk*( newPos - pMH->pos ) - Fd*( pMH->v - vLeg );
        pMH->update( dt, Ftot/pMH->m );
    }

    oldPos = newPos;

    if( !pLeg && pVelPer )// transition to velPer
    {
        oldPos = pVelPer->v0;
        s = 0.0f;
    }
}

/*
void pathMover::update()
{
    if( !( pLeg && pMH ) ) return;
    pLeg = pLeg->update( pMH->pos.x, pMH->pos.y, s, v );
    pLeg->T( s, pMH->v.x, pMH->v.y );// unit length in correct direction
    pMH->v.x *= v;// scale
    pMH->v.y *= v;
    pMH->setPosition( pMH->pos );
}
*/

// aniMover
void aniMover::init2( spriteSheet* p_SS, size_t set_Num, float sclFctr )
{
    pSS = p_SS;
    setNum = set_Num;
    frIdx = 0;
    showMvHit = false;
    if( pSS )
    {
        spr.setTexture( pSS->txt );
        spr.setTextureRect( pSS->getFrRect( frIdx, setNum ) );
        spr.setScale(sclFctr,sclFctr);
        spr.setOrigin( spr.getLocalBounds().width/2.0f, spr.getLocalBounds().height/2.0f );
        if( pMH ) spr.setPosition( pMH->pos.x, pMH->pos.y );
    }
}

void aniMover::update( float dt )
{
    pathMover::update(dt);
    if( !pSS ) return;
    if( loopSet ) spr.setTextureRect( pSS->getFrRect( frIdx, setNum, true ) );
    if( pMH ) spr.setPosition( pMH->pos.x, pMH->pos.y );
}

void aniMover::update( float dt, vec2d grav )
{
    pathMover::update( dt, grav );
    if( !pSS ) return;
    if( loopSet ) spr.setTextureRect( pSS->getFrRect( frIdx, setNum, true ) );
    if( pMH ) spr.setPosition( pMH->pos.x, pMH->pos.y );
}

// special animation funcs
void aniMover::setFrames( size_t SetNum, bool LoopSet, size_t FrIdx )
{
    if( !pSS ) return;
    setNum = SetNum;
    if( setNum >= pSS->numSets() ) setNum = 0;
    frIdx = FrIdx;
    if( frIdx >= pSS->getFrCount(setNum) ) frIdx = 0;
    loopSet = LoopSet;
    spr.setTextureRect( pSS->getFrRect( frIdx, setNum, false ) );
}

bool aniMover::incrFrame()
 {
    if( !pSS ) return false;
    spr.setTextureRect(pSS->getFrRect( frIdx, setNum, true )  );
    return frIdx == 0;
 }

 bool aniMover::decrFrame()
 {
    if( !pSS ) return false;
    if( frIdx > 0 ) --frIdx;
    else frIdx = pSS->getFrCount( setNum ) - 1;
    spr.setTextureRect(pSS->getFrRect( frIdx, setNum, false )  );
    return frIdx == 0;
 }

 bool aniMover::incrSet()
 {
    if( !pSS ) return false;
    setFrames( ++setNum, loopSet );
    return setNum == 0;
 }

 bool aniMover::decrSet()
 {
    if( !pSS ) return false;
    if( setNum > 0 ) --setNum;
    else setNum = pSS->numSets() - 1;
    setFrames( setNum, loopSet );
    return setNum == 0;
 }

 void aniMover::setPosition( float X, float Y )
 {
    if( pSS ) spr.setPosition( X, Y );
    if( pMH ) pMH->setPosition( vec2d(X,Y) );
 }

 // target stuff
 int* target::pKillCount = nullptr;
 float target::HBtimeLimit = 100.0f;

void target::update( float dt )
{
    if( !pMH ) return;
    if( inUse )
    {
         aniMover::update(dt);
         if( pSS && setNum == setH && frIdx == 0 )// hit animation over
            setNum = setN;
        // for Health Bar
        if( HBtime < HBtimeLimit )// show health bar
        {
            HBtime += dt;
        //    HBvtx[0].position = HBvtx[1].position = HBvtx[2].position = HBvtx[3].position = sf::Vector2f( pMH->pos.x - pMH->r, pMH->pos.y - pMH->r ) + HBofst;
         //   float barLen = 2.0f*pMH->r*( strength/i_strength );
         //   HBvtx[1].position.x += barLen; HBvtx[2].position.x += barLen;
         //   HBvtx[2].position.y += HBheight; HBvtx[3].position.y += HBheight;
            setHBpos();
        }
    }
    else if( frIdx < pSS->getFrCount( setNum ) )
    {
        aniMover::update(dt);
        if( frIdx == 0 ) frIdx = pSS->getFrCount( setNum );// loop finished
    }
    else if( tLaunch >= 0.0f )
    {
        tLaunch -= dt;
        if( tLaunch < 0.0f )// launch target!
        {
         //   inUse = true;
         //   frIdx = pSS->getFrCount( setNum );
         //   reset( i_strength );
            inUse = true;
            frIdx = 0;
            spr.setTextureRect( pSS->getFrRect( frIdx, setNum ) );
        }
    }
}

void target::update( float dt, vec2d grav )
{
   if( inUse )
    {
         aniMover::update( dt, grav );
         if( pSS && setNum == setH && frIdx == 0 )// hit animation over
            setNum = setN;
    }
    else if( frIdx < pSS->getFrCount( setNum ) )
    {
        aniMover::update( dt, grav );
        if( frIdx == 0 ) frIdx = pSS->getFrCount( setNum );// loop finished
    }
}

void target::onHit( float damage )
{
    if( setNum != setN ) return;

    strength -= damage;
    if( strength <= 0.0f )// kill
    {
        inUse = false;
        if( pKillCount ) ++(*pKillCount);
        if( setK < pSS->numSets() )
        {
        //    std::cout << "\n kill ani started";
            setNum = setK;
            frIdx = 1;// kill ani supported
        }
        else// block normal animation
        {
            std::cout << "\n no kill ani";
            setNum = setN;
            frIdx = pSS->getFrCount( setNum );
        }
    }
    else
    {
        setNum = setH;
        frIdx = 0;
        HBtime = 0.0f;
        setHBpos();
    }
}

void target::setHBpos()
{
    HBvtx[0].position = HBvtx[1].position = HBvtx[2].position = HBvtx[3].position = sf::Vector2f( pMH->pos.x - pMH->r, pMH->pos.y - pMH->r ) + HBofst;
    float barLen = 2.0f*pMH->r*( strength/i_strength );
    HBvtx[1].position.x += barLen; HBvtx[2].position.x += barLen;
    HBvtx[2].position.y += HBheight; HBvtx[3].position.y += HBheight;
}

void target::reset( Leg* p_path, float t_launch )
{
    pLeg = p_path;
    tLaunch = t_launch;
    s = 0.0f;
    strength = i_strength;
  //  pMH->v *= 0.0f;
    vec2d vu;
    pLeg->T(0.0f, vu.x, vu.y );
    pMH->v = vu*v;
    setNum = setN;
    setPosition( pLeg->x(0.0f), pLeg->y(0.0f) );
    oldPos = vec2d( pLeg->x(0.0f), pLeg->y(0.0f) );
    HBtime = HBtimeLimit+1.0f;

    if( tLaunch < 0.0f )// immediate use
    {
        inUse = true;
        frIdx = 0;
        spr.setTextureRect( pSS->getFrRect( frIdx, setNum ) );
    }
    else// to be launched
    {
        inUse = false;
        frIdx = pSS->getFrCount( setNum );
    }
}

void target::reset( Leg* p_path, velPeriod* vp, vec2d pos, float t_launch )
{
    pLeg = p_path;
    tLaunch = t_launch;
    s = 0.0f;
    strength = i_strength;
    pMH->v *= 0.0f;
 //   vec2d vu;
 //   pLeg->T(0.0f, vu.x, vu.y );
 //   pMH->v = vu*v;
    setNum = setN;
    HBtime = HBtimeLimit+1.0f;
    pVelPer = vp;
    if( pLeg )
    {
        setPosition( pLeg->x(0.0f), pLeg->y(0.0f) );
        oldPos = vec2d( pLeg->x(0.0f), pLeg->y(0.0f) );
        vec2d vu;
        pLeg->T(0.0f, vu.x, vu.y );
        pMH->v = vu*v;
    }
    else
    {
        setPosition( pos.x, pos.y );

        if( vp && !pLeg )
        {
            oldPos = vp->v0;
        }
    }

    if( tLaunch < 0.0f )// immediate use
    {
        inUse = true;
        frIdx = 0;
        spr.setTextureRect( pSS->getFrRect( frIdx, setNum ) );
    }
    else// to be launched
    {
        inUse = false;
        frIdx = pSS->getFrCount( setNum );
    }
}

// firePatterns
bool firePattern1::update( float dt, size_t& idx, float& timer, vec2d& vShot, vec2d* pTgtDir )// fire shot when true
{
    timer += dt;
    if( timer >= delayVec[idx] )// fire rime
    {
        timer = 0.0f;
        idx = (1+idx)%delayVec.size();
        vShot = pTgtDir ? velVec[idx].to_base( *pTgtDir ) : velVec[idx];
        return true;
    }

    return false;
}

bool firePattern1::init( std::ifstream& fin )// pattern data from file
{
    if( !fin ) return false;

    size_t numDatum = 0;
    fin >> numDatum;
    delayVec.reserve( numDatum );
    velVec.reserve( numDatum );

    for( size_t i=0; i<numDatum; ++i )
    {
        vec2d vTemp;
        float timeTemp;
        fin >> vTemp.x >> vTemp.y >> timeTemp;
        velVec.push_back( vTemp );
        delayVec.push_back( timeTemp );
    }

    return true;
}

void makeCircleFP( firePattern1& FP, size_t Nshots, float vMag, float dt, float dtLong )
{
    FP.delayVec.clear(); FP.delayVec.reserve( Nshots );
    FP.velVec.clear();   FP.velVec.reserve( Nshots );
    float Ang = 0.0f;
    for( size_t i=0; i<Nshots; ++i )
    {
        const float dAng = 6.283f/(float)Nshots;
        vec2d shotV( vMag*cosf(Ang), vMag*sinf(Ang) );
        FP.velVec.push_back( shotV );
        FP.delayVec.push_back( dt );
        Ang += dAng;
    }

    FP.delayVec.back() = dtLong;
}

mvHit* ship::pFireMh = nullptr;

void ship::update( float dt )
{
    target::update(dt);
    vec2d vShot;
    mvHit* pFiredMH = nullptr;
    vec2d fireDir;
    vec2d* pFireDir = nullptr;
 //   if( pFireMh )
    bool doFire = false;
    if( !fireOnTarget ) doFire = true;
    else if( pFireMh && fireOnTarget )
    {
        fireDir = pFireMh->pos - pMH->pos;
        fireDir /= fireDir.mag();
        pFireDir = &fireDir;
        doFire = true;
    }

    // fire!
    if( doFire && inUse && pFireFunc && pFirePat && pFirePat->update( dt, fp_idx, fp_time, vShot, pFireDir ) )
    {
        if( !fireOnTarget ) vShot += pMH->v;
        float vMag = vShot.mag();
        pFiredMH = pFireFunc( pMH->pos + shotOfst, vShot/vMag, vMag );
        if( pFiredMH && pMH->is_free ) pMH->v -= pFiredMH->m*vShot/pMH->m;// recoil
    }
}
