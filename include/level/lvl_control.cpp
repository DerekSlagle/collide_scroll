#include "lvl_control.h"

bool lvl_control::init( )
{
    Level::clearColor = sf::Color(50,50,50);

//    okTestBox.init( sf::Vector2f( 400.0f, 300.0f ), "Super Important Message Here!" );
//    button::RegisteredButtVec.push_back( &okTestBox );

    ctUtil::init_superList( superList, 300.0f, 30.0f );
    button::RegisteredButtVec.push_back( &superList );
    ctUtil::init_textList( textList, 50.0f, 30.0f );
    button::RegisteredButtVec.push_back( &textList );



    sf::Text txt( "textButt1", *button::pFont, 10);
    txt.setColor( sf::Color(255,0,0) );
    txtButt1.init( 500.0f, 30.0f, 200.0f, 25.0f, txt );
    txtButt1.pStrFunc = std::bind( [](const char* str) { std::cout << "\nTB1: " << str; }, std::placeholders::_1 );
    button::RegisteredButtVec.push_back( &txtButt1 );

    txt.setString("slide1");
    slide1.init( 200, 500, 150, 22, txt, -0.05, 1.0, 0.4, nullptr );
    slide1.setPosition( {100.0f,650.0f} );
    button::RegisteredButtVec.push_back( &slide1 );

    okTestBox.init( sf::Vector2f( 400.0f, 300.0f ), "Super Important Message Here!" );
    button::RegisteredButtVec.push_back( &okTestBox );

    colorPick.init( 200.0f, 300.0f, 200.0f, 220.0f );
    colorPick.is_Modal = true;
 //   colorPick.setSel(true);
    colorPick.OKbutt.pHitFunc = std::bind( [this](void) { B1.img.setFillColor(colorPick.finalColor); } );
    button::RegisteredButtVec.push_back( &colorPick );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );

    B1.r = 10.0f;
    B1.img.setRadius( B1.r );
    B1.img.setOrigin( B1.r, B1.r );
    B1.img.setFillColor( sf::Color(150,0,100) );
    B1.setPosition( vec2d( 300.0f, 500.0f ) );
    B1.v.x = B1.v.y = 0.0f;
    B1.m = 1.0f; B1.Cr = 0.8f;
    Level::pMHfling = &B1;

 //   theLaser.init( B1.pos, vec2d(1.0f,0.0f), 1.0f, sf::Color::Red );

 /*   size_t rows = 13, cols = 27;
    float dx = Level::winW/(float)cols, dy = Level::winH/(float)rows;
    for( size_t r=1; r<=rows; ++r)
        for( size_t c=1; c<=cols; ++c)
        {
            pMvHitVec.push_back( new ball(B1) );
            pMvHitVec.back()->setPosition( vec2d( dx*c, dy*r ) );
        }   */

    // functionSeg
    std::ifstream fin( "include/level/lvl_control_data/funcSeg_data.txt" );
    if( !fin ) { std::cout << "no funcSeg_data\n"; return false; }

    float fa, fb;
    fin >> fa >> fb;
 //   std::function<float(float)> fY = nullptr;// std::bind( [fa,fb](float x) { return fa*(x-fb)*(x-fb); } );
    std::function<float(float)> fY = std::bind( [fa,fb](float x) { return fa*(x-fb)*(x-fb); }, std::placeholders::_1 );
 //   std::function<float(float)> fYpm = nullptr;// = std::bind( [fa,fb](float x) { return 2.0f*fa*(x-fb); } );
    std::function<float(float)> fYpm = std::bind( [fa,fb](float x) { return 2.0f*fa*(x-fb); }, std::placeholders::_1 );
    functionSeg* pFS = new functionSeg( fin, fY, fYpm );
    pSegVec.push_back( pFS );

    return true;
}

bool lvl_control::handleEvent( sf::Event& rEvent )// mouse input launches rB
{
     if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::F3 )
        {
            okTestBox.setSel( !okTestBox.sel );// toggle
        }
        else if ( rEvent.key.code == sf::Keyboard::F4 )// open color picker
        {
            colorPick.setSel( true );
        }
        else if ( rEvent.key.code == sf::Keyboard::F7 )// open color picker
        {
            superList.closeList();
        }
    //    else if ( rEvent.key.code == sf::Keyboard::L ) theLaser.state = true;
    }
 //   else if ( rEvent.type == sf::Event::KeyReleased ) { if ( rEvent.key.code == sf::Keyboard::L ) theLaser.state = false; }

    return true;
}

void lvl_control::update( float dt )// hit test buttons and write to pSelNum
{
    B1.update(dt);
    for( auto& pSeg : pSegVec ) pSeg->hit(B1);

 /*   if( button::mseDnLt && !button::pButtMse && theLaser.state )
    {
        vec2d fireDir( pullLine[1].position.x - pullLine[0].position.x, pullLine[1].position.y - pullLine[0].position.y );
        fireDir /= fireDir.mag();
        theLaser.update( B1.pos, fireDir );
        for( auto& pMh : pMvHitVec ) theLaser.hit(*pMh);
    }
    for( auto& pSeg : pSegVec ) pSeg->hit(B1);

    for( auto& pMh : pMvHitVec ) pMh->update(dt);
    for( auto& pMh : pMvHitVec ) pMh->hit(B1);

    for( size_t i=0; i+1< pMvHitVec.size(); ++i )
        for( size_t j=i+1; j< pMvHitVec.size(); ++j )
                pMvHitVec[i]->hit( *pMvHitVec[j] ); */

    return;
}

void lvl_control::draw( sf::RenderTarget& RT ) const
{
    B1.draw(RT);
 //   for( auto& pMh : pMvHitVec ) pMh->draw(RT);
    for( auto& pSeg : pSegVec ) pSeg->draw(RT);
 //   if( button::mseDnLt && !button::pButtMse && theLaser.state ) theLaser.draw(RT);

    return;
}

void lvl_control::cleanup()
{
    return;
}
