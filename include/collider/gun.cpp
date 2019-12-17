#include "gun.h"

sf::Texture* gun::pTxtr = nullptr;

void gun::init( std::istream& is )
{
    block::init(is);
    is >> fireDelay >> bulletSpeed;
    // assign wDir hDir
    wDir = ptVec[1] + ptVec[2]; wDir /= wDir.mag();
    hDir = ptVec[0] + ptVec[1]; hDir /= hDir.mag();
    // muzzle flash
    mzlFlash[0].color = mzlFlash[1].color = sf::Color::Yellow;// center
    mzlFlash[2].color = mzlFlash[3].color = mzlFlash[4].color = mzlFlash[5].color = sf::Color::Red;// sides
    is_free = false;// stationary by default
}

void gun::setFlashPosition( vec2d flashPos )
{
    float s0 = 3.0f, sf = 5.0f, len = 12.0f;
    mzlFlash[0].position = Equate( flashPos );
    mzlFlash[1].position = Equate( flashPos + 1.3f*len*wDir );
    mzlFlash[2].position = Equate( flashPos + s0*hDir );
    mzlFlash[3].position = Equate( flashPos + len*wDir + sf*hDir );
    mzlFlash[4].position = Equate( flashPos - s0*hDir );
    mzlFlash[5].position = Equate( flashPos + len*wDir - sf*hDir );
}

void gun::update( float dt )
{
    regPolygon::update(dt);
    if( !p_FireFunc ) return;

    if( is_firing )// begin firing
    {
        ++fireCnt;
        if( !was_firing )fireCnt = fireDelay;// fire right away
        if( fireCnt >= fireDelay )// fire!
        {
        //    fireCnt = 0;// reset
        //    setFlashPosition( pos + (W/2.0f)*wDir );
            mvHit* pMh = p_FireFunc( pos + (W/2.0f)*wDir, wDir, bulletSpeed );
            if( pMh )
            {
                fireCnt = 0;
                setFlashPosition( pos + (W/2.0f)*wDir );
                if( is_free ) v -= pMh->v*pMh->m/m;// recoil
            }
        }

        if( fireDelay == 0 ) is_firing = false;// single fire
    }

    was_firing = is_firing;
}

/*
oid gun::update( float dt )
{
    regPolygon::update(dt);
    if( p_FireFunc || (ppBulletVec && ppBulletVec->size() > 0) )
    {
        if( is_firing )// begin firing
        {
            ++fireCnt;
            if( !was_firing )fireCnt = fireDelay;// fire right away
            if( fireCnt >= fireDelay )// fire!
            {
                fireCnt = 0;// reset
                if( ppBulletVec )
                {
                    size_t bulletIdx_st = bulletIdx;// will iterate once through for an available shot, then give up
                    do
                    {
                        bulletIdx = ( 1 + bulletIdx )%ppBulletVec->size();// std::cout << "\nfire!";
                    } while( bulletIdx != bulletIdx_st && !ppBulletVec->at(bulletIdx)->onFire() );

                    if( bulletIdx != bulletIdx_st )// found one!
                    {
                        mvHit* pMh = ppBulletVec->at(bulletIdx);
                        pMh->v = bulletSpeed*wDir;
                        pMh->setPosition( pos + (W/2.0f)*wDir + pMh->project(wDir)*wDir );
                        if( is_free ) v -= pMh->v*pMh->m/m;// recoil
                    }
                }
                else if( p_FireFunc ) p_FireFunc( pos + (W/2.0f)*wDir, wDir, bulletSpeed );
                setFlashPosition( pos + (W/2.0f)*wDir );
            }

            if( fireDelay == 0 ) is_firing = false;// single fire
        }

        was_firing = is_firing;
    }
 //   else std::cout << "no bs ";
}
*/

void gun::setAim( vec2d aimDir )
{
    if( aimDir.mag() < 0.1f ) return;

    wDir = aimDir; wDir /= wDir.mag();
    hDir.x = wDir.y;
    hDir.y = -wDir.x;

    vec2d hW = (W/2.0f)*wDir, hH = (H/2.0f)*hDir;

    ptVec[0] = -hW + hH;// up lt
    ptVec[1] =  hW + hH;// up rt
    ptVec[2] =  hW - hH;// dn rt
    ptVec[3] = -hW - hH;// dn lt

    for( size_t i=0; i<nSides; ++i)
    {
        vtxVec[i].position.x = pos.x+ptVec[i].x;
        vtxVec[i].position.y = pos.y+ptVec[i].y;
    }

    vtxVec[nSides].position.x = pos.x+ptVec[0].x;
    vtxVec[nSides].position.y = pos.y+ptVec[0].y;

    return;
}

void gun::draw( sf::RenderTarget& rRW )const
{
    if( gun::pTxtr ) rRW.draw( &(vtxVec[0]), 4, sf::Quads, gun::pTxtr );
    else regPolygon::draw(rRW);
    if( is_firing && fireCnt == 0 ) rRW.draw( mzlFlash, 6, sf::Lines );
}

void gun::setRotation( float angle )
{
    circPos = angle;
    block::setRotation(angle);
    wDir = ptVec[1] + ptVec[2]; wDir /= wDir.mag();
    hDir = ptVec[0] + ptVec[1]; hDir /= hDir.mag();
}

state_ab* gun::newState()
{
    std::function<void(vec2d,vec2d, float, int)> p_SetFuncVVFI = std::bind( &gun::setState_bk, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
    return new state_vvfi( &pos, &v, &circPos, &fireCnt, p_SetFuncVVFI );
}

void gun::setState_bk( vec2d Pos, vec2d Vel, float R, int growSt )
{
    fireCnt = growSt;
    v = Vel;
    setPosition(Pos);
    setRotation(R);
}
