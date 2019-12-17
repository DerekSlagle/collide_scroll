#include "pipeCap.h"

void pipeCap::init( lineSeg* pTie, float h, float Diameter, float FluidVel, float gravN, float fluidDensity )
{
    diam = Diameter;
    fluidVel = FluidVel;
    grav_N = gravN; Density = fluidDensity;
    Tu = pTie->L.get_LH_norm();

    // make the flap
    vec2d Lu = pTie->L/pTie->len;
    flap.pos = pTie->pos;// + h*Lu;
    flap.L = h*Lu;
    flap.len = h;
    Pctr = flap.pos + Lu*( h + diam/2.0f );
    flap.vtx[0].color = flap.vtx[1].color = sf::Color::Red;//pTie->vtx[0].color;
    flap.vtx[0].position.x = flap.pos.x;
    flap.vtx[0].position.y = flap.pos.y;
    flap.vtx[1].position.x = flap.pos.x + flap.L.x;
    flap.vtx[1].position.y = flap.pos.y + flap.L.y;
    flap.is_bulletProof = pTie->is_bulletProof;
    std::cout << "\ncap ctor bpf: " << flap.is_bulletProof;

    // fix the wall
    pTie->pos += Lu*( h + diam );
    pTie->L -= Lu*( h + diam );
    pTie->len = pTie->L.mag();
 //   pTie->vtx[0].position = flap.vtx[1].position;
    pTie->vtx[0].position.x = pTie->pos.x;
    pTie->vtx[0].position.y = pTie->pos.y;
}

bool pipeCap::hit( mvHit& mh )
{
    bool Hit = flap.hit(mh);

    vec2d sep = mh.pos - Pctr;
    if( sep.dot(sep) > Rmax*Rmax ) return Hit;
    if(  sep.dot(Tu) < 0.0f ) return Hit;// in pipe, not in tank

    // it's a hit!
    float mhRad = mh.project(Tu);
    float magSep = sep.mag();
    mh.v += mh.drag*Density*fluidVel*sep*mhRad*2.0f/(mh.m*magSep);
    return true;
}

void pipeCap::draw( sf::RenderTarget& RT )const
{
    flap.draw(RT);
}
