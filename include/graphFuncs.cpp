#include "graphFuncs.h"

void graphFuncs::init( std::istream& is )
{
    is >> pos.x >> pos.y >> length >> sclY;
    is >> xLt >> xRt >> x0;// x0 = expansion center
//    sclX = (xRt-xLt)/length;
    sclX = length/(xRt-xLt);

    size_t Ncoeffs = 0;
    is >> Ncoeffs;
    C.reserve(Ncoeffs);
    for( size_t i=0; i<Ncoeffs; ++i )
    {
        float Ctmp = 0.0f;
        is >> Ctmp;
        C.push_back( Ctmp );
    }

    // assign vertex colors
    if( xLt < xRt && x0 >= xLt && x0 <= xRt )// all should be good
    {
        dxVtx = 3.0f;
        size_t Nvtx = (size_t)( (xRt-xLt)*sclX/dxVtx );
        if( Nvtx < 2 ) return;// min for a LinesStrip
        vtxVec.resize( Nvtx );
   //     for( auto& vtx : vtxVec ) vtx.color = sf::Color::Cyan;
        dxVtx = ((xRt-xLt))/(float)( vtxVec.size() - 1 );
        float x = xLt;
        float y = 0.0f, yp;// as ref params in f()

        for( auto& vtx : vtxVec )
        {
            vtx.color = sf::Color::Cyan;
            vtx.position.x = (x - xLt)*sclX + pos.x;
            f( y, yp, x );
        //    vtx.position.y = pos.y;
            vtx.position.y = pos.y - y*sclY;// inverted y axis
            x += dxVtx;
        }

        // the axes
        float tip_dx = 10.0f;
        float tip_dy = 4.0f;
        xAxis[0].position.x = pos.x;
        xAxis[0].position.y = pos.y;
        xAxis[1].position.x = pos.x + length;
        xAxis[1].position.y = pos.y;
        xAxis[2].position = xAxis[3].position = xAxis[4].position = xAxis[5].position = xAxis[1].position;// tip
        xAxis[3].position.x -= tip_dx;
        xAxis[3].position.y += tip_dy;
        xAxis[5].position.x -= tip_dx;
        xAxis[5].position.y -= tip_dy;

        // y axis
        yAxis[0].position.x = pos.x + (x0-xLt)*sclX;
        yAxis[0].position.y = pos.y + length/5.0f;
        yAxis[1].position.x = yAxis[0].position.x;
        yAxis[1].position.y = pos.y - length/2.0f;
        yAxis[2].position = yAxis[3].position = yAxis[4].position = yAxis[5].position = yAxis[1].position;// tip
        yAxis[3].position.x -= tip_dy;
        yAxis[3].position.y += tip_dx;
        yAxis[5].position.x += tip_dy;
        yAxis[5].position.y += tip_dx;

        // color
        for( size_t i=0; i<6; ++i ) xAxis[i].color = yAxis[i].color = sf::Color::White;
    }

}

void graphFuncs::draw( sf::RenderTarget& RT )
{
    RT.draw( xAxis, 6, sf::Lines );
    RT.draw( yAxis, 6, sf::Lines );
    RT.draw( &(vtxVec[0]), vtxVec.size(), sf::LinesStrip );
}

void polyNomial::init( std::istream& is )
{
    graphFuncs::init( is );

    Nterms = C.size();
}

void polyNomial::f( float& y, float& yp, float x )
{
    x -= x0;
    float xPow = 1;// *= x each iteration. and *x for y.
    float Exp = 1.0f;
    y = 0.0f;// will accumulate sum
    yp = 0.0f;

    for( float c : C )
    {
        y += c*xPow*x;
        yp += Exp*c*xPow;
        xPow *= x;
        Exp += 1.0f;
    }
    return;
}
