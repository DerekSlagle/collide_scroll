#include "okBox.h"
//#include "textButton.h"

void okBox::init( sf::Vector2f Pos, const char* pMsg )
{
    if( button::pFont )
    {
        msg.setFont( *button::pFont );
        msg.setCharacterSize(16);
        msg.setString( pMsg );
        msg.setColor( sf::Color::Black );
        okMsg.setFont( *button::pFont );
        okMsg.setCharacterSize(20);
        okMsg.setString( "OK" );
        okMsg.setColor( sf::Color::Black );
    }

    mseOver = sel = false;
    pHitFunc = nullptr;
    pos = Pos;
    shadeColor = sf::Color( 0, 0, 200, 50 );

    bkgdRect.setPosition( Pos );
    bkgdRect.setSize( sf::Vector2f( msg.getLocalBounds().width + 40.0f, 90.0f ) );
    bkgdRect.setFillColor( bkgdColor );
    bkgdRect.setOutlineColor( noMoColor );
    bkgdRect.setOutlineThickness( 3.0f );
    sf::Vector2f dPos( (bkgdRect.getLocalBounds().width - msg.getLocalBounds().width)/2.0f, 10.0f );
    msg.setPosition( Pos + dPos );

    okButt.setSize( sf::Vector2f( 50.0f, 30.0f ) );
    dPos.x = bkgdRect.getLocalBounds().width - okButt.getLocalBounds().width; dPos.y = bkgdRect.getLocalBounds().height - okButt.getLocalBounds().height + 20.0f;
    dPos /= 2.0f;
    okButt.setPosition( Pos + dPos );
    okButt.setFillColor( buttColor );
    okButt.setOutlineColor( noMoColor );
    okButt.setOutlineThickness( 2.0f );
    dPos.x = okButt.getLocalBounds().width - okMsg.getLocalBounds().width; dPos.y = okButt.getLocalBounds().height - okMsg.getLocalBounds().height;
    dPos /= 2.0f; dPos.x -= 5.0f; dPos.y -= 5.0f;
    okMsg.setPosition( okButt.getPosition() + dPos  );

    shadeRect.setPosition( sf::Vector2f( 0.0f, 0.0f ) );
    shadeRect.setSize( sf::Vector2f( 1500.0f, 1000.0f ) );
    shadeRect.setFillColor( shadeColor );
}

void okBox::setSel( bool Sel )
{
    sel = mseOver = Sel;

    if( sel )// steal focus
    {
     //   if( button::pButtMse ) { button::pButtMse->mseOver = false; button::pButtMse->setSel(false); button::pButtMse = this; }
     //   if( button::pTxtButtFocus ) { button::pTxtButtFocus->setSel(false); button::pTxtButtFocus = nullptr; }
        grabFocus( this );
        bkgdRect.setOutlineColor( noMoColor );
        okButt.setOutlineColor( noMoColor );
    }
    else// surrender focus
    {
        if( button::pButtMse == this ) button::pButtMse = nullptr;
    }
}

bool okBox::hit()const
{
    return mseOver;
}

void okBox::draw( sf::RenderTarget& RT )const
{
    if( sel )
    {
        RT.draw(shadeRect);
        RT.draw(bkgdRect);
        RT.draw(okButt);
        RT.draw(okMsg);
        RT.draw(msg);
    }
}

bool okBox::MseOver()
{
    if( mseOver )// okBox is open
    {
        if( hitFloatRect( bkgdRect.getGlobalBounds() ) )
        {
            bkgdRect.setOutlineColor( moColor );
            // mseover okButt
            if( hitFloatRect( okButt.getGlobalBounds() ) ) okButt.setOutlineColor( moColor );
            else okButt.setOutlineColor( noMoColor );
        }
        else bkgdRect.setOutlineColor( noMoColor );
    }

    return mseOver;
}

bool okBox::hitLeft()
{
    if( sel && hitFloatRect( okButt.getGlobalBounds() ) )// close okBox
    {
        sel = mseOver = false;
    //    if( button::pButtMse == this ) button::pButtMse = nullptr;// causes immediate crash
        if( pHitFunc ) pHitFunc();
    }

    return mseOver;
}

void okBox::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - pos;
    bkgdRect.setPosition( bkgdRect.getPosition() + dPos );
    okButt.setPosition( okButt.getPosition() + dPos );
    msg.setPosition( msg.getPosition() + dPos );
    okMsg.setPosition( okMsg.getPosition() + dPos );
}
