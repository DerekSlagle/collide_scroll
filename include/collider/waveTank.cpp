#include "waveTank.h"

void waveTank::init( std::istream& is )
{
    vDrift.x = vDrift.y = 0.0f;
    wave.is_hard = false;
    is >> pos.x >> pos.y;
    is >> width >> height;

    vec2d xu(1.0f,0.0f), yu(0.0f,1.0f);
    float hfW = width/2.0f, hfH = height/2.0f;
    sideLt.init( pos - hfW*xu + hfH*yu,  pos - hfW*xu - hfH*yu, sf::Color::Cyan );
    sideRt.init( pos + hfW*xu + hfH*yu,  pos + hfW*xu - hfH*yu, sf::Color::Cyan );
    bottom.init( pos - hfW*xu + hfH*yu,  pos + hfW*xu + hfH*yu, sf::Color::Cyan );

    float fluidDepth = 0.0f;
    is >> fluidDepth;
    wave.lineSeg::init( pos - hfW*xu + (hfH-fluidDepth)*yu,  pos + hfW*xu + (hfH-fluidDepth)*yu, sf::Color::Blue );
    wave.Depth = fluidDepth;
    is >> wave.Elev;
    is >> wave.airDensity >> wave.fluidDensity >> wave.grav_N;
    float L_wv = 100.0f;
    is >> wave.Amp_wvRt >> L_wv;
    wave.K_wvRt = waveSeg::PIx2/L_wv;
    is >> wave.wvSpeed;
    wave.rotFreqRt = wave.K_wvRt*wave.wvSpeed;
    is >> wave.Amp_wvLt >> L_wv;
    wave.K_wvLt = waveSeg::PIx2/L_wv;
    wave.rotFreqLt = wave.wvSpeed*wave.K_wvLt;

    is >> wave.Npts_wv;
    wave.magL = wave.len;
    std::cerr << "\n wave points= " << wave.Npts_wv << " len= " << wave.len;
    wave.wvVec.reserve( wave.Npts_wv );
    float dx = wave.len/(float)(wave.Npts_wv-1);
    sf::Vertex vtx;
    vtx.color = sf::Color::Blue;// result is blue
    for( int i=0; i<wave.Npts_wv; ++i )
    {
        float x_rel = dx*(float)i;
        vtx.position.x = wave.pos.x + x_rel;
        vtx.position.y = wave.pos.y + wave.y_wvLt(x_rel) + wave.y_wvRt(x_rel);
        wave.wvVec.push_back( vtx );
    }

    bool bpf = false;
    is >> bpf;
    sideLt.is_bulletProof = sideRt.is_bulletProof = bottom.is_bulletProof = bpf;
    is >> wave.viewBelow;
    if( wave.viewBelow ) { wave.underColor = sf::Color( 0, 50, 205, 30 ); wave.initUnderView(); }
}

void waveTank::update( float dt ) { wave.update(dt); }

/*
bool waveTank::hit( mvHit& mh )
{
    // not simple. Screen cases efficiently
    bool Hit = false;
    if( sideLt.hit(mh) ) Hit = true;
    if( sideRt.hit(mh) ) Hit = true;
    if( bottom.hit(mh) ) Hit = true;

    bool HitWave = wave.hit(mh);
    if( HitWave && !Hit )// mh was hit by wave
    {
        // is mh underwater?
        if( mh.pos.y > wave.pos.y )
        {
            float mhRad = mh.project(vec2d(1,0));
            mh.v += mh.drag*wave.fluidDensity*vDrift*mhRad*2.0f/(mh.m);
        }
    }

    return Hit || HitWave;
}   */


bool waveTank::hit( mvHit& mh )
{
    // not simple. Screen cases efficiently
    bool Hit = false;
    if( sideLt.hit(mh) ) Hit = true;
    if( sideRt.hit(mh) ) Hit = true;
    if( bottom.hit(mh) ) Hit = true;
    if( wave.hit(mh) ) Hit = true;

    // apply drift if mh is in tank
    if( mh.pos.y < wave.pos.y || mh.pos.y > bottom.pos.y ) return Hit;// above or below
    if( mh.pos.x < sideLt.pos.x || mh.pos.x > sideRt.pos.x ) return Hit;// left or right
    float mhRad = mh.project(vec2d(1,0));
    mh.v += mh.drag*wave.fluidDensity*vDrift*mhRad*2.0f/(mh.m);

    return Hit;
}


void waveTank::draw( sf::RenderTarget& RT )const
{
    sideLt.draw(RT);
    sideRt.draw(RT);
    bottom.draw(RT);
    wave.draw(RT);
}
