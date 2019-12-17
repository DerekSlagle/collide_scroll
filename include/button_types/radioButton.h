#ifndef RADIOBUTTON_H_INCLUDED
#define RADIOBUTTON_H_INCLUDED

#include "buttonRect.h"
//#include "../collider/stateTypes.h"

class radioButton : public buttonRect
{
    public:
    std::vector< sf::CircleShape > buttVec;
    sf::CircleShape dot;// in selected circle
    float sep = 1.0f;// distance between buttons
    float r;// button radius
    bool isHoriz = true;
    bool alwaysSel = true;// some button always selected
    size_t selIdx = 0;// index to currently selected
    size_t Nbutts = 1;// = buttVec.size()
    std::function<void(size_t)> pFuncIdx = nullptr;

    static sf::Color bkgdColor, borderColor;

    radioButton(){}
    virtual ~radioButton(){}
    void init( float x, float y, float W, float H, size_t numButts, const sf::Text& Title, int iSel = -1 );
    radioButton( float x, float y, float W, float H, size_t numButts, const sf::Text& Title, int iSel = -1 )
    { init( x, y, W, H, numButts, Title, iSel ); }

    virtual bool hitLeft();
    virtual void setSel( bool Sel ) { sel = Sel || alwaysSel; }
    virtual void setPosition( sf::Vector2f Pos );
    virtual void draw( sf::RenderTarget& RT )const;

    void setIndex( size_t i );

    // for recording state in 1 float
  //  state_ab* newState();
    void setState( const float* pf ) { setIndex( (size_t)pf[0] ); }
    void getState( float* pf )const { *pf = (float)selIdx + 0.5f; }
};


#endif // RADIOBUTTON_H_INCLUDED
