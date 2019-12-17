#include "pipeElbow.h"

void pipeElbow::init( vec2d Pos0, vec2d Pos1, vec2d T0, vec2d T1, float Diameter, float FluidVel, float gravN, float fluidDensity, sf::Color UnderColor )
{
//    s[0].x = x1; s[0].y = y1;
//    s[1].x = x2; s[1].y = y2;
//    R = radius;
//    arcSeg::init( float x1, float y1, float x2, float y2, float radius, sf::Color clr );
    diam = Diameter > 0 ? Diameter : -Diameter;
    fluidVel = FluidVel;
    grav_N = gravN; Density = fluidDensity;
    underColor = UnderColor;
    Tu = Pos1 - Pos0;
    Tu /= Tu.mag();

 //   vec2d sep = Pos1 - Pos0;
    vec2d N0( T0.get_LH_norm() ), N1( T1.get_LH_norm() );
    float Rctr = (Pos1-Pos0).mag()/(N0-N1).mag();
    vec2d pt0, pt1;

    if( Tu.cross(T0)*Tu.cross(N0) < 0.0f )// s[0] end = 1st args
    {
        std::cout << "\nthis one";
        pt0 = Pos0 - N0*diam/2.0f; pt1 = Pos1 + N1*diam/2.0f;
        side[0].init( pt0.x, pt0.y, pt1.x, pt1.y, Rctr - diam/2.0f );// inner wall
        pt0 = Pos0 + N0*diam/2.0f; pt1 = Pos1 - N1*diam/2.0f;
        side[1].init( pt0.x, pt0.y, pt1.x, pt1.y, Rctr + diam/2.0f );// outer wall
    }
    else if( Tu.cross(T1)*Tu.cross(N1) < 0.0f )// other case
    {
        std::cout << "\nthat one";
        pt0 = Pos1 - N1*diam/2.0f; pt1 = Pos0 + N0*diam/2.0f;
        side[0].init( pt0.x, pt0.y, pt1.x, pt1.y, Rctr - diam/2.0f );// inner wall
        pt0 = Pos1 + N1*diam/2.0f; pt1 = Pos0 - N0*diam/2.0f;
        side[1].init( pt0.x, pt0.y, pt1.x, pt1.y, Rctr + diam/2.0f );// outer wall
    }

    /*
    if( Tu.cross(T0)*Tu.cross(N0) < 0.0f )// s[0] end = 1st args
    {
        std::cout << "\nthis one";
        pt0 = Pos0 + N0*diam/2.0f; pt1 = Pos1 + N1*diam/2.0f;
        side[0].init( pt0.x, pt0.y, pt1.x, pt1.y, Rctr - diam/2.0f );// inner wall
        pt0 = Pos0 - N0*diam/2.0f; pt1 = Pos1 - N1*diam/2.0f;
        side[1].init( pt0.x, pt0.y, pt1.x, pt1.y, Rctr + diam/2.0f );// outer wall
    }
    else if( Tu.cross(T1)*Tu.cross(N1) < 0.0f )// other case
    {
        std::cout << "\nthat one";
        pt0 = Pos1 + N1*diam/2.0f; pt1 = Pos0 + N0*diam/2.0f;
        side[0].init( pt0.x, pt0.y, pt1.x, pt1.y, Rctr - diam/2.0f );// inner wall
        pt0 = Pos1 - N1*diam/2.0f; pt1 = Pos0 - N0*diam/2.0f;
        side[1].init( pt0.x, pt0.y, pt1.x, pt1.y, Rctr + diam/2.0f );// outer wall
    }
    */
}

bool pipeElbow::hit( mvHit& mh )
{
    bool Hit = side[0].hit(mh);
    if( !Hit ) Hit = side[1].hit(mh);

    // mh in pipe?
    vec2d sep = mh.pos - side[0].pos;
    float sepMag = sep.mag();
    if( sepMag > side[1].R ) return Hit;
    if( sepMag < side[0].R ) return Hit;
    if( sep.cross(side[0].s[0]) < 0.0f ) return Hit;
    if( sep.cross(side[0].s[1]) > 0.0f ) return Hit;
    vec2d Tfl = sep.get_LH_norm();

    mh.v -= fluidVel*Tfl;
    mh.Float( vec2d(0.0f,-1.0f), grav_N, Density );
    mh.v += fluidVel*Tfl;
    Hit = true;

    return Hit;
}

void pipeElbow::draw( sf::RenderTarget& RT )const
{
    side[0].draw(RT);
    side[1].draw(RT);
    // under color when solved
}
