#ifndef LVL_CONTROL_H_INCLUDED
#define LVL_CONTROL_H_INCLUDED

#include "Level.h"
#include "../collider/functionSeg.h"
#include "../collider/laser.h"

class lvl_control : public Level
{
    public:
    size_t launchIdx = 0;// to support launching a mvHit
    vec2d gravity;
    bool gravity_on = false;

    buttonList superList;
    buttonList textList;
    textButton txtButt1;
    slideBar slide1;
    okBox okTestBox;
    colorPicker colorPick;

    ball B1;
 //   laser theLaser;

    // functions
    lvl_control():Level() { std::cout << "Hello from lvl_control ctor\n"; }
    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    virtual void cleanup();
    virtual ~lvl_control() { cleanup(); }
};

#endif // LVL_CONTROL_H_INCLUDED
