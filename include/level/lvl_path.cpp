#include "lvl_path.h"

// hitSpot functions
hitSpot::hitSpot( Leg* path, float s, sf::Color dotClr, float Rdot )
{
    Leg* pStartLeg = path;
    while( s > pStartLeg->len ) { s -= pStartLeg->len; pStartLeg = pStartLeg->next; }
    pLeg = pStartLeg;
    s0 = s;
    pt.x = pStartLeg->x(s0);
    pt.y = pStartLeg->y(s0);
    dot.setRadius(Rdot);
    dot.setOrigin( Rdot, Rdot );
    dot.setPosition(pt.x, pt.y);
    dot.setFillColor( dotClr );
}

void hitSpot::place( pathMover* pMover )const { pMover->setState( s0, pMover->v, pLeg ); }

// lvl_path funcs
bool lvl_path::init( )
{
    mvHit::windowTopOpen = false;
    Level::clearColor = sf::Color(50,50,50);
    for( size_t i=0; i<4; ++i ) button::moValBox[i].color = Level::clearColor;

    assignMessages();

    std::ifstream fin( "include/images/tiles1_SSdata.txt" );
    if( !fin ) { std::cout << "no bkgdSS data\n"; return false; }
    bkgd_SS.init(fin);
    fin.close();

    fin.open( "include/images/shipsFlapWedgeUfo_SSdata.txt" );
    if( !fin ) { std::cout << "no shipSS data\n"; return false; }
    ship_SS.init(fin);
    ship_SS.img.createMaskFromColor( sf::Color::Black );
    ship_SS.txt.update( ship_SS.img );
    fin.close();

    fin.open( "include/level/lvl_path_data/quadPack1_data.txt" );
    if( !fin ) { std::cout << "no quadPack data\n"; return false; }
    QdPack1.init( fin, &bkgd_SS );
    fin.close();

    sf::IntRect iR = ship_SS.getFrRect( 0, 0 );
    sf::FloatRect tR; tR.left = iR.left; tR.top = iR.top; tR.width = iR.width; tR.height = iR.height;
    sf::FloatRect rR;
    rR.left = 20.0f; rR.top = 500.0f;
    rR.width = tR.width; rR.height = tR.height;
    assignRHbox_all( Qvtx, 0, rR, tR );

    int retVal = initPathMovers();
    if( retVal != 0 ){ std::cout << "pathMover init fail..\n"; return false; }
    tracePath( *pPath, 5.0f, pathVtxVec, sf::Color(168,168,168), sf::Color::Red );
    tracePath( *pPath2, 5.0f, pathVtxVec2, sf::Color(168,168,168), sf::Color::Red );
    if( pMoverVec.size() > 0 ) Level::pMHfling = pMoverVec[0]->pMH;

    // the recorder
     rec::init( 2000, 1, std::bind( lvl_path::init_rec, this, std::placeholders::_1), &recorder_on );
     ctUtil::init_recorderControl( recorderControl, 1150, 30.0f, -40.0f, 20.0f );
     button::RegisteredButtVec.push_back( &recorderControl );
     button::RegisteredButtVec.push_back( &Level::goto_MMButt );

    return true;
}

bool lvl_path::handleEvent( sf::Event& rEvent )// mouse input launches rB
{
    static size_t frIdx = 0, setNum = 0;
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        sf::Keyboard::Key K = rEvent.key.code;
        if( K == sf::Keyboard::Num0 ) setNum = 0;
        else if( K == sf::Keyboard::Num1 ) { setNum = 1; frIdx = 0; }
        else if( K == sf::Keyboard::Num2 ) { setNum = 2; frIdx = 0; }
        else if( K == sf::Keyboard::Num3 ) { setNum = 3; frIdx = 0; }
        else if( K == sf::Keyboard::Num4 ) { setNum = 4; frIdx = 0; }
        else if( K == sf::Keyboard::Num5 ) { setNum = 5; frIdx = 0; }
        else if( K == sf::Keyboard::Num6 ) { setNum = 6; frIdx = 0; }
        else if ( K == sf::Keyboard::F9 )// cycle frame in SS
        {
            sf::IntRect iR = ship_SS.getFrRect( frIdx, setNum, true );
            sf::FloatRect tR; tR.left = iR.left; tR.top = iR.top; tR.width = iR.width; tR.height = iR.height;
            sf::FloatRect rR;
            rR.left = 20.0f; rR.top = 500.0f;
            rR.width = tR.width; rR.height = tR.height;
            assignRHbox_all( Qvtx, 0, rR, tR );
        }
        else if ( K == sf::Keyboard::F3 )// cycle MHfling
        {
            launchIdx = (1+launchIdx)%pMH_pathVec.size();
            Level::pMHfling = pMH_pathVec[launchIdx];
        }
        else if ( K == sf::Keyboard::F4 )// go off path
        {
            pMoverVec[launchIdx]->pLeg = nullptr;
        }
        else if ( K == sf::Keyboard::F5 )// back on path
        { pMoverVec[launchIdx]->init( pPath, pMoverVec[launchIdx]->pMH, pMoverVec[launchIdx]->v, 100.0f, false ); }
        else if ( K == sf::Keyboard::F6 )// back on path
        { pMoverVec[launchIdx]->init( pPath2, pMoverVec[launchIdx]->pMH, pMoverVec[launchIdx]->v, 0.0f, false ); }
    }

    return true;
}

void lvl_path::update( float dt )// hit test buttons and write to pSelNum
{
    for( auto& x : pMoverVec ) x->update(dt);

    for( size_t i=0; i+1< pMH_pathVec.size(); ++i )// mvHits vs mvHits
    for( size_t j=i+1; j< pMH_pathVec.size(); ++j )
        pMH_pathVec[i]->hit( *pMH_pathVec[j] );

    if( hitSpotButt.sel )
        for( auto& hs : hitSpotVec )
        {
            bool Hit = false;
            for( auto& pMV : pMoverVec )
                if( !pMV->pLeg && (Hit = pMV->pMH->hit(hs.pt)) ) hs.place( pMV );

            if( Hit ) continue;
         }

 /*   if( gravity_on ) for( auto& pMH : pMvHitVec ) pMH->update( gravity );
    else for( auto& pMH : pMvHitVec ) pMH->update();

     if( gravity_on ) for( auto& pMH : pBulletVec ) pMH->update( gravity );
    else for( auto& pMH : pBulletVec ) pMH->update();

    for( size_t i=0; i< pMvHitVec.size(); ++i )// mvHits vs mover_mvHits
    for( auto& x : pMH_pathVec )
        pMvHitVec[i]->hit( *x );

    for( auto& x : pMH_pathVec )//  mover_mvHits vs bullets
    for( auto& pBt : pBulletVec )
        x->hit( *pBt ); */

    return;
}

void lvl_path::draw( sf::RenderTarget& RT ) const
{
    RT.draw( &pathVtxVec[0], pathVtxVec.size(), sf::LinesStrip );
    RT.draw( &pathVtxVec2[0], pathVtxVec2.size(), sf::LinesStrip );
    for( auto& x : hitSpotVec ) RT.draw( x.dot );
    for( auto& msg : msgVec ) RT.draw( msg );
    QdPack1.draw(RT);

    for( auto& x : pMoverVec ) x->draw(RT);
    RT.draw( Qvtx, 4, sf::Quads, &(ship_SS.txt) );

    return;
}

void lvl_path::cleanup()
{
    // mvHits on the paths
    for( auto& x : pMH_pathVec ) delete x;// cleanup
    pMH_pathVec.clear();
    // pathMover adapters for each mvHit
    for( auto& x : pMoverVec ) delete x;// cleanup
    pMoverVec.clear();

    std::cout << "destroying " << destroyPath( pPath ) << " Legs\n";
    pPath = nullptr;

    rec::cleanup();
    return;
}

int lvl_path::initPathMovers(void)
{
    // Load the mvHits
    std::ifstream fin("include/level/lvl_path_data/control_data.txt");
    if( !fin ) return 1;

    char mvMode = 'd';
    fin >> mvMode;
    float Fk_min, Fk_max;
    fin >> Fk_min >> pathMover::Fk >> Fk_max;
    float Fd_min, Fd_max;
    fin >> Fd_min >> pathMover::Fd >> Fd_max;
 //   fin >> pathMover::Fk >> pathMover::Fd;
    fin.close();

    // Load the Legs. One path only for now
    fin.open( "include/level/lvl_path_data/path_data1.txt" );
    if( !fin ) return 3;

    std::string pathFname;
    fin >> pathFname;
    std::ifstream finPath( pathFname.c_str() );
    if( !finPath ) return 4;
    makePath( finPath, pPath );
    finPath.close();

    if( !load_mvHits( fin, pMH_pathVec ) ) return 2;

    size_t numMvHits = pMH_pathVec.size();

    Leg* pStartLeg = pPath;
    pMoverVec.reserve( numMvHits );
    for( size_t i=0; i < numMvHits; ++i )
    {
        float s_st = 0.0f;
        fin >> s_st;
        size_t setnum = 0;
        fin >> setnum;
        pStartLeg = pPath;
        while( s_st > pStartLeg->len ) { s_st -= pStartLeg->len; pStartLeg = pStartLeg->next; }
        aniMover* pAM = new aniMover( pStartLeg, pMH_pathVec[i], 4.0f, s_st );
        pMoverVec.push_back( pAM );
        pAM->mode = mvMode;
        pAM->init2( &ship_SS, setnum );
    }

    size_t numHitSpots = 0;
    fin >> numHitSpots;
    for(size_t i=0; i<numHitSpots; ++i)
    {
        float s0 = 0.0f;
        fin >> s0;
        hitSpotVec.push_back( hitSpot( pPath, s0, sf::Color::White, 6.0f ) );
    }

    fin.close();

    // a 2nd path explicitly
    fin.open( "include/level/lvl_path_data/path_data2.txt" );
    if( !fin ) return 3;

    fin >> pathFname;
    finPath.open( pathFname.c_str() );
    if( !finPath ) return 4;
    makePath( finPath, pPath2 );
    finPath.close();

    std::vector<mvHit*> pMH_tempVec;
    if( !load_mvHits( fin, pMH_tempVec ) ) return 2;

    numMvHits = pMH_tempVec.size();

    pStartLeg = pPath2;
    pMoverVec.reserve( numMvHits );
    for( size_t i=0; i < numMvHits; ++i )
    {
        float s_st = 0.0f;
        fin >> s_st;
        pStartLeg = pPath2;
        while( s_st > pStartLeg->len ) { s_st -= pStartLeg->len; pStartLeg = pStartLeg->next; }
        pathMover* pPM = new pathMover( pStartLeg, pMH_tempVec[i], 4.0f, s_st );
        pMoverVec.push_back( pPM );
        pPM->mode = mvMode;
    }
    //consolidate mvHits
    for( auto& x : pMH_tempVec ) pMH_pathVec.push_back(x);
    // add hitspots
    fin >> numHitSpots;
    for(size_t i=0; i<numHitSpots; ++i)
    {
        float s0 = 0.0f;
        fin >> s0;
        hitSpotVec.push_back( hitSpot( pPath2, s0, sf::Color::White, 6.0f ) );
    }

    fin.close();// done loading 2nd path

    // controls
    vec2d Cpos;
    sf::Text label("Path", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    Cpos.x = 50.0f; Cpos.y = 600.0f;
    pathControl.init( Cpos.x, Cpos.y, 50.0f, 25.0f, label );
    pathControl.persist = 3;
    pathControl.ownButts = true;
 //   pathControl.inDefView = false;
    label.setString("path speed");
    pathControl.pButtVec.push_back( new slideBar( Cpos.x+70, Cpos.y-50, 150, 20, label, -8.0f, 8.0f, 4.0f, std::bind( [this](float v) { for( auto& x : this->pMoverVec ) x->v = v; }, std::placeholders::_1 ) ) );
    label.setString("spring K");
    pathControl.pButtVec.push_back( new buttonValOnHit( Cpos.x+70, Cpos.y, 150, 20, label, Fk_min, Fk_max, pathMover::Fk, &pathMover::Fk ) );
    label.setString("damping");
    pathControl.pButtVec.push_back( new buttonValOnHit( Cpos.x+70, Cpos.y+50, 150, 20, label, Fd_min, Fd_max, pathMover::Fd, &pathMover::Fd ) );
    button::RegisteredButtVec.push_back( &pathControl );

    label.setString("SPOTS");
    hitSpotButt.init( Cpos.x, Cpos.y+50, 50.0f, 25.0f, label );
    button::RegisteredButtVec.push_back( &hitSpotButt );

    return 0;
}

void lvl_path::init_rec( std::vector<state_ab*>& p_stateVec )
{
    state_ab* pSt = nullptr;
    for( auto& pMover : pMoverVec )// movers
    {
        pSt = pMover->pMH->newState();
        if( pSt ) p_stateVec.push_back( pSt );
        pSt = pMover->newState();
        if( pSt ) p_stateVec.push_back( pSt );
    }

    // add control states
    int NbuttStates = 0;
    for( auto& pButt : button::RegisteredButtVec )
     NbuttStates += ctUtil::addButtonState( *pButt, p_stateVec );
     std::cout << "added " << NbuttStates << " button states\n";

    return;
}

void lvl_path::assignMessages()
{
    const sf::Font& rFont = *button::pFont;
    size_t fontSz = 12;
    float x0 = 20.0f;
    float y = 20.0f;
    float dy = 15.0f;

    msgVec.push_back( sf::Text("F3  capture next mvHit", rFont, fontSz) );
    msgVec.push_back( sf::Text("F4  free mvHit from path", rFont, fontSz) );
    msgVec.push_back( sf::Text("F5  place mvHit on fig 8 path", rFont, fontSz) );
    msgVec.push_back( sf::Text("F6  place mvHit on oval path", rFont, fontSz) );

    for( auto& msg : msgVec )
    {
        msg.setColor( sf::Color::White );
        msg.setPosition( x0, y );
        y += dy;
    }
}
