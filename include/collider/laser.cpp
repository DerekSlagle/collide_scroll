#include "laser.h"

float laser::beamLength = 1500.0f;

void laser::init( vec2d Pos, vec2d Dir, float Damage, sf::Color beamColor )
{
    pos = Pos; dir = Dir; damage = Damage;
    beam[0].position.x = pos.x;
    beam[0].position.y = pos.y;
    beam[1].position.x = pos.x + dir.x*beamLength;
    beam[1].position.y = pos.y + dir.y*beamLength;
    beam[0].color = beam[1].color = beamColor;
    state = false;
}

void laser::update( vec2d Pos, vec2d Dir )
{
    pos = Pos;
    dir = Dir;
    beam[0].position.x = pos.x;
    beam[0].position.y = pos.y;
    beam[1].position.x = pos.x + dir.x*beamLength;
    beam[1].position.y = pos.y + dir.y*beamLength;
}

bool laser::hit( mvHit& mh )
{
    if( state )
    {
        vec2d sep = mh.pos - pos;
        float d = sep.dot( dir );
        if( d > 0.0f )// is in front of beam
        {
            float h = sep.cross( dir );
            vec2d Nu( dir.y, -dir.x );// RH norm
            if( h < 0.0f )
            {
                Nu *= -1.0f;
                h *= -1.0f;
            }

            if( h < mh.project(Nu) )// hit
            {
                mh.v.to_base(dir);
                mh.v += 0.2*dir;// thrust along beam
                mh.v += 0.4*Nu;// thrust out of beam
                mh.v.from_base(dir);
                return true;
            }
        }
    }
    return false;
}

bool laser::hit( target& tgt )
{
    if( state && tgt.inUse )
    {
        vec2d sep = tgt.pMH->pos - pos;
        float d = sep.dot( dir );
        if( d > 0.0f )// is in front of beam
        {
            float h = sep.cross( dir );
            vec2d Nu( dir.y, -dir.x );// RH norm
            if( h < 0.0f )
            {
                Nu *= -1.0f;
                h *= -1.0f;
            }

            if( h < tgt.pMH->project(Nu) )// hit
            {
                tgt.pMH->v.to_base(dir);
                tgt.pMH->v += 0.25*dir;// thrust along beam
                tgt.pMH->v.from_base(dir);
                tgt.onHit( damage );
                return true;
            }
        }
    }
    return false;
}
