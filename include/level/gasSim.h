#ifndef GASSIM_H_INCLUDED
#define GASSIM_H_INCLUDED

#include "collideSim.h"

class gasSim : public collideSim
{
    public:
    size_t numReps = 2;// logic loops per render
    ball* pB;
    size_t nBalls;
//    collideBase* pCol;
    lineSeg* pCol;
    size_t nWallSegs;
    bool valve_open;

    lineSeg sink;
    int sinkMode = 0;// -1=cool, 0=no effect, +1=heat
    sf::Color sinkClr;
    float coolCoeff, heatCoeff;// mult by ball::v

    float membraneX;

    sf::Text numltMsg, numltQtyMsg, EltMsg, EltQtyMsg, EltAvgMsg, EltAvgQtyMsg;
    sf::Text numrtMsg, numrtQtyMsg, ErtMsg, ErtQtyMsg, ErtAvgMsg, ErtAvgQtyMsg;

    // functions
    gasSim():collideSim(), pB(nullptr), pCol(nullptr) {}

    virtual bool init( const sf::Font& font );
    virtual void reset();
    virtual bool handleEvent( sf::Event& rEvent );// mouse input launches rB
    virtual void logic();
    virtual void draw( sf::RenderWindow& RW );
    virtual void cleanup();
    virtual ~gasSim();

};

#endif // GASSIM_H_INCLUDED
