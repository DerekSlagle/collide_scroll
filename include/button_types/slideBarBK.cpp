#include "slideBar.h"

float slideBar::grabX = 0.0f;

void slideBar::init( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, float* p_val )
{
     mseOver = sel = false;
    pos.x = x; pos.y = y;
    // buttonRect
    sz.x = W; sz.y = H;

    initButtRects( R, W, H, sf::Vector2f(x,y), sf::Color::White, sf::Color(88,88,88), sf::Color::White, sf::Color(164,164,164) );
    sz = R->getSize();
    title = Title;
    sf::FloatRect titleSz = title.getLocalBounds();
    sf::Vector2f dPos;
    dPos.x = (sz.x - titleSz.width)/2.0f;
//    dPos.y = (sz.y + titleSz.height)/2.0f;
    dPos.y = -2.0f*titleSz.height;
    title.setPosition(pos+dPos);

    for( size_t i=0; i<4; ++i )
        R[i].setPosition(pos);

    title.setColor( sf::Color::Red );

    // finally
    pVal = p_val;
    fVal_min = val_min;
    fVal_max = val_max;
    fVal     = init_val;

    // the slide button
    float hButt = sz.y - 2.0f*b;
    wButt = 2.0f*hButt;
    float capLen = b + wButt/2.0f;
    L = sz.x - 2.0f*capLen;
    float s = L*( fVal - fVal_min )/( fVal_max - fVal_min );
    s += capLen;// rel to left end

    sldButt.setSize( sf::Vector2f(wButt,hButt) );
    sldButt.setPosition( pos.x + s, pos.y + b );
    sldButt.setOutlineThickness( 2.0f );
    sldButt.setFillColor( sf::Color::White );
    sldButt.setOutlineColor( sf::Color::Blue );

    marks[0].position.x = pos.x + capLen; marks[0].position.y = pos.y + b;// min mark
    marks[1].position.x = pos.x + capLen; marks[1].position.y = pos.y + sz.y - b;// min mark
    marks[0].color = marks[1].color = sf::Color::Black;

    marks[2].position.x = pos.x + sz.x - capLen; marks[2].position.y = pos.y + b;// max mark
    marks[3].position.x = pos.x + sz.x - capLen; marks[3].position.y = pos.y + sz.y - b;// max mark
    marks[2].color = marks[3].color = sf::Color::Black;

    marks[4].position.x = pos.x + capLen; marks[4].position.y = pos.y + sz.y/2.0f;
    marks[5].position.x = pos.x + sz.x - capLen; marks[5].position.y = pos.y + sz.y/2.0f;
    marks[4].color = marks[5].color = sf::Color::Black;

    // the 3 texts
    valStr.setFont( *button::pFont );
    valStr.setCharacterSize( 10 );
    sf::Vector2f POS = pos;
    POS.x += sz.x/2.0f; POS.y += sz.y;
    valStr.setPosition(POS);
    float_to_Text( valStr, fVal );
    valStr.setColor( sf::Color::Green );

    minValStr = maxValStr = valStr;
    POS = pos; POS.y += sz.y;
    minValStr.setPosition(POS);
    float_to_Text( minValStr, fVal_min );
    minValStr.setColor( sf::Color::Green );
    maxValStr.setPosition(pos+sz);
    float_to_Text( maxValStr, fVal_max );
    maxValStr.setColor( sf::Color::Green );

}

void slideBar::set_fVal()// fVal already assigned. Set sldButt position
{
    if( pVal ) *pVal = fVal;
    else if( pSetFunc ) pSetFunc(fVal);
    float s = L*( fVal - fVal_min )/( fVal_max - fVal_min );
    s += b;// rel to left end
    sldButt.setPosition( pos.x + s, pos.y + b );
    float_to_Text( valStr, fVal );
}
// set sldButt position programaticaly
void slideBar::set_fVal( float y )
{
    // clamp value
    if( y < fVal_min ) y = fVal_min;
    else if( y > fVal_max ) y = fVal_max;
    fVal = y;
    set_fVal();
}

float slideBar::find_s()// from mouse position
{
    return button::mseX - ( grabX + pos.x + wButt/2.0f + b );
}

bool slideBar::hitLeft()
{
 //   if( !(button::mseDnLt || button::mseDnRt) ) return false;
    if( !hitR( sldButt ) ) { sel = false; return false; }

    if( button::mseDnLt )// grab
    {
        sel = true;
        std::cerr << "\n sel = true";
        grabX = button::mseX - sldButt.getPosition().x;
        float s = find_s();
        float y = s*( fVal_max - fVal_min )/L + fVal_min;
        set_fVal(y);
        return true;
    }
    else// release
    {
        std::cerr << "\n sel = false";
        sel = false;
        float s = find_s();
        float y = s*( fVal_max - fVal_min )/L + fVal_min;
        set_fVal(y);
        return true;
    }

    return false;
}


bool slideBar::MseOver()
{
    if( hit() )
    {
        mseOver = true;
        button::mseOverAny = true;
     //   buttonValOnHit::mseOverAny_delay = true;
        button::pButtMse = this;

        if( sel && hitR( sldButt ) )
     //   if( hitR( sldButt ) )
        {
            float s = find_s();
            float y = s*( fVal_max - fVal_min )/L + fVal_min;
            set_fVal(y);
        }

        return true;
    }

    return false;
}

void slideBar::draw( sf::RenderTarget& rRW )const
{
    buttonRect::draw(rRW);
    rRW.draw( marks, 6, sf::Lines );
    rRW.draw( sldButt );
    rRW.draw( valStr );
    rRW.draw( minValStr );
    rRW.draw( maxValStr );
}

void slideBar::setPosition( sf::Vector2f Pos )
{

}

// non members
float float_to_Text(sf::Text& strArg, float x)// returns position of end of strArg
{
    std::stringstream ss;
    ss << x;
    std::string tempStr;
    ss >> tempStr;
    strArg.setString( tempStr.c_str() );
    sf::FloatRect fr = strArg.getLocalBounds();
    return fr.left + fr.width;
}

bool hitR( const sf::RectangleShape& RS )
{
    sf::Vector2f sbSz = RS.getSize();
    sf::Vector2f sbPos = RS.getPosition();
    if(  button::mseX < sbPos.x || button::mseX > sbPos.x + sbSz.x ) return false;// off end of sldButt
    else if( button::mseY < sbPos.y || button::mseY > sbPos.y + sbSz.y ) return false;
    return true;
}
