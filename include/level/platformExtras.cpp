#include "platformExtras.h"

void pickItem::init( size_t Nsides, float R, float Value, sf::Vector2f Pos, sf::Color clr )
{
    pos = Pos;
    r = R;
    value = Value;
    vtxVec.reserve(Nsides+1);// to close the LinesStrip
    vec2d pt0( r, 0.0f );// starter side
//    pt0 = pt0.Rotate( iAngle );
    float dAngle = 6.283185f/Nsides;
    for( size_t i=0; i<Nsides; ++i)
    {
        vtxVec.push_back( sf::Vertex( sf::Vector2f(pos.x+pt0.x, pos.y+pt0.y), clr ) );
        pt0 = pt0.Rotate( dAngle );
    }
    vtxVec.push_back( vtxVec[0] );
}

bool pickItem::hit( mvHit& mh ) const
{
    vec2d sep = vec2d( pos.x, pos.y ) - mh.pos;
    return sep.dot(sep) < (r+mh.r)*(r+mh.r);
}
