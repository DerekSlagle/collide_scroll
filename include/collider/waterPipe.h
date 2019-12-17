#ifndef WATERPIPE_H_INCLUDED
#define WATERPIPE_H_INCLUDED

#include "mvHit.h"
#include "lineSeg.h"

class waterPipe
{
    public:
  //  lineSeg side[2], flap[2];
 //   lineSeg *pTieLeg[2];
    float fluidVel;
    float grav_N, Density;
    float diam;
 //   float Rmax = 100.0f;
    vec2d Tu;// unit length along axis

 //   sf::Vertex underQuad[4];
    sf::Color underColor;

    waterPipe(){}
    virtual ~waterPipe(){}
 //   void init( lineSeg* pTie1, float h1, lineSeg* pTie2, float h2, float Diameter, float FluidVel, float gravN, float fluidDensity, sf::Color UnderColor );
    virtual bool hit( mvHit& mh ) = 0;
    virtual void draw( sf::RenderTarget& RT )const = 0;
};

#endif // WATERPIPE_H_INCLUDED
