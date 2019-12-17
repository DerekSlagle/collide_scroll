#ifndef PLATFORMEXTRAS_H_INCLUDED
#define PLATFORMEXTRAS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "../vec2d.h"
#include "../pathMover.h"
#include "../collider/mvHit.h"
#include "../collider/ball.h"
#include "../collider/shotTypes.h"

struct pickItem
{
    float value = 1.0f;
    float r = 1.0f;
    bool inUse = true;
    sf::Vector2f pos;
    std::vector<sf::Vertex> vtxVec;
    void init( size_t Nsides, float R, float Value, sf::Vector2f Pos, sf::Color clr = sf::Color::Red );
    pickItem( size_t Nsides, float R, float Value, sf::Vector2f Pos, sf::Color clr = sf::Color::Red ) { init( Nsides, R, Value, Pos, clr ); }
    pickItem( size_t Nsides, float R, float Value, vec2d Pos, sf::Color clr = sf::Color::Red ) { init( Nsides, R, Value, Equate(Pos), clr ); }
    void draw( sf::RenderTarget& RT )const { if( inUse ) RT.draw( &(vtxVec[0]), vtxVec.size(), sf::LinesStrip ); }
    bool hit( mvHit& mh ) const;
};

#endif // PLATFORMEXTRAS_H_INCLUDED
