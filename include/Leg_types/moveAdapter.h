#ifndef MOVEADAPTER_H_INCLUDED
#define MOVEADAPTER_H_INCLUDED

#include "Leg.h"
#include <iostream>

class moveAdapter
{
    public:
    Leg* pLeg = nullptr;// pLeg->len gives length
    float v0=0.0f, v1=0.0f, tL=1.0f;// given
    float a0=0.0f, ap=0.0f;// implied
    float tElap = 0.0f;// current time on Leg 0 -> tL

    moveAdapter *next = nullptr;// next if v1 > 0, prev if < 0

     moveAdapter(){}
    void init( Leg& rLeg, float v_0, float v_1, float tLeg );
    moveAdapter( Leg& rLeg, float v_0, float v_1, float tLeg ) { init( rLeg, v_0, v_1, tLeg ); }
    moveAdapter* update( float& x, float& y, float dt );// uses pLeg to assign x, y
    moveAdapter* update( float& s, float dt );// no pLeg usage. Writes s only
    float get_s() const { return v0*tElap + a0*tElap*tElap/2.0f + ap*tElap*tElap*tElap/6.0f; }
};


#endif // MOVEADAPTER_H_INCLUDED
