#include "pipeStraight.h"

void pipeStraight::init( vec2d Pos0, vec2d Pos1, vec2d T0, vec2d T1, float Diameter, float FluidVel, float gravN, float fluidDensity, sf::Color UnderColor )
{
    // base object
    diam = Diameter > 0 ? Diameter : -Diameter;
    fluidVel = FluidVel;
    grav_N = gravN; Density = fluidDensity;
    underColor = UnderColor;
    Tu = Pos1 - Pos0;
    Tu /= Tu.mag();

    // this
    pos0 = Pos0;
    pos1 = Pos1;
    Tend0 = T0;
    Tend1 = T1;
    vec2d N0 = T0. get_LH_norm();
    float proj0 = T0.dot(Tu);
    vec2d N1 = T1. get_LH_norm();
    float proj1 = T1.dot(Tu);
    side[0].init( pos0 + N0*(diam/(2.0f*proj0)), pos1 + N1*(diam/(2.0f*proj1)) );
    side[1].init( pos0 - N0*(diam/(2.0f*proj0)), pos1 - N1*(diam/(2.0f*proj1)) );
    side[0].is_bulletProof = side[1].is_bulletProof = true;

    // define water color quad
    underQuad[0].position.x = side[0].pos.x;
    underQuad[0].position.y = side[0].pos.y;// up lt
    underQuad[1].position.x = side[0].pos.x + side[0].L.x;// up rt
    underQuad[1].position.y = side[0].pos.y + side[0].L.y;
    underQuad[2].position.x = side[1].pos.x + side[1].L.x;// dn rt
    underQuad[2].position.y = side[1].pos.y + side[1].L.y;
    underQuad[3].position.x = side[1].pos.x;// dn lt
    underQuad[3].position.y = side[1].pos.y;
    underQuad[0].color = underQuad[1].color = underQuad[2].color = underQuad[3].color = underColor;
}

bool pipeStraight::hit( mvHit& mh )
{
    bool Hit = side[0].hit(mh);
    if( !Hit ) Hit = side[1].hit(mh);

    // is mh in pipe?
    vec2d posRel = mh.pos - pos0;
    vec2d posRel_1 = mh.pos - pos1;
//    float mhRad = mh.project(Tu);

//    if( posRel.dot(Tu) > 0.0f && posRel.dot(Tu) < side[0].len )// mh between pipe ends
 //   if( posRel.cross( Tend0.get_LH_norm() ) > 0.0f && posRel_1.cross( Tend1.get_LH_norm() ) > 0.0f )// mh between pipe ends
    if( posRel.dot( Tend0 ) < 0.0f && posRel_1.dot( Tend1 ) < 0.0f )// mh between pipe ends
    {

        float perpCLdist = Tu.cross(posRel);
      //  std::cout << " " << perpCLdist;
     //   if( perpCLdist < (diam/2.0f-mhRad) && perpCLdist > (mhRad-diam/2.0f) )// mh in pipe
        if( perpCLdist < diam/2.0f && perpCLdist > -diam/2.0f )// mh in pipe
     //   if( perpCLdist < 20.0f && perpCLdist > -20.0f )// mh in pipe
        {
        //    std::cout << "diam " << diam;
            mh.v += fluidVel*Tu;
            mh.Float( vec2d(0.0f,-1.0f), grav_N, Density );
            mh.v -= fluidVel*Tu;
            Hit = true;
        }
    }

    return Hit;
}

void pipeStraight::draw( sf::RenderTarget& RT )const
{
    side[0].draw(RT);
    side[1].draw(RT);
    RT.draw( underQuad, 4, sf::Quads );
}
