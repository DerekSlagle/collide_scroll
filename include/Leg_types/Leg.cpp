#include "Leg.h"

Leg::Leg( float X0, float Y0, float Len ): x0(X0), y0(Y0), len(Len), prev(nullptr), next(nullptr)
{
    //ctor
}

Leg::~Leg()
{
    //dtor
}

void Leg::T( float s, float& Tx, float& Ty )const// providde unit tangent to curve at s
{
    static const float ds = 0.01f;
    Tx = x(s+ds) - x(s-ds);
    Ty = y(s+ds) - y(s-ds);
    float magT = sqrtf( Tx*Tx + Ty*Ty );
    Tx /= magT;
    Ty /= magT;
}

const Leg* Leg::updateGrav( float& rx, float& ry, float& s, float& v, float dt, float g )const
{
    float dy = y(s+v) - y(s);
    float dv = g*dy/v;
//    if( v < g && v > -g ) dv = g;
//    v += dv;
    if( v < 0.0f && v > -g*dt ) v = g*dt;
    else if( v > 0.0f && v < g*dt ) v = -g*dt;
    else v += dv*dt;
    return update( rx, ry, s, v, dt );
}

const Leg* Leg::update( float& rx, float& ry, float& s, float v, float dt )const
{
    s+= v*dt;
    const Leg* pNewLeg = this;
    if( v < 0.0f )
    {
        if( s < 0.0f )
        {
            pNewLeg = prev;
            if( prev )
                s += pNewLeg->len;
            else s = 0.0f;
        }

    }
    else// v > 0
    {
        if( s > len )
        {
            pNewLeg = next;
            if( next )
                s -= len;
            else s = len;
        }
    }

    if( pNewLeg )
    {
    //    rx = pNewLeg->x(s);
    //    ry = pNewLeg->y(s);
        pNewLeg->set_xy( s, rx, ry );
    }
    else// rest at end of current Leg
    {
      //  rx = x(s);
      //  ry = y(s);
        set_xy( s, rx, ry );
    }

    return pNewLeg;
}
