#ifndef MAKEOBSTACLEMAP_H_INCLUDED
#define MAKEOBSTACLEMAP_H_INCLUDED

#include <vector>
#include "collideSim.h"
#include "get_datum.h"
#include "ball_go.h"
#include "slideBar.h"

class makeObstacleMap : public collideSim
{
    public:
 //   size_t nSegs = 0;
//    collideBase **ppCB = nullptr;

    std::vector<lineSeg*> pLSvec;
    std::vector<lineSegRotate*> pLSRvec;
    std::vector<lineSegElevator*> pLSEvec;
    lineSegElevator* pLSEcurr = nullptr;// used to call set function for vel data member in modes 'C' and 'N'

    std::vector<arcSeg*> pASvec;

    std::vector<slideBar> sliderVec;

    ball B1;// sidelined
    ball_go Bgo;// current in use
    float rollFriction;
    vec2d gravity;

    bool friction_on = false;
    float Cfric = 0.0f;

    bool showPoints = false;
    float dr = 4.0f;

    size_t numReps = 3;// logic loops per render
    bool lbuttHeld = false;// left mouse button down. For ball launch logic
    sf::Vertex pullLine[2];// for launching blue ball
    char operMode = 'T';// 'T'= test with B1. Others are: 'N'= add a New seg, 'C'= change a seg
    bool sliders_inuse = false;
    bool sliders_inuse_last = false;

    // funcs
    makeObstacleMap():collideSim() {}

    virtual bool init( const sf::Font& font );
    virtual void reset();
    virtual bool handleEvent( sf::Event& rEvent );// mouse input launches rB
    virtual void logic();
    virtual void draw( sf::RenderWindow& RW );
    virtual void cleanup();
    virtual ~makeObstacleMap();

    void drawPoints( sf::RenderWindow& RW );
    void loadObstacles( std::ifstream& fin );
    void write_to_file()const;

 //   vec2d oldPt, newPt;// for use in operMode = 'C'
 //   bool oldPt_valid = false;
    void handleClick_change( vec2d pt );
    void handleKey_change( sf::Keyboard::Key K );
    void handleKey_test( sf::Keyboard::Key K );
    bool is_exist_pt( vec2d& pt )const;// writes to pt and returns true if pt within dr

    vec2d ptA, ptB;// for use in operMode = 'N'
    bool got_ptA = false, got_ptB = false;
    char addType = 'n';// 'L', 'A', 'R' = legal values
    static const size_t maxDatum = 4;
    size_t numDatum = 0, idxDatum=0;
    get_datum getDat[maxDatum];
 //   get_datum getDat1;
    void handleClick_addNew( vec2d pt );
    void handleKey_addNew( sf::Keyboard::Key K );

    void set_datum_for_Rotate( lineSegRotate* pS, bool setDefaultVals );
    void set_datum_for_Elevator( lineSegElevator* pS, bool setDefaultVals );
};

bool nearPt( vec2d a, vec2d b, float dR );

#endif // MAKEOBSTACLEMAP_H_INCLUDED
