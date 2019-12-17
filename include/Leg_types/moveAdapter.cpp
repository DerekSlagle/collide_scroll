#include "moveAdapter.h"

void moveAdapter::init( Leg& rLeg, float v_0, float v_1, float tLeg )
{
    pLeg = &rLeg;
    v0 = v_0; v1 = v_1;
    tL = tLeg;
    ap = 12.0f*( (v1+v0)*tL/2.0f - rLeg.len )/(tL*tL*tL);
    a0 = (v1-v0)/tL - ap*tL/2.0f;
    std::cout << "\na0 = " << a0 << " ap = " << ap;
}

moveAdapter* moveAdapter::update( float& x, float& y, float dt )
{
    tElap += dt;

    if( tElap >= tL )// end of Leg
    {
        if( next )
        {
            next->tElap = tElap - tL;
            next->pLeg->set_xy( next->get_s(), x, y );
        }
        else
        {
            pLeg->set_xy( pLeg->len, x, y );
        }
        return next;
    }

    pLeg->set_xy( get_s(), x, y );

    return this;
}

moveAdapter* moveAdapter::update( float& s, float dt )
{
    tElap += dt;

    if( tElap >= tL )// end of Leg
    {
        if( next )
        {
            float tE = tElap - tL;// remainder;
            next->tElap = tE;
            s = next->get_s();//next->v0*tE + next->a0*tE*tE/2.0f + next->ap*tE*tE*tE/6.0f;
        }
        else s = pLeg->len;

        return next;
    }

    s = get_s();//v0*tElap + a0*tElap*tElap/2.0f + ap*tElap*tElap*tElap/6.0f;

    return this;
}
