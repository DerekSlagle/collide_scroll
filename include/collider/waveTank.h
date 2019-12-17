#ifndef WAVETANK_H_INCLUDED
#define WAVETANK_H_INCLUDED

#include "waveSeg.h"
#include "mvHit.h"

class waveTank
{
    public:
    lineSeg sideLt, sideRt, bottom;
    waveSeg wave;
    float height, width;// Depth is waveSeg property
    vec2d pos;
    vec2d vDrift;

    // funcs
    waveTank() {}
    ~waveTank() {}
    void init( std::istream& );
    void update( float dt );
    bool hit( mvHit& mh );
    void draw( sf::RenderTarget& )const;

};

#endif // WAVETANK_H_INCLUDED
