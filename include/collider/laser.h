#ifndef LASER_H_INCLUDED
#define LASER_H_INCLUDED

#include "../pathMover.h"

class laser
{
    public:
    static float beamLength;

    vec2d pos;
    vec2d dir;// unit
    float damage;
    sf::Vertex beam[2];// to edge of window
    bool state;// on/off

    laser(){}
    ~laser(){}
    void init( vec2d Pos, vec2d Dir, float Damage, sf::Color beamColor );

    void update( vec2d Pos, vec2d Dir );
    bool hit( mvHit& );
    bool hit( target& );
    void draw( sf::RenderTarget& RT ) const { if( state ) RT.draw( beam, 2, sf::Lines ); }
};

#endif // LASER_H_INCLUDED
