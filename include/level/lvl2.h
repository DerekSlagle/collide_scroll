#ifndef LVL2_H_INCLUDED
#define LVL2_H_INCLUDED

#include "Level.h"

class lvl2 : public Level
{
    public:
  //  bool recorder_on = false;
  //  buttonList recorderControl;
    expandPolygon *pXpgA = nullptr;
 //   lineSegElevator* pLsElev = nullptr;
    buttonList elevControl;

    buttonList flipControl;

 //   buttonRect flipButt;
 //   floatSpot flipSpot;
    floatSpot bumpSpot;

    radioButton selectMH;// do not register
    slideBar sldBar1;// do not register
    buttonValOnHit xpgVelStrip;// do not register
    buttonList miscControl;// manages but does not own above 3 buttons

 //   slideBar timeBar;


    size_t launchIdx = 0;// to support launching a mvHit
    size_t numReps = 1;// logic loops per render.
    std::vector<sf::Text> msgVec;
    sf::Text helpMsg;
    bool showMsgs = false;

    // functions
    lvl2():Level() { std::cout << "Hello from lvl2 ctor\n"; }// if( !init() ) std::cout << "bad init\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );// mouse input launches rB
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    virtual void cleanup();
    virtual ~lvl2() { cleanup(); }

    void init_rec( std::vector<state_ab*>& p_stateVec );
    void assignMessages();
};

#endif // LVL2_H_INCLUDED
