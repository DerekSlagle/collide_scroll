#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "include/button_types/button.h"
//#include "include/level/Level.h"
#include "include/level/mainMenu.h"
#include "include/level/lvl1.h"
#include "include/level/lvl2.h"
#include "include/level/lvlB.h"
#include "include/level/lvl_platformer.h"
#include "include/level/lvl_shots.h"
#include "include/level/lvl_CGOL.h"
#include "include/level/lvl_Intersect.h"

const unsigned int wdwW = 1280, wdwH = 720;// window dimensions
float Level::winW = (float)wdwW, Level::winH = (float)wdwH;

Level* loadLevel( size_t I );

int main()
{
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) return EXIT_FAILURE;
    button::init( font, 30 );
    mainMenu main_Menu;
    if( !main_Menu.init() ) { std::cout << "Level init fail\n"; return 1; }
    main_Menu.pLoadLvl = loadLevel;
    Level::init_stat( &main_Menu );
    mvHit::wdwW = (float)wdwW;// class static copy. used in update() as window boundaries.
    mvHit::wdwH = (float)wdwH;

    // ready to go!
    sf::RenderWindow RW(sf::VideoMode(wdwW, wdwH), "Levels",  sf::Style::Titlebar | sf::Style::Close);
    RW.setVerticalSyncEnabled(true);
    float dt = 1.0f;

    while (RW.isOpen())
        if( !Level::run_stat( RW, dt ) ) RW.close();

    Level::cleanup_stat();

    return 0;
}

Level* loadLevel( size_t I )
{
    Level* pLvl = nullptr;

    switch( I )
    {
        case 0 :
        pLvl = new lvl1;// tanks
        break;

        case 1 :
        pLvl = new lvl2;// states
        break;

        case 2 :
        pLvl = new lvlB;// explode
        break;

        case 3 :
        pLvl = new lvl_Intersect;
        break;

        case 4 :
        pLvl = new lvl_platformer;
        break;

        case 5 :
        pLvl = new lvl_shots;
        break;

        case 6 :
        pLvl = new lvl_CGOL;
        break;
    }

    if( !pLvl ) return nullptr;
    if( pLvl->init() ) return pLvl;
    else { delete pLvl; return nullptr; }
}
