#include "functionSeg.h"
#include "mvHit.h"

void functionSeg::init( std::istream& is, std::function<float(float)> p_fY, std::function<float(float)> p_fYpm )
{
    pfY = p_fY;
    pfYpm = p_fYpm;

    float x1, y1, x2, y2;
    is >> x1 >> y1 >> x2 >> y2;
    unsigned int r,g,b;
    is >> r >> g >> b;
    sf::Color clr(r,g,b);
    is >> viewSeg;

    lineSeg::init( x1, y1, x2, y2, clr );
    Lu = L/len;

    // assign vertex positions

    Npts_fx = (int)( len + 1.0f );
    vtxVec.reserve( Npts_fx );
    if( p_fY && p_fYpm )
    {
        for( int i=0; i<Npts_fx; ++i )
        {
            float fx = (float)i;
            vec2d vtxPos = pos + Lu*fx + N*pfY(fx);
            sf::Vertex vtx;
            vtx.position.x = vtxPos.x;
            vtx.position.y = vtxPos.y;
            vtx.color = clr;
            vtxVec.push_back( vtx );
        }
    }
    else
    {
        for( int i=0; i<Npts_fx; ++i )
        {
            float fx = (float)i;
            vec2d vtxPos = pos + Lu*fx;
            sf::Vertex vtx;
            vtx.position.x = vtxPos.x;
            vtx.position.y = vtxPos.y;
            vtx.color = clr;
            vtxVec.push_back( vtx );
        }
    }
}

void functionSeg::draw( sf::RenderTarget& rRW )const
{
    if( viewSeg ) lineSeg::draw(rRW);
    rRW.draw( &vtxVec[0], vtxVec.size(), sf::LinesStrip );
}

bool functionSeg::hit( mvHit& mh )
{
    if( !(pfY && pfYpm) ) return lineSeg::hit(mh);// no curve

    vec2d sep = mh.pos - pos;// sep is rel position
 //   sep.to_base(Lu);// rotate to Lu, N coords
    float d = N.cross(sep);
    if( d < 0.0f || d > len ) return false;// not above curve

    float yPm = pfYpm(sep.x);// new pos
    float MHdim = mh.project(-N);
    float ampRes = pfY( sep.x ) - MHdim*( sqrtf( 1.0f + yPm*yPm ) - 1.0f );// wave amplitude + set above
    sep.y -= ampRes;
    float h = sep.dot(N);// height above curve
    if( h > MHdim || h < -MHdim ) return false;// not intersecting curve

     // will be treating hit
    //position shift
    mh.pos -= ampRes*N;
    // Get normal to surface
    vec2d Nsurf( 1.0f, yPm );
    Nsurf = Nsurf.get_LH_norm();

    bool Hit = false;
    vec2d Pimp, Nh;
    float dSep;
//    if( is_onMe( mh, Pimp, Nh, dSep ) )
    if( mh.is_inMe( *static_cast<const lineSeg*>(this), Pimp, Nh, dSep ) )
    {
        if( is_hard )
        {
            mh.bounce( Cf, Nsurf, friction_on );// velocity response
            mh.pos += ampRes*N;
            mh.setPosition( mh.pos + Nh*dSep );// position change response
            return true;
        }
        else
            mh.Float( Nsurf, Nh, dSep, 0.4f, 0.05f, 0.4f );

        Hit = true;
    }

    mh.pos += ampRes*N;// new pos

    return Hit;
}
