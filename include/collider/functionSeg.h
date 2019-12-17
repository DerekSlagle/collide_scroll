#ifndef FUNCTIONSEG_H_INCLUDED
#define FUNCTIONSEG_H_INCLUDED

#include<vector>
#include "lineSeg.h"

class functionSeg : public lineSeg
{
    public:
    vec2d Lu;// unit vector. N is other

    // draw the curve
    int Npts_fx = 100;
    std::vector<sf::Vertex> vtxVec;
    bool viewSeg = false;

    // func pointers
    std::function<float(float)> pfY;
    std::function<float(float)> pfYpm;

    // funcs
    functionSeg() {}// defs here
    virtual ~functionSeg() {}
    // defs in cpp


//    virtual void init( std::istream& is );
//    functionSeg( std::istream& is ) { init(fin); }
    void init( std::istream& is, std::function<float(float)> p_fY, std::function<float(float)> p_fYpm );
    functionSeg( std::istream& is, std::function<float(float)> p_fY, std::function<float(float)> p_fYpm )  { init(is,p_fY,p_fYpm); }

 //   virtual void to_file( std::ofstream& fout );
    virtual void draw( sf::RenderTarget& )const;
//    virtual void update( float dt );
//    virtual state_ab* newState();// static object has no dynamic state

    virtual bool hit( mvHit& );

};


#endif // FUNCTIONSEG_H_INCLUDED
