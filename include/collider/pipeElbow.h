#ifndef PIPEELBOW_H_INCLUDED
#define PIPEELBOW_H_INCLUDED

#include "waterPipe.h"
#include "arcSeg.h"

class pipeElbow : public waterPipe
{
    public:
    arcSeg side[2];
    // draw underwater area?

    pipeElbow(){}
    ~pipeElbow(){}
    void init( vec2d Pos0, vec2d Pos1, vec2d T0, vec2d T1, float Diameter, float FluidVel, float gravN, float fluidDensity, sf::Color UnderColor );
    pipeElbow( vec2d Pos0, vec2d Pos1, vec2d T0, vec2d T1, float Diameter, float FluidVel, float gravN, float fluidDensity, sf::Color UnderColor )
    { init( Pos0, Pos1, T0, T1, Diameter, FluidVel, gravN, fluidDensity, UnderColor ); }

    bool hit( mvHit& mh );
    void draw( sf::RenderTarget& RT )const;
};

#endif // PIPEELBOW_H_INCLUDED
