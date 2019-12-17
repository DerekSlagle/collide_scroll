#ifndef MAINMENU_H_INCLUDED
#define MAINMENU_H_INCLUDED

#include<string>
#include "Level.h"

class mainMenu : public Level
{
    public:
    std::vector<sf::Text> msgVec;
    std::vector< sf::Rect<float> > rectVec;
    sf::Color msgClrReg;
    sf::Color msgClrMseOver;
    size_t numOptions = 0;
    size_t lvlNum = 0;
    std::function<Level*(size_t)> pLoadLvl = nullptr;

    okBox levelBox;

    // functions
    //mainMenu( int* p_selNum ):Level(), pSelNum(p_selNum) { std::cout << "Hello from mainMenu ctor\n"; }
    mainMenu():Level() { std::cout << "Hello from mainMenu ctor\n"; }

    virtual bool init();
    virtual void reset();

    virtual bool handleEvent( sf::Event& rEvent );// mouse input launches rB
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    virtual void cleanup();
    virtual ~mainMenu();
};

#endif // MAINMENU_H_INCLUDED
