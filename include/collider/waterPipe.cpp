#include "waterPipe.h"

/*
void waterPipe::init( lineSeg* pTie1, float h1, lineSeg* pTie2, float h2, float Diameter, float FluidVel, float gravN, float fluidDensity, sf::Color UnderColor )
{
    diam = Diameter;
    fluidVel = FluidVel;
    grav_N = gravN; Density = fluidDensity;
    underColor = UnderColor;
    // assign side closest to wall tops
    vec2d TW1 = pTie1->L/pTie1->len;
    vec2d P1 = pTie1->pos + h1*TW1;
    vec2d TW2 = pTie2->L/pTie2->len;
    vec2d P2 = pTie2->pos + h2*TW2;
    side[0].init( P1, P2, pTie1->vtx->color );
    P1 += diam*TW1; P2 += diam*TW2;// flaps attach here
    side[1].init( P1, P2, pTie1->vtx->color );
    Tu = side[0].L/side[0].len;

    // flaps
    flap[0].init( P1, pTie1->L + pTie1->pos, pTie1->vtx->color );
    flap[1].init( P2, pTie2->L + pTie2->pos, pTie2->vtx->color );

    // shorten the tie walls
    pTie1->L = side[0].pos - pTie1->pos;
    pTie1->vtx[1].position.x = side[0].pos.x;
    pTie1->vtx[1].position.y = side[0].pos.y;
    pTie1->len = pTie1->L.mag();

    pTie2->L = side[0].pos + side[0].L - pTie2->pos;
    pTie2->vtx[1].position.x = side[0].pos.x + side[0].L.x;
    pTie2->vtx[1].position.y = side[0].pos.y + side[0].L.y;
    pTie2->len = pTie2->L.mag();

    // set bulletProofing
    flap[0].is_bulletProof = pTie1->is_bulletProof;
    flap[1].is_bulletProof = pTie2->is_bulletProof;
    side[0].is_bulletProof = side[1].is_bulletProof = pTie1->is_bulletProof || pTie2->is_bulletProof;

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

bool waterPipe::hit( mvHit& mh )
{
    bool Hit = flap[0].hit(mh);
    if( !Hit ) Hit = flap[1].hit(mh);
    if( side[0].hit(mh) ) Hit = true;
    if( side[1].hit(mh) ) Hit = true;

    // is mh in pipe?
    vec2d posRel = mh.pos - side[0].pos;
    float mhRad = mh.project(Tu);
    if( posRel.dot(Tu) > 0.0f && posRel.dot(Tu) < side[0].len )// mh between pipe ends
    {
        if( Tu.cross(mh.pos - side[0].pos) < 0.0f && Tu.cross(mh.pos - side[1].pos) > 0.0f )// mh in pipe
        {
         //   vec2d dragAcc = mh.drag*Density*fluidVel*Tu*mhRad*2.0f/mh.m;
       //     mh.v -= fluidVel*Tu;
            mh.v += fluidVel*Tu;
            mh.Float( vec2d(0.0f,-1.0f), grav_N, Density );
        //    mh.v += fluidVel*Tu;
            mh.v -= fluidVel*Tu;
        }
    }
    else// end effects
    {
        // left end
        vec2d ctr = (side[0].pos + side[1].pos)/2.0f;
        vec2d R = ctr - mh.pos;
        float magR = R.mag();
        if ( magR < Rmax )
        {
            mh.v -= mh.drag*Density*fluidVel*R*mhRad*2.0f/(mh.m*magR);
        }
        else// try right end
        {
            ctr = ( side[0].pos + side[0].L + side[1].pos + side[0].L )/2.0f;
            R =mh.pos - ctr;
            magR = R.mag();
            if ( magR < Rmax )
            {
                mh.v -= mh.drag*Density*fluidVel*R*mhRad*2.0f/(mh.m*magR);
            }
        }
    }

    return Hit;
}

void waterPipe::draw( sf::RenderTarget& RT )const
{
    RT.draw( underQuad, 4, sf::Quads );
    side[0].draw(RT);
    side[1].draw(RT);
    flap[0].draw(RT);
    flap[1].draw(RT);
}   */
