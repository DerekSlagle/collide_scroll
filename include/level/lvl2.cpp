#include "lvl2.h"

bool lvl2::init( )
{
    mvHit::wdwCf = 0.0f;
    mvHit::windowTopOpen = false;
    mapW = winW;
    mapH = winH;
    viewOfstX = viewOfstY = 0.0f;// as fraction of winW
    mapView.reset( sf::FloatRect( 0.0f, 0.0f, winW, winH ) );
    recorder_on = false;

    std::ifstream fin( "include/level/lvl2_data/mvHit_data.txt" );
    if( !fin ) { std::cout << "no mvHit_data\n"; return false; }
    if( !load_mvHits( fin, pMvHitVec ) ) { std::cout << "no load_mvHits\n"; return false; }
    fin >> numReps;  std::cout << "\n numReps = " << numReps;
    flingVelGrad /= (float)numReps;
    lineSegSwing::grav /= (float)(numReps*numReps);
    fin.close();

    sf::Text label("other", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );

    // misc control
    vec2d miscPos( 780.0f, 30.0f );
    miscControl.init( miscPos.x, miscPos.y, 50.0f, 25.0f, label );
    miscControl.persist = 3;
    miscControl.ownButts = false;
    button::RegisteredButtVec.push_back( &miscControl );
    miscPos.x = 850.0f;

    // find the special ones
    if( pMvHitVec.size() > 0 )
    {
        launchIdx = 0;
        label.setString("fling mvHit select");
        label.setColor( sf::Color(255,0,0) );
        selectMH.init( miscPos.x, miscPos.y, 30.0f*pMvHitVec.size(), 20.0f, pMvHitVec.size(), label, launchIdx );
        selectMH.pFuncIdx = std::bind( [this](size_t i) { if( i < pMvHitVec.size() ) { pMHfling = pMvHitVec[i]; this->launchIdx = i; std::cout << "\nmh = " << i; } }, std::placeholders::_1 );
        selectMH.pFuncIdx(launchIdx);

        miscControl.pButtVec.push_back( &selectMH );
        for( auto& x : pMvHitVec )
        {
            pXpgA = dynamic_cast<expandPolygon*>(x);
            if( pXpgA )
            {
                label.setString("expand poly speed");
                pXpgA->growSpeed /= (float)numReps;
                xpgVelStrip.init( miscPos.x, miscPos.y + 40.0f, 150, 20, label, 1.0f/(float)numReps, 5.0f/(float)numReps, pXpgA->growSpeed, &(pXpgA->growSpeed) );
                miscControl.pButtVec.push_back( &xpgVelStrip );
                break;
            }
        }// 1st will do
    }

    fin.open( "include/level/lvl2_data/seg_data.txt" );
    if( !fin ) { std::cout << "no segHit_data\n"; return false; }
   if( !loadSegs( fin, pSegVec ) ) { std::cout << "no load_segHits\n"; return false; }
   std::cout << "\nloaded segHits: " << pSegVec.size();
    fin.close();

    label.setString("rotVel");
    lineSegRotate* pLsr = nullptr;
    for( auto& x : pSegVec )
    {
        pLsr = dynamic_cast<lineSegRotate*>(x);
        if( pLsr )
        {
            pLsr->rotVel = 0.02f/(float)numReps;
            sldBar1.init( miscPos.x, miscPos.y + 80.0f, 150, 20, label, -0.1f/(float)numReps, 0.1f/(float)numReps, pLsr->rotVel, &(pLsr->rotVel) );
            miscControl.pButtVec.push_back( &sldBar1 );
            break;
        }
    }// 1st will do

    lineSegElevator* pLsElev = nullptr;
    for( auto& x : pSegVec ) { pLsElev = dynamic_cast<lineSegElevator*>(x); if( pLsElev ) break; }// 1st will do
    if( pLsElev )
    {
        ctUtil::init_elevatorControl( elevControl, *pLsElev, 690.0f, 30.0f, numReps );
        button::RegisteredButtVec.push_back( &elevControl );
    }

    for( auto& x : pSegVec )
    {
        lineSegFlip* pLsFlip = dynamic_cast<lineSegFlip*>(x);
        if( pLsFlip )
        {
            ctUtil::init_flipperControl( flipControl, *pLsFlip, numReps );
            button::RegisteredButtVec.push_back( &flipControl );
            break;
        }
    }

    for( auto& x : pSegVec )
    {
        lineBumper* pLsBump = dynamic_cast<lineBumper*>(x);
        if( pLsBump )
        {
            bumpSpot.init( Equate( pLsBump->pos ), sf::Vector2f( 15.0f, -8.0f ), 6.0f, "bumper", 14 );
            pLsBump->set_bumpSpeed( pLsBump->vel.mag()/(float)numReps );
            pLsBump->bumpSteps *= numReps;
            bumpSpot.init2( 1.0f/(float)numReps, 7.0f/(float)numReps, pLsBump->vel.mag(), "speed", std::bind(lineBumper::set_bumpSpeed, pLsBump, std::placeholders::_1) );
            button::RegisteredButtVec.push_back( &bumpSpot );
            break;
        }
    }

    rec::init( 1000, 1, std::bind( lvl2::init_rec, this, std::placeholders::_1), &recorder_on );
    ctUtil::init_recorderControl( recorderControl, 1200, 30.0f, -40.0f, 20.0f );
    button::RegisteredButtVec.push_back( &recorderControl );

    button::RegisteredButtVec.push_back( &Level::goto_MMButt );

 //   label.setString("time delta");
 //   timeBar.init( 200.0f, 650.0f, 150, 20, label, -1.0f, 2.0f, 1.0f, nullptr );
 //   button::RegisteredButtVec.push_back( &timeBar );

    assignMessages();

    return true;
}

bool lvl2::handleEvent( sf::Event& rEvent )// mouse input launches rB
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        sf::Keyboard::Key K = rEvent.key.code;
        if ( K == sf::Keyboard::F3 )// fling next mvHit in container
        {
            if( pMvHitVec.size() > 0 )
            {
                launchIdx = (1 + launchIdx )%pMvHitVec.size();
                selectMH.setIndex(launchIdx);
             //   pMHfling = pMvHitVec[launchIdx];
            }

        }
        else if ( K == sf::Keyboard::R )// rotate current poly
        {
            if( pMHfling )
            {
                regPolygon* pPoly = dynamic_cast<regPolygon*>( pMHfling );
                if( pPoly ) pPoly->setRotation( pPoly->getRotation() + 0.05f );
            }
        }
        else if ( K == sf::Keyboard::F4 )// change fling mvHit to the one under the mouse right now
        {
            vec2d sep, N;
            float dSep;
            for( size_t i=0; i<pMvHitVec.size(); ++i )
        //    if( pMvHitVec[i]->is_inMe( vec2d( button::mseX, button::mseY ), sep, N, dSep ) )
            if( pMvHitVec[i]->is_inMe( vec2d( button::mseX + viewOfstX, button::mseY + viewOfstY ), sep, N, dSep ) )
            {
                launchIdx = i;
                selectMH.setIndex(launchIdx);
            //    pMHfling = pMvHitVec[launchIdx];
                break;
            }
        }
    //    else if ( K == sf::Keyboard::F5 ) pMvHitVec[launchIdx]->is_free = !pMvHitVec[launchIdx]->is_free;// toggle value
        else if ( K == sf::Keyboard::F5 ){ if( pMHfling ) pMHfling->is_free = !pMHfling->is_free; }// toggle value
        else if ( K == sf::Keyboard::F6 ){ for( auto& pMh : pMvHitVec ) pMh->v *= 0.0f; }// freeze all
    //    else if ( K == sf::Keyboard::F7 ){ elevControl.closeList(); }// freeze all
        else if ( K == sf::Keyboard::Z ) { if( pXpgA ) pXpgA->Transform(); }
        else if ( K == sf::Keyboard::H ) { showMsgs = !showMsgs; }
        else if ( K == sf::Keyboard::R )// rotate current poly
        {
            if( pMHfling )
            {
                regPolygon* pPoly = dynamic_cast<regPolygon*>( pMHfling );
                if( pPoly ) pPoly->setRotation( pPoly->getRotation() + 0.05f );
            }
        }
    }
    else if (rEvent.type == sf::Event::MouseWheelScrolled)
    {
        float dR = 2.0f*(float) rEvent.mouseWheelScroll.delta;
        if( pXpgA ) pXpgA->set_rFull( pXpgA->rFull + dR );
    }

    return true;
}

void lvl2::update( float dt )// hit test buttons and write to pSelNum
{
  //  dt = timeBar.fVal;
  //  mvHit::timeIsForward = dt >= 0.0f;
  //  if( recorder_on ) run_update = rec::update();
 //   if( !(run_update || step_update) ) return;

//    if( gravity_on ) for( auto& pMH : pMvHitVec ) pMH->update( gravity );
    for( size_t i=0; i < numReps; ++i )
    {
        for( auto& pMH : pMvHitVec ) pMH->update(dt);
        for( auto& pSH : pSegVec ) pSH->update(dt);// rotate etc

        for( size_t i=0; i+1< pMvHitVec.size(); ++i )// mvHits vs mvHits
        for( size_t j=i+1; j< pMvHitVec.size(); ++j )
            pMvHitVec[i]->hit( *pMvHitVec[j] );

        for( auto& pMH : pMvHitVec )// mvHits vs segHits
            for( auto& pSH : pSegVec )
                pSH->hit( *pMH );
    }

    return;
}

void lvl2::draw( sf::RenderTarget& RT ) const
{
    if( showMsgs )for( const auto& msg : msgVec ) RT.draw(msg);
    else RT.draw( helpMsg );

    for( auto& pMH : pMvHitVec ) pMH->draw(RT);
    for( auto& pSH : pSegVec ) pSH->draw(RT);
    return;
}

void lvl2::cleanup()
{
    flingVelGrad *= (float)numReps;
    lineSegSwing::grav *= (float)(numReps*numReps);
    rec::cleanup();
    return;
}

void lvl2::init_rec( std::vector<state_ab*>& p_stateVec )
{
//    p_stateVec.reserve( pMvHitVec.size() );

    state_ab* pSt = nullptr;
    for( auto& mh : pMvHitVec )// regular mvHits
    {
        pSt = mh->newState();
        if( pSt ) p_stateVec.push_back( pSt );
    }

    for( auto& sh : pSegVec )// segHits
    {
        pSt = sh->newState();
        if( pSt ) p_stateVec.push_back( pSt );
    }

 //   p_stateVec.push_back( new state_f( &(sldBar1.fVal), std::bind( slideBar::set_fVal, &sldBar1, std::placeholders::_1) ) );
     // add control states
    int NbuttStates = 0;
    for( auto& pButt : button::RegisteredButtVec )
     NbuttStates += ctUtil::addButtonState( *pButt, p_stateVec );
     std::cout << "lvl2 added " << NbuttStates << " button states\n";

    return;
}

void lvl2::assignMessages()
{
    const sf::Font& rFont = *button::pFont;
    size_t fontSz = 12;
    float x0 = 20.0f;
    float y = 10.0f;
    float dy = 15.0f;

    helpMsg.setFont(rFont);
    helpMsg.setCharacterSize(16);
    helpMsg.setString("Key: H = help");
    helpMsg.setPosition( x0, y );
    helpMsg.setColor( sf::Color::Magenta );

    msgVec.push_back( sf::Text("F3 cycles fling capture", rFont, fontSz) );
    msgVec.push_back( sf::Text("F4 capture mvHit under mouse", rFont, fontSz) );
    msgVec.push_back( sf::Text("F5 toggle is_free", rFont, fontSz) );
    msgVec.push_back( sf::Text("F6 freeze all mvHits", rFont, fontSz) );
    msgVec.push_back( sf::Text("Key: R rotate mvHit", rFont, fontSz) );
    msgVec.push_back( sf::Text("Key: Z expand/contract poly", rFont, fontSz) );
    msgVec.push_back( sf::Text("Key: M goto main menu", rFont, fontSz) );
    msgVec.push_back( sf::Text("Key: Esc = kill app now", rFont, fontSz) );
    msgVec.push_back( sf::Text("Key: H hide these messages", rFont, fontSz) );

    for( auto& msg : msgVec )
    {
        msg.setColor( sf::Color::White );
        msg.setPosition( x0, y );
        y += dy;
    }
}
