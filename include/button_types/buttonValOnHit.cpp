#include "buttonValOnHit.h"

bool buttonValOnHit::mseOverAny_delay = false;// delayCnt == moDelay
//int buttonValOnHit::delayCnt = 0;       // is condition for updating & drawing moVals
//sf::Text buttonValOnHit::moValtxt;// real time value
//sf::Vertex buttonValOnHit::moValBox[4];// Quad as bkgd
//const  sf::Font* buttonValOnHit::pFont = nullptr;// about time?
//int buttonValOnHit::moDelay = 30;// about 1/2 second?
sf::Vector2f buttonValOnHit::moValBoxSz(50.0f,15.0f);

// declare local array of 4 rects for base init() call. Prepare as desired
//void buttonValOnHit::init( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, bool isFloat, void* p_val )
void buttonValOnHit::init( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, float* p_val )
{
    // init base members - 2 classes worth
    // button
    mseOver = sel = false;
    pos.x = x; pos.y = y;
    // buttonRect
    sz.x = W; sz.y = H;
    // dimensional stuff
    is_horiz = W > H;
    capLen = 8.0f;
    if( is_horiz )
    {
        if( W < 4.0f*capLen ) W = 4.0f*capLen;
        valLen = W - 2.0f*capLen;
    }
    else// vert
    {
        if( H < 4.0f*capLen ) H = 4.0f*capLen;
        valLen = H - 2.0f*capLen;
    }

 //   initButtRects( R, W, H, sf::Vector2f(x,y), sf::Color::White, sf::Color(88,88,88), sf::Color::White, sf::Color(164,164,164) );
    buttonRect::init(x,y,W,H,Title);
    sz = R.getSize();
    title = Title;
    sf::FloatRect titleSz = title.getLocalBounds();
    sf::Vector2f dPos;
    dPos.x = (sz.x - titleSz.width)/2.0f;
//    dPos.y = (sz.y + titleSz.height)/2.0f;
    dPos.y = sz.y + titleSz.height/2.0f - 6.0f;
    title.setPosition(pos+dPos);

 //   for( size_t i=0; i<4; ++i )
 //       R[i].setPosition(pos);

    title.setColor( sf::Color::Red );

    // finally
    pVal = p_val;
    fVal_min = val_min;
    fVal_max = val_max;
    fVal     = init_val;

    float gap = 5.0f;
    // will provide for_horiz dependence later. Enough chore as is
    marks[0].position.x = pos.x + capLen; marks[0].position.y = pos.y + gap;// min mark
    marks[1].position.x = pos.x + capLen; marks[1].position.y = pos.y + sz.y - gap;// min mark
    marks[0].color = marks[1].color = sf::Color::Black;

    marks[2].position.x = pos.x + sz.x - capLen; marks[2].position.y = pos.y + gap;// max mark
    marks[3].position.x = pos.x + sz.x - capLen; marks[3].position.y = pos.y + sz.y - gap;// max mark
    marks[2].color = marks[3].color = sf::Color::Black;

    marks[4].position.x = pos.x + valLen/3.0f + capLen; marks[4].position.y = pos.y + gap;// max mark
    marks[5].position.x = pos.x + valLen/3.0f + capLen; marks[5].position.y = pos.y + sz.y - gap;// max mark
    marks[4].color = marks[5].color = sf::Color::Red;

    // the value box
    float boxGap = 2.0f;
    float x0 =  pos.x + sz.x + capLen - 3.0f;
    float y0 =  pos.y + boxGap;
    float boxDim = sz.y - 2.0f*boxGap;
    valBox[0].position.x = x0; valBox[0].position.y = y0;// up lt
    valBox[1].position.x = x0 + 2.0f*(boxDim + capLen); valBox[1].position.y = y0;// up rt
    valBox[2].position.x = x0 + 2.0f*(boxDim + capLen); valBox[2].position.y = y0 + boxDim;// dn rt
    valBox[3].position.x = x0; valBox[3].position.y = y0 + boxDim;// dn lt
    valBox[4].position.x = x0; valBox[4].position.y = y0;// up lt
    valBox[0].color = valBox[1].color = valBox[2].color = valBox[3].color = valBox[4].color = sf::Color::Green;

    //   sf::Text txtVal, txtVal_min, txtVal_max;// numeric labels
    txtVal = txtVal_min = txtVal_max = title;
    dPos.x = 0.0f;
    txtVal_min.setPosition(pos+dPos);
    to_SF_string( txtVal_min, fVal_min );
    txtVal_min.setColor( sf::Color::Green );

    dPos.x = sz.x - 20.0f;
    txtVal_max.setPosition(pos+dPos);
    to_SF_string( txtVal_max, fVal_max );
    txtVal_max.setColor( sf::Color::Green );
    // value in box
    dPos.x = sz.x + capLen;
    dPos.y = 2.0f*boxGap;
    txtVal.setPosition(pos+dPos);
    setFval();//setFloatValue( init_val );
    txtVal.setColor( sf::Color::Cyan );
}

float buttonValOnHit::find_fVal()
{
    float s = 0.0f;
    return find_fVal(s);
}

float buttonValOnHit::find_fVal( float& s )// from mouse position
{
    s = button::mseX - ( pos.x + capLen );
    if( s < 0.0f ) s = 0.0f;
    else if( s > valLen ) s = valLen;
    float x = fVal_min + s*( fVal_max - fVal_min )/valLen;
    return x;
}


bool buttonValOnHit::hitLeft()// the magic happens here
{
    if( !button::mseDnLt ) return false;

    if( hit() )
    {
        float s = 0.0f;
        fVal = find_fVal(s);
        setFval(s);
        return true;
    }

    return false;
}

/*
bool buttonValOnHit::MseOver()
{
    mseOver = hit();
    return mseOver;
}   */

void buttonValOnHit::update()
{
    return;
 //   if( mseOver && button::delayCnt >= button::moDelay )
    //        to_SF_string( button::moValtxt, find_fVal() );
}


bool buttonValOnHit::MseOver()
{
    mseOver = false;
    if( hit() )
    {
        if( button::delayCnt >= button::moDelay )
            to_SF_string( button::moValtxt, find_fVal() );

        mseOver = true;
    }

    R.setOutlineColor( (mseOver ? moClr : unMoClr ) );

    return mseOver;
}
/*
bool buttonValOnHit::MseOver()
{
    if( hit() )
    {
        if( button::delayCnt >= button::moDelay )
            to_SF_string( button::moValtxt, find_fVal() );

        mseOver = true;
   //     button::pButtMse = this;
        return true;
    }

    mseOver = false;
 //   if( button::pButtMse == this ) button::pButtMse = nullptr;
    return false;
}
*/

void buttonValOnHit::init_delayBox()
{
    to_SF_string( button::moValtxt, fVal );
    float xLt = pos.x + sz.x/2.0f - 20.0f;
    float yUp = pos.y - sz.y/2.0f - 5.0f;
    button::moValtxt.setPosition( xLt, yUp );
    button::moValtxt.setColor( sf::Color::White );
    button::moValtxt.setCharacterSize( 12 );
    sf::Vector2f& boxSz =  buttonValOnHit::moValBoxSz;
    moValBox[0].position.x = xLt;
    moValBox[0].position.y = yUp;// up lt
    moValBox[1].position.x = xLt + boxSz.x;
    moValBox[1].position.y = yUp;// up rt
    moValBox[2].position.x = xLt + boxSz.x;
    moValBox[2].position.y = yUp + boxSz.y;// dn rt
    moValBox[3].position.x = xLt;
    moValBox[3].position.y = yUp + boxSz.y;// dn lt
 //   moValBox[0].color = moValBox[1].color = moValBox[2].color = moValBox[3].color = sf::Color::Black;
}

void buttonValOnHit::draw( sf::RenderTarget& rRW )const
{
    if( mseOver && button::delayCnt >= button::moDelay )
    {
        rRW.draw( button::moValBox, 4, sf::Quads );
        rRW.draw( button::moValtxt );
    }

    buttonRect::draw(rRW);
    rRW.draw( marks, 6, sf::Lines );
    if( drawValBox ) rRW.draw( valBox, 5, sf::LinesStrip );
    rRW.draw( txtVal_min );
    rRW.draw( txtVal_max );
    rRW.draw( txtVal );

    return;
}

void buttonValOnHit::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - pos;// offset
    for(size_t i = 0; i < 6; ++i )  marks[i].position += dPos;
    for(size_t i = 0; i < 5; ++i )  valBox[i].position += dPos;

    txtVal.setPosition( txtVal.getPosition() + dPos );
    txtVal_min.setPosition( txtVal_min.getPosition() + dPos );
    txtVal_max.setPosition( txtVal_max.getPosition() + dPos );

    buttonRect::setPosition( Pos );
    return;
}

void buttonValOnHit::set_fVal( float f_val )
{
    if( f_val < fVal_min || f_val > fVal_max ) return;
    if( f_val == fVal ) return;
    fVal = f_val;
    setFval();
}

// private member function
void buttonValOnHit::setFval( float s )// operation performed in hitLeft
{
    marks[4].position.x = marks[5].position.x = pos.x + capLen + s;
    to_SF_string( txtVal, fVal );
    if( pVal ) *( static_cast<float*>(pVal) ) = fVal;
    else if( pSetFunc ) pSetFunc( fVal );// new
}

void buttonValOnHit::setFval()// operation performed in setFloatValue()
{
    float s = ( fVal - fVal_min )*valLen/( fVal_max - fVal_min );
    setFval(s);
}
