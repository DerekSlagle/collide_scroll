#ifndef GRAPHFUNCS_H_INCLUDED
#define GRAPHFUNCS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include<vector>
//#include<functional>
//#include <string>
#include "vec2d.h"

struct graphFuncs
{
    std::vector<sf::Vertex> vtxVec;
    sf::Vertex xAxis[6];// includes nifty arrow in +x direction
    sf::Vertex yAxis[6];// includes nifty arrow in +y direction
    std::vector<float> C;
    float xLt, xRt, x0;// x0 = expansion center

    float length;
    float sclX = 1.0f, sclY = 1.0f;
    float dxVtx = 3.0f;// distance between sf::Vertex

    vec2d pos;// base position corresponds to x = xLt, y = 0

    virtual void f( float& y, float& yp, float x ) = 0;
    void draw( sf::RenderTarget& RT );// draws axes + scaled and positioned function
    graphFuncs() {}
    void init( std::istream& is );
    graphFuncs( std::istream& is ) { init(is); }
    virtual ~graphFuncs() {}
};

struct polyNomial : graphFuncs
{
    size_t Nterms = 0;
    void init( std::istream& is );
    polyNomial( std::istream& is ) { init(is); }
    virtual void f( float& y, float& yp, float x );
    virtual ~polyNomial() {}
};


#endif // GRAPHFUNCS_H_INCLUDED
