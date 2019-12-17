#ifndef PIPESTRAIGHT_H_INCLUDED
#define PIPESTRAIGHT_H_INCLUDED

#include "waterPipe.h"

class pipeStraight : public waterPipe
{
    public:
    lineSeg side[2];
    sf::Vertex underQuad[4];
    vec2d pos0, pos1;// ends of pipe center line
    vec2d Tend0, Tend1;

    pipeStraight(){}
    ~pipeStraight(){}
    void init( vec2d Pos0, vec2d Pos1, vec2d T0, vec2d T1, float Diameter, float FluidVel, float gravN, float fluidDensity, sf::Color UnderColor );
    pipeStraight( vec2d Pos0, vec2d Pos1, vec2d T0, vec2d T1, float Diameter, float FluidVel, float gravN, float fluidDensity, sf::Color UnderColor )
    { init( Pos0, Pos1, T0, T1, Diameter, FluidVel, gravN, fluidDensity, UnderColor ); }

    bool hit( mvHit& mh );
    void draw( sf::RenderTarget& RT )const;
};

#endif // PIPESTRAIGHT_H_INCLUDED
