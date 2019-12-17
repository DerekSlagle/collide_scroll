#ifndef OKBOX_H_INCLUDED
#define OKBOX_H_INCLUDED

# include "button.h"

class okBox : public button
{
    public:
    /* inherited
    bool mseOver, sel;
    bool inDefView = true;// default view. Apply no offset
    char mode = 'T';// 'T' = toggle, 'I' = intermittent (NOHC)
    sf::Vector2f pos;
    std::function<void(void)> pHitFunc = nullptr;
    */

    sf::RectangleShape bkgdRect, okButt;
    sf::Text msg, okMsg;
    sf::Color noMoColor = sf::Color::Blue, moColor = sf::Color::Green;
    sf::Color bkgdColor = sf::Color(188,188,188);
    sf::Color buttColor = sf::Color::White;

    sf::Color shadeColor = sf::Color( 0, 0, 200, 50 );
    sf::RectangleShape shadeRect;

    void init( sf::Vector2f Pos, const char* pMsg );
    okBox( sf::Vector2f Pos, const char* pMsg ) { init( Pos, pMsg ); }
    okBox() {}
    virtual void setSel( bool Sel );// over in buttonList to open/close list
    virtual bool hit()const;
    virtual void draw( sf::RenderTarget& RT )const;
    virtual bool MseOver();// assigns mseOver - virtual due to ValOnHit live display feature
    virtual bool hitLeft();// alters sel - overload in ValOnHit
 //   virtual bool hitRight() { return mseOver; }// overload in joyButton
    virtual void setPosition( sf::Vector2f Pos );
    virtual ~okBox(){}
};

#endif // OKBOX_H_INCLUDED
