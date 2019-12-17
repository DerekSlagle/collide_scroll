#include "Level.h"

bool Level::run_update = true;
bool Level::step_update = true;
Level* Level::pCurrLvl = nullptr;
Level* Level::pMM = nullptr;
mvHit* Level::pMHfling = nullptr;
sf::View Level::mapView;
float Level::mapW = 20.0f, Level::mapH = 20.0f;
float Level::viewOfstX = 0.0f, Level::viewOfstY = 0.0f;
bool Level::usingMapView = false;
sf::Color Level::clearColor(0,0,0);
bool Level::recorder_on = false;
buttonList Level::recorderControl;
buttonRect Level::goto_MMButt;
sf::Text Level::pauseMsg;
sf::Vertex Level::pullLine[2];
float Level::flingVelGrad = 0.15f;
sf::CircleShape Level::flingMark;
std::vector<segHit*> Level::pSegVec;
std::vector<mvHit*> Level::pMvHitVec;

Level::Level()
{
    run_update = true;
    step_update = true;
    recorder_on = false;
    Level::usingMapView = false;
    Level::clearColor = sf::Color::Black;
    for( size_t i=0; i<4; ++i ) button::moValBox[i].color = Level::clearColor;
    viewOfstX = 0.0f; viewOfstY = 0.0f;
    pMHfling = nullptr;
    goto_MMButt.setPosition( { (winW-goto_MMButt.sz.x)/2.0f,winH - 40.0f } );
}


Level::~Level()
{
    run_update = true;
    step_update = true;
    recorder_on = false;
    viewOfstX = 0.0f; viewOfstY = 0.0f;
    Level::usingMapView = false;
    pMHfling = nullptr;
    mvHit::wdwW = winW;// class static copy. used in update() as window boundaries.
    mvHit::wdwH = winH;

    mvHit::wdwCf = 0.0f;
    mvHit::windowTopOpen = false;
    Level::clearColor = sf::Color::Black;

    button::pButtMse = nullptr;// new
    button::pTxtButtFocus = nullptr;// new

    if( pSegVec.size() > 0 ) { for( auto& x : pSegVec ) delete x; pSegVec.clear(); }
    if( pMvHitVec.size() > 0 ) { for( auto& x : pMvHitVec ) delete x; pMvHitVec.clear(); }

    if( button::RegisteredButtVec.size() > 0 )
    {
        std::cout << "\n release " << button::RegisteredButtVec.size() << " controls";
        button::RegisteredButtVec.clear();
    }

    rec::cleanup();
    recorderControl.closeList();
    if( recorderControl.ownButts ){ std::cout << "\ndeleting " << recorderControl.pButtVec.size() << " recorder butts "; for( auto& butt : recorderControl.pButtVec ) delete butt; }
    recorderControl.pButtVec.clear();
}

void Level::init_stat( Level* p_MnMenu )
{
    Level::pCurrLvl = Level::pMM = p_MnMenu;
    float r = 3.0f;
    flingMark.setRadius(r);
    flingMark.setFillColor( sf::Color::Cyan );
    flingMark.setOrigin( r,r );
    pullLine[0].color = pullLine[1].color = sf::Color(0,0,255);

    if( button::pFont )
    {
        pauseMsg.setFont( *button::pFont );
        pauseMsg.setColor( sf::Color::Red );
        pauseMsg.setCharacterSize(30);
        pauseMsg.setString( "Pause" );
        pauseMsg.setPosition( 1050.0f, 20.0f );
        sf::Text label("goto MM", *button::pFont, 12);
        label.setColor( sf::Color(0,0,255) );
        goto_MMButt.init( 1000.0f, 30.0f, 60.0f, 28.0f, label );
   //     goto_MMButt.init( (winW-goto_MMButt.sz.x)/2.0f, winH-40.0f, 60.0f, 28.0f, label );
        goto_MMButt.pHitFunc = [](){ Level::goto_MM(); };
    }
}

bool Level::run_stat( sf::RenderWindow& RW, float dt )
{
    button::reset();

    sf::Event event;
    while (RW.pollEvent(event))
        if( !Level::handleEvent_stat( event ) ) return false;

    Level::update_stat(dt);
    // draw
    RW.clear( clearColor );// sf::Color(0,64,128) )
    Level::draw_stat(RW);
    RW.display();

    return true;
}

void Level::goto_MM()
{
    if( pCurrLvl && ( pCurrLvl != pMM ) ) delete pCurrLvl;
    pCurrLvl = pMM;
    pMM->reset();
}

bool Level::handleEvent_stat( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::Closed ) return false;

    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::Escape ) return false;
        else if ( rEvent.key.code == sf::Keyboard::F1 ) { run_update = !run_update; }// toggle play/pause
        else if ( rEvent.key.code == sf::Keyboard::F2 ) { step_update = true; }// one step only if run_update==false
        else if ( rEvent.key.code == sf::Keyboard::M )
        {
            if( !button::pTxtButtFocus )
            {
            //    if( pCurrLvl && ( pCurrLvl != pMM ) ) delete pCurrLvl;
            //    pCurrLvl = pMM;
            //    pMM->reset();
                if( pCurrLvl != pMM ) goto_MM();
                return true;
            }
        }
    }

    button::handleEvent( rEvent );

    if( !pCurrLvl ) return true;
    return pCurrLvl->handleEvent( rEvent );
}

void Level::update_stat( float dt )
{
    if( !pCurrLvl ) return;
 //   if( run_update || step_update )
 //   {
 //   button::MseOverAll( viewOfstX, viewOfstY );
    if( Level::usingMapView ) button::MseOverAll( viewOfstX, viewOfstY );
    else button::MseOverAll();
    if( recorder_on ){ run_update = rec::update(); ctUtil::updateRecorderDisplay(); }
    if( run_update || step_update ) pCurrLvl->update(dt);
    flingLineUpdate();
  //  }
    step_update = false;
}

void Level::draw_stat( sf::RenderTarget& RT )
{
    if( !pCurrLvl ) return;

    if( usingMapView )
    {
         RT.setView( mapView );
         if( pMHfling && button::mseDnLt && !button::pButtMse ) RT.draw(pullLine, 2, sf::Lines);
         pCurrLvl->draw_map(RT);
         if( pMHfling ) RT.draw( flingMark );
         button::drawAll_map(RT);
         RT.setView( RT.getDefaultView() );
         pCurrLvl->draw(RT);
    }
    else
    {
        if( pMHfling && button::mseDnLt && !button::pButtMse ) RT.draw(pullLine, 2, sf::Lines);
        pCurrLvl->draw(RT);
        if( pMHfling ) RT.draw( flingMark );
    }

    button::drawAll_def(RT);
    if( recorder_on ) ctUtil::drawRecorderDisplay( RT );
    if( !run_update ) RT.draw( pauseMsg );
}

void Level::flingLineUpdate()// use of pullLine[2]
{
    if( !pMHfling ) return;
    flingMark.setPosition( pMHfling->pos.x, pMHfling->pos.y );

    if( button::pButtMse ) return;
 //   sf::Vector2f msePos( button::mseX, button::mseY );
    sf::Vector2f msePos( button::mseX + viewOfstX, button::mseY + viewOfstY );

    if( button::clickEvent_Lt() == 1 )// button pressed
    {
        pullLine[0].position = pullLine[1].position = msePos;
        pMHfling->v *= 0.0f;
        pMHfling->setPosition( vec2d( msePos.x, msePos.y ) );
    }
    else if( button::clickEvent_Lt() == -1 )// button released
    {
        pullLine[1].position = msePos;
        // assign velocity
        vec2d sep;
        sep.x = pullLine[1].position.x - pullLine[0].position.x;
        sep.y = pullLine[1].position.y - pullLine[0].position.y;
        if( sep.mag() > 1.0f ) pMHfling->v = flingVelGrad*sep;
    }
    else if( button::mseDnLt )// held down
    {
        pullLine[1].position = msePos;
        pMHfling->setPosition( vec2d(pullLine[0].position.x, pullLine[0].position.y) );
        pMHfling->v *= 0.0f;
      //  pMHfling->pos.x = pullLine[0].position.x;
     //   pMHfling->pos.y = pullLine[0].position.y;
    }
}

// non members

bool hitRect( sf::Rect<float> hitRect, sf::Vector2f pt )
{
    if( pt.x < hitRect.left ) return false;
    if( pt.x > hitRect.left + hitRect.width ) return false;
    if( pt.y < hitRect.top ) return false;
    if( pt.y > hitRect.top + hitRect.height ) return false;

    return true;
}
