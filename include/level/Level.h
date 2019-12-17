#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <fstream>
#include <iostream>

//#include "../button_types/button.h"
#include "../button_types/controlUtils.h"
#include "../collider/collideHelperFuncs.h"
//#include "../collider/mvHit.h"
//#include "../collider/segHit.h"

class Level
{
    public:
    static bool run_update, step_update;
    static Level* pCurrLvl;
    static Level* pMM;

    static float winW, winH;
    static float mapW, mapH;
    static float viewOfstX, viewOfstY;
    static sf::View mapView;
    static bool usingMapView;
    static sf::Color clearColor;

    static bool recorder_on;
    static buttonList recorderControl;
    static buttonRect goto_MMButt;

    // support other common stuff
    static sf::Vertex pullLine[2];
    static float flingVelGrad;
    static sf::CircleShape flingMark;
    static mvHit* pMHfling;
    static sf::Text pauseMsg;

    static std::vector<segHit*> pSegVec;
    static std::vector<mvHit*> pMvHitVec;

    static bool handleEvent_stat( sf::Event& rEvent );
    static void init_stat( Level* p_MnMenu );
    static void update_stat( float dt );
    static void draw_stat( sf::RenderTarget& RT );
    static bool run_stat( sf::RenderWindow& RW, float dt );// does it all
    static void cleanup_stat() { if( pCurrLvl && ( pCurrLvl != pMM ) ) delete pCurrLvl; }
    static void flingLineUpdate();
    static void goto_MM();

    Level();

    // pure virtual funcs
    virtual bool init() = 0;
    virtual void reset() { return; }// use in mainMenu

    virtual bool handleEvent( sf::Event& rEvent ) = 0;
    virtual void update( float dt ) = 0;// calls virtual logic()
    virtual void draw( sf::RenderTarget& RT ) const = 0;
    virtual void draw_map( sf::RenderTarget& RT ) const { (void)RT; return; }

    virtual void cleanup() = 0;
    virtual ~Level();
};

bool hitRect( sf::Rect<float> hitRect, sf::Vector2f pt );


#endif // LEVEL_H_INCLUDED
