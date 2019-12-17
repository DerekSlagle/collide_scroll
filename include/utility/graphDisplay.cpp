#include "graphDisplay.h"

graphDisplay::graphDisplay()
{
    //ctor
}

graphDisplay::~graphDisplay()
{
    //dtor
}

void graphDisplay::init( sf::Vector2f Pos, sf::Vector2f Sz, float(*func)(float), float xmin, float xmax, float ymin, float ymax, sf::Color bkClr, sf::Color graphClr )
{
    F = func;
    u_min = xmin;
    u_max = xmax;
    v_min = ymin;
    v_max = ymax;// domain and range. Scale determined
    pos = Pos;
    sz = Sz;
    bkRect.setPosition(Pos);
    bkRect.setSize(Sz);
    bkRect.setFillColor( bkClr );

    u_scale = (u_max - u_min)/sz.x;
    x_ogn = -u_min/u_scale;

    v_scale = (v_max - v_min)/sz.y;
    y_ogn = -v_min/v_scale;

    // sf::Vertex axes[8] is 2 sf::Quads 0-3 = x-axis, 3-7 = y-axis
    float de = 2.0f;// edge space
    size_t i = 0;// looping

    // x-axis
    axes[0].position.x = de;        axes[0].position.y = y_ogn - 1.0f;// UpLt
    axes[1].position.x = sz.x - de; axes[1].position.y = y_ogn - 1.0f;// UpRt
    axes[2].position.x = sz.x - de; axes[2].position.y = y_ogn + 1.0f;// BtRt
    axes[3].position.x = de;        axes[3].position.y = y_ogn + 1.0f;// BtLt
     // y-axis
    axes[4].position.x = x_ogn - 1.0f; axes[4].position.y = de;       // UpLt
    axes[5].position.x = x_ogn + 1.0f; axes[5].position.y = de;       // UpRt
    axes[6].position.x = x_ogn + 1.0f; axes[6].position.y = sz.y - de;// BtRt
    axes[7].position.x = x_ogn - 1.0f; axes[7].position.y = sz.y - de;// BtLt

    for(i=0; i<8;++i)
    {
        axes[i].position.x += pos.x;
        axes[i].position.y = sz.y - axes[i].position.y;// xform upright
        axes[i].position.y += pos.y;
        axes[i].color = graphClr;
    }

    vtxVec.clear();
    for( float x = 0.0f; x < sz.x; x += 1.0f )
    {
        float u = u_min + x*u_scale;
        float v = F(u);
        sf::Vertex vtx;
        vtx.color = graphClr;
        vtx.position.x = x + pos.x;
        vtx.position.y = (v-v_min)/v_scale;
        vtx.position.y = sz.y - vtx.position.y;// xform upright
        vtx.position.y += pos.y;
        vtxVec.push_back(vtx);
    }
}

void graphDisplay::update( float t, float (*w)(float,float) )// eg: w = k*x - f*t
{
    for( sf::Vertex& vtx : vtxVec )
    {
        float u = u_min + (vtx.position.x-pos.x)*u_scale;
        float v = F( w(u,t) );
        vtx.position.y = (v-v_min)/v_scale;
        vtx.position.y = sz.y - vtx.position.y;// xform upright
        vtx.position.y += pos.y;
    }
}

void graphDisplay::draw( sf::RenderWindow& rRW )
{
    rRW.draw(bkRect);
    rRW.draw( axes, 8, sf::Quads );
//    rRW.draw( &vtxVec[0], vtxVec.size(), sf::Points );
    rRW.draw( &vtxVec[0], vtxVec.size(), sf::LinesStrip );
}

// sample code
/*
float F1( float x ){ return 5.0f*sinf(x); }// to init()
float w1( float x, float t ) { return x - t; }// to update()

// in main()
static float a = 0.0f;
    const float da = 0.1f;
    a += da;
    if( a > 6.28f ) a -= 6.28f;
graphDisplay grfDisp;
grfDisp.init( sf::Vector2f(50.0f,50.0f),sf::Vector2f(400.0f, 400.0f), F1, -10.0f, 10.0f, -12.0f, 12.0f, sf::Color::Cyan, sf::Color::Black );
// update and draw
grfDisp.update( t, w1 );
//...
grfDisp.draw(RW);

*/
