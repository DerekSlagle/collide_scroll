#ifndef LVL_CGOL_H_INCLUDED
#define LVL_CGOL_H_INCLUDED

#include "Level.h"

// Conways Game of Life simulation
class lvl_CGOL : public Level
{
    public:
    std::vector< std::vector<bool> > stA, stB;// alternating which state is current generation. Will reproduce into other.
    size_t cols, rows;// also vector dimensions
    int top=0, bottom=0, left=0, right=0;
    float wCell, hCell;// for drawing
    sf::Vertex liveBox[5];
//    sf::RectangleShape cellRect;
    bool A_isCurrent = true;// in use?
    bool anyAlive = false;
    float genPeriod, genTime = 0.0f;
    buttonValOnHit genPeriodStrip;

    int genCount = 0;
    sf::Text genCntMsg, genCntMsgNum;

    // multi game list
    std::vector<std::string> gameFileNameVec;
    buttonList gameList;

    // functions
    lvl_CGOL():Level() { std::cout << "Hello from lvl_CGOL ctor\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    virtual void cleanup();
    virtual ~lvl_CGOL() { cleanup(); }

    bool loadGame( const std::string& fName );
};

#endif // LVL_CGOL_H_INCLUDED
