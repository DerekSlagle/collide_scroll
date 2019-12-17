#ifndef PIPECAP_H_INCLUDED
#define PIPECAP_H_INCLUDED

#include "waterPipe.h"

class pipeCap : public waterPipe
{
    public:
    float Rmax = 100.0f;
    lineSeg flap;
    lineSeg *pTieLeg;
    vec2d Pctr;

    pipeCap(){}
    ~pipeCap(){}
    void init( lineSeg* pTie, float h, float Diameter, float FluidVel, float gravN, float fluidDensity );
    pipeCap( lineSeg* pTie, float h, float Diameter, float FluidVel, float gravN, float fluidDensity ) { init( pTie, h, Diameter, FluidVel, gravN, fluidDensity ); }
    bool hit( mvHit& mh );
    void draw( sf::RenderTarget& RT )const;
};

#endif // PIPECAP_H_INCLUDED
