#include "lvl_platformer.h"
//#include <Windows.h>

bool lvl_platformer::init()
{
    mvHit::wdwCf = 0.3f;
    mvHit::windowTopOpen = true;

 /*   assignMessages();

    int retVal = initPathMovers();
    if( retVal != 0 ){ std::cout << "pathMover init fail..\n"; return false; }
    // after above, which loads ship_SS
    retVal = initShots();
    if( retVal != 0 ){ std::cout << "shot init fail..\n"; return false; }
    float rMin = shotStepSz*pShotVec[0]->r;
    numLoops = (size_t)( 1.0f + shotVel/rMin );
    std::cout << "\n init: shotVel: " << shotVel << " numLoops: " << numLoops << " rMin: " << rMin; */

    mapView.reset(sf::FloatRect( 0.0f, 0.0f, mvHit::wdwW, mvHit::wdwH ));
    old_wdwW = mvHit::wdwW;
    old_wdwH = mvHit::wdwH;
    iPos_viewCtr.x = old_wdwW/2.0f;
    iPos_viewCtr.y = old_wdwH/2.0f;

//    PGCbkRect.setFillColor( sf::Color(50,50,50) );
    for( size_t i=0; i<4; ++i ) button::moValBox[i].color = sf::Color(50,50,50);

    // platGuys ball
    std::ifstream fin("include/level/platformer_data/ball_data_1.txt");
    if( !fin ) return false;
    B1.init( fin );
    B1.maxVel = 0.0f;

    // the multiControl
    sf::Text label("MULTI", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    float PGCx = 400.0f, PGCy = 30.0f, PGCdy = 37.0f;
    multiControl.init( PGCx, PGCy - 15.0f, 50.0f, 25.0f, label );
    multiControl.persist = 3;
    multiControl.ownButts = true;
    button::RegisteredButtVec.push_back( &multiControl );

    // platGuy params
    buttonList* pList = new buttonList;
    label.setString("platGuy");
    pList->init( PGCx, multiControl.pos.y + 1.1f*25.0f, 50.0f, 25.0f, label );
    pList->persist = 3;
    pList->ownButts = true;
    multiControl.pButtVec.push_back( pList );

    float minVal, maxVal;
    label.setString("friction coeff");
    fin >> minVal >> maxVal >> mvHit::wdwCf;
    pList->pButtVec.push_back( new buttonValOnHit( PGCx + 60.0f, PGCy + 0.0f*PGCdy, 150, 20, label, minVal, maxVal,  mvHit::wdwCf, &mvHit::wdwCf ) );
    label.setString("gravity");
    fin >> minVal >> maxVal >> gravity.y;
    pList->pButtVec.push_back( new buttonValOnHit( PGCx + 60.0f, PGCy + 1.0f*PGCdy, 150, 20, label, minVal, maxVal,  gravity.y, &(gravity.y) ) );
    // plat guy
    label.setString("running speed");
    fin >> minVal >> maxVal >> maxSpeed;
    pList->pButtVec.push_back( new buttonValOnHit( PGCx + 60.0f, PGCy + 2.0f*PGCdy, 150, 20, label, minVal, maxVal,  maxSpeed, &maxSpeed ) );
    label.setString("jump speed");
    fin >> minVal >> maxVal >> jumpSpeed;
    pList->pButtVec.push_back( new buttonValOnHit( PGCx + 60.0f, PGCy + 3.0f*PGCdy, 150, 20, label, minVal, maxVal,  jumpSpeed, &jumpSpeed ) );
    label.setString("bounce amp");
    fin >> minVal >> maxVal >> bounceAmp;
    pList->pButtVec.push_back( new buttonValOnHit( PGCx + 60.0f, PGCy + 4.0f*PGCdy, 150, 20, label, minVal, maxVal,  bounceAmp, &bounceAmp ) );
    label.setString("climb speed");
    fin >> minVal >> maxVal >> climbSpeed;
    pList->pButtVec.push_back( new buttonValOnHit( PGCx + 60.0f, PGCy + 5.0f*PGCdy, 150, 20, label, minVal, maxVal,  climbSpeed, &climbSpeed ) );

    listBK_rectVec.push_back( sf::RectangleShape() );
    listBK_rectVec.back().setPosition( PGCx + 55.0f, PGCy - 15.0f );
    listBK_rectVec.back().setSize( sf::Vector2f( 215.0f, 6.0f*PGCdy + 15.0f ) );
    listBK_rectVec.back().setFillColor( sf::Color(50,50,50) );
    pListVec.push_back( pList );

    button::RegisteredButtVec.push_back( &Level::goto_MMButt );

    /*
    sf::Text label("platGuy", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    float PGCx = 500.0f, PGCy = 30.0f, PGCdy = 37.0f;
    platGuyControl.init( PGCx, PGCy - 15.0f, 50.0f, 25.0f, label );
    platGuyControl.persist = 3;
    platGuyControl.ownButts = true;
    button::RegisteredButtVec.push_back( &platGuyControl );
    // enviro
    float minVal, maxVal;
    label.setString("friction coeff");
    fin >> minVal >> maxVal >> mvHit::wdwCf;
    platGuyControl.pButtVec.push_back( new buttonValOnHit( PGCx + 60.0f, PGCy + 0.0f*PGCdy, 150, 20, label, minVal, maxVal,  mvHit::wdwCf, &mvHit::wdwCf ) );
    label.setString("gravity");
    fin >> minVal >> maxVal >> gravity.y;
    platGuyControl.pButtVec.push_back( new buttonValOnHit( PGCx + 60.0f, PGCy + 1.0f*PGCdy, 150, 20, label, minVal, maxVal,  gravity.y, &(gravity.y) ) );
    // plat guy
    label.setString("running speed");
    fin >> minVal >> maxVal >> maxSpeed;
    platGuyControl.pButtVec.push_back( new buttonValOnHit( PGCx + 60.0f, PGCy + 2.0f*PGCdy, 150, 20, label, minVal, maxVal,  maxSpeed, &maxSpeed ) );
    label.setString("jump speed");
    fin >> minVal >> maxVal >> jumpSpeed;
    platGuyControl.pButtVec.push_back( new buttonValOnHit( PGCx + 60.0f, PGCy + 3.0f*PGCdy, 150, 20, label, minVal, maxVal,  jumpSpeed, &jumpSpeed ) );
    label.setString("bounce amp");
    fin >> minVal >> maxVal >> bounceAmp;
    platGuyControl.pButtVec.push_back( new buttonValOnHit( PGCx + 60.0f, PGCy + 4.0f*PGCdy, 150, 20, label, minVal, maxVal,  bounceAmp, &bounceAmp ) );
    label.setString("climb speed");
    fin >> minVal >> maxVal >> climbSpeed;
    platGuyControl.pButtVec.push_back( new buttonValOnHit( PGCx + 60.0f, PGCy + 5.0f*PGCdy, 150, 20, label, minVal, maxVal,  climbSpeed, &climbSpeed ) );

    PGCbkRect.setPosition( PGCx + 55.0f, PGCy - 15.0f );
    PGCbkRect.setSize( sf::Vector2f( 215.0f, 6.0f*PGCdy + 15.0f ) );
    */
//    numReps = 1;
    fin.close();

     assignMessages();

     // new location
    int retVal = initPathMovers();
    if( retVal != 0 ){ std::cout << "pathMover init fail..\n"; return false; }
    // after above, which loads ship_SS
    retVal = initShots();
    if( retVal != 0 ){ std::cout << "shot init fail..\n"; return false; }
    float rMin = shotStepSz*pShotVec[0]->r;
    numLoops = (size_t)( 1.0f + shotVel/rMin );
    std::cout << "\n init: shotVel: " << shotVel << " numLoops: " << numLoops << " rMin: " << rMin;

    gravity_on = true;
    gravity.x = 0.0f;// to here

    loadPlatforms();

    worldWidth = qpVec.back().pos.x + qpVec.back().L.x;
    if( worldWidth > mvHit::wdwW ) { old_wdwW = mvHit::wdwW; mvHit::wdwW = worldWidth; Level::usingMapView = true; }
    else Level::usingMapView = false;

    fin.open( "include/level/platformer_data/M484SpaceSoldier_SSdata.txt" );
    if( !fin ) return false;

    spaceManSS.init(fin);
    spaceManSS.img.createMaskFromColor( sf::Color::Black );
    spaceManSS.txt.update( spaceManSS.img );
    // other platformGuy data
    fin >> setRunLt >> setRunRt >> setStill;
    fin >> frFaceLt >> frFaceRt >> frJumpLt >> frJumpRt;
    fin >> frHitLt >> frHitRt >> frCrouchLt >> frCrouchRt;
    fin.close();

    float scl = 1.5f;
    vec2d iPos = ( 2.0f*qpVec[2].pos + qpVec[2].L )/2.0f;// midpoint of qpVec[1]
    platGuy.init( nullptr, &B1, 0.0f, 0.0f );
    platGuy.init2( &spaceManSS, setStill, scl );
    platGuy.setPosition( iPos.x, iPos.y - 200.0f );
    platGuy.frIdx = frFaceRt;
    platGuy.spr.setTextureRect( platGuy.getFrRect() );
    platGuy.loopSet = false;

    B1.r = scl*(float)platGuy.getFrRect().height/2.0f;
    B1.img.setRadius( B1.r );
    B1.img.setOrigin( B1.r, B1.r );

    // items
    if(  qpVec.size() > 8 )
    {
        vec2d ptA = qpVec[4].pos;
        vec2d ptB = qpVec[4].pos + qpVec[4].L;
        size_t nItems = 5;
        vec2d dPt = (ptB-ptA)/(float)(nItems-1);
        pickVec.reserve( 2*nItems );
        ptA.y -= 40.0f;
        for( size_t i=0; i<nItems; ++i ) pickVec.push_back( pickItem( 5, 15.0f, 4.0f, ptA + dPt*(float)i ) );

        ptA = qpVec[6].pos + qpVec[6].L/2.0f;
        ptB = qpVec[6].pos + qpVec[6].L;
        dPt = (ptB-ptA)/(float)(nItems-1);
        ptA.y -= 40.0f;
        for( size_t i=0; i<nItems; ++i ) pickVec.push_back( pickItem( 3, 12.0f, 4.0f, ptA + dPt*(float)i, sf::Color::Blue ) );

    }

    if(  qpVec.size() > 5 )
    {
        // place some rigid mvHits
         fin.open( "include/level/platformer_data/fixed_mvHit_data.txt" );
        if( !fin ) { std::cout << "\n no mvHit_data"; return false; }
        if( !load_mvHits( fin, pMvHitVec ) ) return false;
        for( auto& pMH : pMvHitVec )
        {
            pMH->setPosition( pMH->pos );
            fin >> pMH->is_free;// make them rigid
        }
    fin.close();
    }

    // a flame SS
/*    fin.open( "include/images/flame2_SSdata.txt" );
    if( !fin ) { std::cout << "no flame SS data\n"; return false; }
    fire_SS.init(fin);
    fin.close();

    scl = 1.0f;
    fireAM.init( nullptr, &fireBall, 0.0f, 0.0f );
    fireAM.init2( &fire_SS, 0, scl );
    fireBall.init( vec2d(500.0f,100.0f), vec2d(0.0f,0.0f), scl*(float)fireAM.getFrRect().height/4.0f, 50.0f, 0.4f, sf::Color::Red );
    fireBall.is_free = false;
    fireAM.setPosition( fireBall.pos.x, fireBall.pos.y );   */

    return true;
}

bool lvl_platformer::loadPlatforms()
{
    std::ifstream fin( "include/level/platformer_data/platform_SSdata.txt" );
    if( !fin ) return false;
    platformSS.init(fin);
    fin.close();

    fin.open( "include/level/platformer_data/quadPack_data.txt" );
    if( !fin ) return false;
    size_t numqp; fin >> numqp;
    // x code
    qpVec.reserve( numqp );
    pSegVec.reserve( numqp + 3 );
    float ofstx = 12.0f, ofsty = 7.0f;
    vec2d offLt( ofstx, ofsty ), offRt( -ofstx, ofsty );
    float Rad = 1.0f;// for arcSeg

    for( size_t i=0; i<numqp; ++i )
    {
        qpVec.push_back( quadPack( fin, &platformSS ) );
        char legType = 'L';
        fin >> legType;
        switch( legType )
        {
            case 'L':
            fin >> offLt.x >> offLt.y >> offRt.x >> offRt.y;
            pSegVec.push_back( new lineSeg( qpVec.back().pos + offLt, qpVec.back().pos + qpVec.back().L + offRt ) );
            pSegVec.back()->Cf = mvHit::wdwCf;
            break;

            case 'A':
            fin >> offLt.x >> offLt.y >> offRt.x >> offRt.y >> Rad;
            if( Rad > 0.0f ) pSegVec.push_back( new arcSeg( qpVec.back().pos + offLt, qpVec.back().pos + qpVec.back().L + offRt , Rad ) );
            else pSegVec.push_back( new arcSeg( qpVec.back().pos + qpVec.back().L + offLt, qpVec.back().pos + offRt , -Rad ) );
            pSegVec.back()->Cf = 0.8f;
            break;
        }

    }

    for( auto& x : pSegVec ) x->testEnd1 = x->testEnd2 = x->friction_on = x->is_bulletProof = true;

    fin.close();
     // ladder Leg
 //   fin.open( "include/level/platformer_data/legData.txt" );
 //   if( !fin ) return false;
 //   float a,b,c,d;
 //   fin >> a >> b >> c >> d;
  //  ladderLeg.init(a,b,c,d);
 //   fin.close();

    // ladders
    fin.open( "include/level/platformer_data/ladderData.txt" );
    if( !fin ) return false;
    size_t numLadders = 0;
    fin >> numLadders;
    for( size_t i = 0; i < numLadders; ++i )
    {
        float a,b,c,d;
        fin >> a >> b >> c >> d;
        ladderVec.push_back( linLeg(a,b,c,d) );
    }

    fin.close();

    return true;
}

bool lvl_platformer::handleEvent( sf::Event& rEvent )
{
    static vec2d msePosUp, msePosDn;
    static bool is_Jdown = false;
    static bool isSpaceDn = false;

    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::Left )// run left
        {
            if( platGuy.setNum != setRunLt )
            {
                B1.maxVel = maxSpeed;
                platGuy.setFrames( setRunLt, false, 0 );
            }
        }
        else if ( rEvent.key.code == sf::Keyboard::Right )// run right
        {
            if( platGuy.setNum != setRunRt )
            {
                B1.maxVel = -maxSpeed;
                platGuy.setFrames( setRunRt, false, 0 );
            }
        }
        else if ( rEvent.key.code == sf::Keyboard::Up )// climb up
        {
            if( platGuy.pLeg ) climbState = -1;
        }
        else if ( rEvent.key.code == sf::Keyboard::Down )// climb down
        {
            if( platGuy.pLeg ) climbState = 1;
        }
        else if ( rEvent.key.code == sf::Keyboard::J )// jump
        {
            if( !is_Jdown )
            {
                jump = true;
                is_Jdown = true;
            }
       //     jump = true;
            if( platGuy.setNum == setStill )
            {
                if( platGuy.frIdx == frFaceLt || platGuy.frIdx == frHitLt || platGuy.frIdx == frCrouchLt )
                    platGuy.setFrames( setStill, false, frJumpLt );
                else platGuy.setFrames( setStill, false, frJumpRt );
            }
            else if( B1.maxVel > 0.0f ) platGuy.setFrames( setStill, false, frJumpLt );
            else platGuy.setFrames( setStill, false, frJumpRt );
        }
        else if ( rEvent.key.code == sf::Keyboard::F4 )
        {
            float s_st0 = 0.0f;
            for( auto& pTgt : pTgt_pathVec )
            {
          //      pTgt->reset(20.0f);
                s_st0+=50.0f;
           //     Leg* pStartLeg = pPath;
           //     while( s_st > pStartLeg->len ) { s_st -= pStartLeg->len; pStartLeg = pStartLeg->next; }
                pTgt->reset( pPath, s_st0 );
           //     pTgt->pLeg = pStartLeg;
           //     pTgt->s = s_st;
           //     pTgt->pMH->setPosition( { pStartLeg->x(s_st), pStartLeg->y(s_st) } );
           //     pTgt->oldPos = vec2d( pStartLeg->x(s_st), pStartLeg->y(s_st) );
            }
        }
        else if ( rEvent.key.code == sf::Keyboard::F5 )// get on or off of ladder
        {
            climb_onOff();
        }
        else if ( rEvent.key.code == sf::Keyboard::L ) platGuy.loopSet = !platGuy.loopSet;// toggle
        else if ( rEvent.key.code == sf::Keyboard::S ) { platGuy.incrSet(); to_sf_string(msgNum1, platGuy.setNum); to_sf_string(msgNum2, platGuy.frIdx); }
        else if ( rEvent.key.code == sf::Keyboard::F ) { platGuy.incrFrame(); to_sf_string(msgNum2, platGuy.frIdx); }
        else if ( rEvent.key.code == sf::Keyboard::H ) { showMsgs = !showMsgs; }
        else if ( rEvent.key.code == sf::Keyboard::Space )
        {
            if( !isSpaceDn )
            {
                vec2d firePos = platGuy.pMH->pos;
                firePos.x += platGuy.pMH->r;
                vec2d fireDir(1.0f,0.0f);
                if( platGuy.setNum == setRunLt ) { fireDir.x = -1.0f; firePos.x -= 2.0f* platGuy.pMH->r; }
                else if( platGuy.setNum == setStill && ( platGuy.frIdx == frFaceLt || platGuy.frIdx == frJumpLt  ) ) { fireDir.x = -1.0f; firePos.x -= 2.0f* platGuy.pMH->r; }

                if( Level::run_update ) firePos -= fireDir*shotVel;// so it renders here. there is an intervening update call
                fireShot( firePos, fireDir, shotVel );
                isSpaceDn = true;
            }
        }
     //   else if ( rEvent.key.code == sf::Keyboard::F5 ) snapShot = true;
    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        if ( rEvent.key.code == sf::Keyboard::Left )// stop running left
        {
            platGuy.setFrames( setStill, false, frFaceLt );
            B1.maxVel = 0.0f;
        }
        else if ( rEvent.key.code == sf::Keyboard::Right )// stop running right
        {
            platGuy.setFrames( setStill, false, frFaceRt );
            B1.maxVel = 0.0f;
        }
        else if ( rEvent.key.code == sf::Keyboard::Up )// stop climbing up
        {
            climbState = 0;
        }
        else if ( rEvent.key.code == sf::Keyboard::Down )// stop climbing down
        {
            climbState = 0;
        }
        else if ( rEvent.key.code == sf::Keyboard::J )// lands facing jump direction
        {
            jump = false;
            is_Jdown = false;
            if( platGuy.frIdx == frJumpLt ) platGuy.setFrames( setStill, false, frFaceLt );
            else platGuy.setFrames( setStill, false, frFaceRt );
        }
        else if ( rEvent.key.code == sf::Keyboard::Space ) isSpaceDn = false;// Spacebar released
    }
     else if (rEvent.type == sf::Event::MouseButtonPressed)// lbutt down
    {
        if (rEvent.mouseButton.button == sf::Mouse::Left)
        {
            if( !button::pButtMse )
            {
                platGuy.setPosition( button::mseX + viewOfstX, button::mseY );
                B1.v *= 0.0f;// be still
            }
        }
    }

    return true;
}

void lvl_platformer::update( float dt )
{
    for( auto& pTgt : pTgt_pathVec ) pTgt->update(dt);
    for( auto& pMH : pMvHitVec ) pMH->update(dt,gravity);
    PG_update(dt);

 //   for( auto& pShot : pShotVec ) pShot->update(dt);

    shots_hit_allMvHits(dt);// shots are moved. update(dt) called

    for( auto& pSeg : pSegVec )
        for( auto& pShot : pShotVec )
            if( pShot->inUse ) pSeg->hit( *pShot );


    for( auto& pSeg : pSegVec )
        for( auto& pMH : pMvHitVec )
            if( pMH->is_free ) pSeg->hit( *pMH );
 //   fireAM.update();
 //   if( platGuy.pMH && fireAM.pMH ) platGuy.pMH->hit( *fireAM.pMH );

    // view position
    if( Level::usingMapView && !button::mseDnLt )
    {
        const float shiftMargin = 300.0f;
     //   std::cerr << "bigView! ";

        if( B1.pos.x > mapView.getCenter().x + shiftMargin  )// move right
        {
            if( B1.pos.x - shiftMargin > mvHit::wdwW - iPos_viewCtr.x )
                mapView.setCenter( mvHit::wdwW - iPos_viewCtr.x, iPos_viewCtr.y );
            else
                mapView.setCenter( B1.pos.x - shiftMargin, iPos_viewCtr.y );
        }
        else if( B1.pos.x < mapView.getCenter().x - shiftMargin  )// move left
        {
            if( B1.pos.x + shiftMargin < iPos_viewCtr.x )
                mapView.setCenter( iPos_viewCtr.x, iPos_viewCtr.y );
            else
                mapView.setCenter( B1.pos.x + shiftMargin, iPos_viewCtr.y );
        }

        viewOfstX = mapView.getCenter().x - iPos_viewCtr.x;
    }

    return;
}

void lvl_platformer::draw_map( sf::RenderTarget& RT ) const
{
    for( const quadPack& qp : qpVec ) qp.draw(RT);
//    B1.draw(RT);
    platGuy.draw(RT);
 //   fireAM.draw(RT);
    for( const auto& x : pSegVec ) x->draw(RT);
    for( const auto& x : pickVec ) x.draw(RT);
    for( const auto& pMH : pMvHitVec ) pMH->draw(RT);
    for( const auto& pTgt : pTgt_pathVec ) pTgt->draw(RT);
    for( const auto& pShot : pShotVec ) pShot->draw(RT);
}

void lvl_platformer::draw( sf::RenderTarget& RT ) const
{
    RT.draw(msg1);
    RT.draw(msgNum1);
    RT.draw(msg2);
    RT.draw(msgNum2);
    RT.draw(bankMsg);
    RT.draw(bankMsgNum);
    if( showMsgs )for( const auto& msg : msgVec ) RT.draw(msg);
    else RT.draw( helpMsg );

 //   if( platGuyControl.sel ) RT.draw( PGCbkRect );
 //   if( shotControl.sel ) RT.draw( shotCbkRect );

    if( multiControl.sel )
    {
        for( size_t i = 0; i < pListVec.size(); ++i )
            if( pListVec[i] && pListVec[i]->sel && i < listBK_rectVec.size() ) RT.draw( listBK_rectVec[i] );
    }

    return;
}

void lvl_platformer::cleanup()
{
     // mvHits on the paths
     std::cout << "deleting " << pTgt_pathVec.size() << " targets\n";
    for( auto& x : pTgt_pathVec ) { delete x->pMH; delete x; }
    pTgt_pathVec.clear();

    std::cout << "destroying " << destroyPath( pPath ) << " Legs\n";
    pPath = nullptr;

    std::cout << "deleting " << pShotVec.size() << " shots\n";
    for( auto& x : pShotVec ) delete x;
    pShotVec.clear();

    return;
}

lvl_platformer::~lvl_platformer()
{
    mvHit::wdwW = old_wdwW;
    mvHit::wdwH = old_wdwH;
    mvHit::wdwCf = 0.0f;
//    for( auto& x : pCBvec ) delete x;
    std::cout << "Bye-bye from platformer dtor\n";
}

// ladder use
void lvl_platformer::climb_onOff()
{
    if( platGuy.pLeg )// climbing off
    {
        platGuy.pLeg = nullptr;
        climbState = 0;
    }
    else// climbing on
    {
        for( const auto& ldr : ladderVec )
        {
            vec2d Pt1( ldr.x0, ldr.y0 );// try this end
            vec2d Pt2( ldr.xf, ldr.yf );// try this end
            if( B1.is_inMe(Pt1) )
            {
                platGuy.pLeg = &ldr;
                platGuy.s = 0.0f;
                climbState = 0;
                platGuy.setPosition( Pt1.x, Pt1.y );
            }
            else if( B1.is_inMe(Pt2) )
            {
                platGuy.pLeg = &ldr;
                platGuy.s = platGuy.pLeg->len;
                climbState = 0;
                platGuy.setPosition( Pt2.x, Pt2.y );
            }

            if( platGuy.pLeg ) break;// found it!
        }
    }
}

/*
void lvl_platformer::climb_onOff()
{
    if( platGuy.pLeg )// climbing off
    {
        platGuy.pLeg = nullptr;
        climbState = 0;
    }
    else// climbing on
    {
        vec2d Pt1( ladderLeg.x0, ladderLeg.y0 );// try this end
        vec2d Pt2( ladderLeg.xf, ladderLeg.yf );// try this end
        if( B1.is_inMe(Pt1) )
        {
            platGuy.pLeg = &ladderLeg;
            platGuy.s = 0.0f;
            climbState = 0;
            platGuy.setPosition( Pt1.x, Pt1.y );
        }
        else if( B1.is_inMe(Pt2) )
        {
            platGuy.pLeg = &ladderLeg;
            platGuy.s = platGuy.pLeg->len;
            climbState = 0;
            platGuy.setPosition( Pt2.x, Pt2.y );
        }
    }
}
*/

void lvl_platformer::PG_update( float dt )
{
    if( platGuy.pLeg )// climbing
    {
        if( climbState != 0 )// moving
        {
            platGuy.v = climbState*climbSpeed;
            platGuy.update(dt);
        }

        for( auto& pTgt : pTgt_pathVec )// mvHits on pPath
            if( pTgt->inUse && pTgt->pMH->hit(B1) ) platGuy.pLeg = nullptr;
    }
    else
    {
        platGuy.update( dt, gravity );
        PG_hit();
        PG_step(dt);
    }
}

void lvl_platformer::PG_hit()
{
    if( jump )
    {
        B1.Cr *= bounceAmp;
        for( auto& seg : pSegVec )
            if( seg->hit(B1) )
            {
                B1.v.y = -jumpSpeed;
                jump = false;
                break;
            }
        B1.Cr /= bounceAmp;
    }
    else
        for( auto& seg : pSegVec ) seg->hit(B1);

   /*
   for( auto& seg : pSegVec )
   {
        if( jump )
        {
            B1.Cr *= bounceAmp;
            if( seg->hit(B1) )
            {
                B1.v.y = -jumpSpeed;
                jump = false;
            }
            B1.Cr /= bounceAmp;
        }
        else seg->hit(B1);
   }
   */

   // pickups
   for( auto& pu : pickVec )
    if( pu.inUse && pu.hit( B1 ) )
    {
        pu.inUse = false;
        bankBal += pu.value;
        to_sf_string(bankMsgNum, bankBal);
    }

    // mvHits
//    for( auto& pMH : pMvHitVec ) pMH->hit( B1 );
    for( auto& pMH : pMvHitVec )// vs rigid mvHits
    {
        float vy0 = B1.v.y;
        vec2d v0 = B1.v;
        if( pMH->hit( B1 ) )
        {
            if( jump )
            {
                B1.v.y = -jumpSpeed;
                jump = false;
            }
            else if( !pMH->is_free )
            {
                vec2d N(0.0f,-1.0f);// = B1.pos - pMH->pos;
                float dvMag = ( B1.v - v0 ).mag();
                if( dvMag > 0.01f ) N = (B1.v - v0)/dvMag;
          //      N /= N.mag();
           //     float dV = B1.Cr*gravity.y;// standing
                float dV = B1.Cr*vy0;// standing
                B1.v = B1.v.to_base(N);
                B1.respond( dV, true );
                B1.v = B1.v.from_base(N);
            }
        }
    }

    // mvHits on pPath
    for( auto& pTgt : pTgt_pathVec ) if( pTgt->inUse ) pTgt->pMH->hit(B1);

    // jump off floor
    if( jump && B1.pos.y > ball::wdwH - (B1.r+2.0f) ) { B1.v.y = -jumpSpeed; jump = false; }
}

void lvl_platformer::PG_step( float dt )
{
    static float dStep = 0.0f;
    float pgW = 2.0f*B1.r;

    if(  platGuy.setNum != setStill )
    {
     //   dStep += 5.0f*(float)numReps*B1.maxVel;
        dStep += 5.0f*B1.maxVel*dt;
        if( platGuy.setNum == setRunRt )// running right
        {
            if( dStep > pgW )
            {
                platGuy.incrFrame();
                dStep = 0.0f;
            //    std::cerr << "frame incr right\n";
            }
            else if( dStep < -1.0f*pgW )// should never occur
            {
                platGuy.decrFrame();
                dStep = 0.0f;
          //      std::cerr << "frame decr right\n";
            }
        }
        else// running left
        {
            if( dStep < -1.0f*pgW )
            {
                platGuy.incrFrame();
                dStep = 0.0f;
            //    std::cerr << "frame incr left\n";
            }
            else if( dStep > pgW )// should never occur
            {
                platGuy.decrFrame();
                dStep = 0.0f;
           //     std::cerr << "frame decr left\n";
            }
        }
    }
}

void lvl_platformer::assignMessages()
{
    const sf::Font& rFont = *button::pFont;
    size_t fontSz = 16;
    float x0 = 200.0f;
    float y = 10.0f;
    float dy = 20.0f;

    // extra msgs
    msg1.setFont(rFont);
    msg1.setCharacterSize(fontSz);
    msg1.setString("setNum: ");// = "Ek = ";
    msg1.setPosition( x0, y );
    msgNum1.setFont(rFont);
    msgNum1.setCharacterSize(fontSz);
    msgNum1.setString("0");// = "0.0f";
    msgNum1.setPosition( x0+60, y );

    msg2.setFont(rFont);
    msg2.setCharacterSize(fontSz);
    msg2.setString("frIdx : ");// = "Ek = ";
    msg2.setPosition( x0, y+dy );
    msgNum2.setFont(rFont);
    msgNum2.setCharacterSize(fontSz);
    msgNum2.setString("0");// = "0.0f";
    msgNum2.setPosition( x0+60, y+dy );

    x0 = 320.0f;
    bankMsg.setFont(rFont);
    bankMsg.setCharacterSize(fontSz);
    bankMsg.setString("$ = ");// = "Ek = ";
    bankMsg.setPosition( x0, y );
    bankMsg.setColor( sf::Color::Cyan );
    bankMsgNum.setFont(rFont);
    bankMsgNum.setCharacterSize(fontSz);
    bankMsgNum.setString("0.0");// = "0.0f";
    bankMsgNum.setPosition( x0+30, y );
    bankMsgNum.setColor( sf::Color::Cyan );

    // basic msgs
    fontSz = 12; x0 = 20.0f; y = 10.0f; dy = 15.0f;
    helpMsg.setFont(rFont);
    helpMsg.setCharacterSize(16);
    helpMsg.setString("Key: H = help");
    helpMsg.setPosition( x0, y );
    helpMsg.setColor( sf::Color::Magenta );

    msgVec.push_back( sf::Text("Click to place spaceman", rFont, fontSz) );
    msgVec.push_back( sf::Text("Left, Right arrow keys to move", rFont, fontSz) );
    msgVec.push_back( sf::Text("Key J to jump", rFont, fontSz) );
    msgVec.push_back( sf::Text("F5 to mount/dismount ladder", rFont, fontSz) );
    msgVec.push_back( sf::Text("Up, Down arrows to climb ladder", rFont, fontSz) );
    msgVec.push_back( sf::Text("Spacebar to fire", rFont, fontSz) );
    msgVec.push_back( sf::Text("F4 to reset targets", rFont, fontSz) );
    msgVec.push_back( sf::Text("Key S to cycle frame set", rFont, fontSz) );
    msgVec.push_back( sf::Text("Key F to cycle frame index", rFont, fontSz) );

    for( auto& msg : msgVec )
    {
        msg.setColor( sf::Color::White );
        msg.setPosition( x0, y );
        y += dy;
    }
}

int lvl_platformer::initPathMovers(void)
{
    pathMover::Fk = 0.5f;
    pathMover::Fd = 0.5f;

    std::ifstream fin( "include/images/shipsFlapWedgeUfo_SSdata.txt" );
    if( !fin ) { std::cout << "no shipSS data\n"; return 1; }
    ship_SS.init(fin);
    ship_SS.img.createMaskFromColor( sf::Color::Black );
    ship_SS.txt.update( ship_SS.img );
    fin.close();

    // Load the Legs. One path only for now
    fin.open( "include/level/platformer_data/path_data1.txt" );
    if( !fin ) return 3;

    std::string pathFname;
    fin >> pathFname;
    std::ifstream finPath( pathFname.c_str() );
    if( !finPath ) return 4;
    makePath( finPath, pPath );
    finPath.close();

    std::vector< mvHit* > pMH_pathVec;
    if( !load_mvHits( fin, pMH_pathVec ) ) return 2;

    size_t numMvHits = pMH_pathVec.size();

    float vel = 0.0f;
    fin >> vel;

    Leg* pStartLeg = pPath;
    pTgt_pathVec.reserve( numMvHits );
    for( size_t i=0; i < numMvHits; ++i )
    {
        float s_st = 0.0f;
        fin >> s_st;
        size_t setnum = 0;
        fin >> setnum;
        std::cerr << "\n setNum: " << setnum << " s_st: " << s_st;

        pStartLeg = pPath;
        while( s_st > pStartLeg->len ) { s_st -= pStartLeg->len; pStartLeg = pStartLeg->next; }
    //    aniMover* pAM = new aniMover( pStartLeg, pMH_pathVec[i], vel, s_st );
    //    pMoverVec.push_back( pAM );
    //    pAM->mode = 'd';
    //    pAM->init2( &ship_SS, setnum );

        target* pTgt = new target( pStartLeg, pMH_pathVec[i], vel, s_st );
        pTgt_pathVec.push_back( pTgt );
        pTgt->mode = 'd';
        pTgt->init2( &ship_SS, setnum );
        pTgt->setH = 1 + setnum;
        pTgt->setK = 8;
     //   pAll_TgtVec.push_back( pTgt );
     //   pAM->showMvHit = true;
    }

    float v_min, v_max, v_0;
    fin >> v_min >> v_max >> v_0;
    float Fk_min, Fk_max;
    fin >> Fk_min >> Fk_max >> pathMover::Fk;
    float Fd_min, Fd_max;
    fin >> Fd_min >> Fd_max >> pathMover::Fd;

    fin.close();

    // path control
    sf::Text label("Path", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    buttonList* pList = new buttonList;
    sf::Vector2f pos0 = multiControl.pos;
    sf::Vector2f buttSz = multiControl.sz;

    pList->init( pos0.x, pos0.y + 3.3f*buttSz.y, buttSz.x, buttSz.y, label );
    pList->persist = 3;
    pList->ownButts = true;
    multiControl.pButtVec.push_back( pList );
 //   pathControl.inDefView = false;
    float fctY = 0.7f, dFctY = 1.5f;
    label.setString("path speed");
    pList->pButtVec.push_back( new slideBar( pos0.x + 60.0f, pos0.y + fctY*buttSz.y, 150, 20, label, v_min, v_max, v_0, std::bind( [this](float v) { for( auto& x : this->pTgt_pathVec ) x->v = v; }, std::placeholders::_1 ) ) );
    label.setString("spring K");
    pList->pButtVec.push_back( new buttonValOnHit( pos0.x + 60.0f, pos0.y + (fctY+=dFctY)*buttSz.y, 150, 20, label, Fk_min, Fk_max, pathMover::Fk, &pathMover::Fk ) );
    label.setString("damping");
    pList->pButtVec.push_back( new buttonValOnHit( pos0.x + 60.0f, pos0.y + (fctY+=dFctY)*buttSz.y, 150, 20, label, Fd_min, Fd_max, pathMover::Fd, &pathMover::Fd ) );

    listBK_rectVec.push_back( sf::RectangleShape() );
    listBK_rectVec.back().setPosition( pos0.x + 55.0f, pos0.y );
 //   listBK_rectVec.back().setSize( sf::Vector2f( 215.0f, 5.0f*buttSz.y ) );
    listBK_rectVec.back().setSize( sf::Vector2f( 215.0f, (fctY+dFctY)*buttSz.y ) );
    listBK_rectVec.back().setFillColor( sf::Color(50,50,50) );
    pListVec.push_back( pList );

    return 0;
}

int lvl_platformer::initShots()
{
    std::ifstream fin("include/level/platformer_data/shot_data.txt");
    if( !fin ) { std::cout << "\nNo gun_shot data"; return 1; }
    // load the shots
    shot shotTmp(fin);
    shotTmp.init2( &ship_SS, 7, 1.0f );
    float vMin, vMax;
    fin >> vMin >> vMax >> shotVel >> shot::minVelSq;
    float rMin, rMax;
    fin >> rMin >> rMax;
    float gMin, gMax;
    fin >> gMin >> gMax >> shotGrav.y;
    shotGrav.x = 0.0f;
    fin.close();
    size_t numShots = 20;
    pShotVec.reserve(numShots);
    for( size_t i=0; i< numShots; ++i )
    {
        shot* pShot = new shot(shotTmp);
        pShot->init2( &ship_SS, 7, 1.0f );
        pShotVec.push_back( pShot );
    }

    // shot control
  /*  sf::Text label("SHOTS", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    float PGCx = 400.0f, PGCy = 30.0f, PGCdy = 37.0f;
    shotControl.init( PGCx, PGCy - 15.0f, 50.0f, 25.0f, label );
    shotControl.persist = 3;
    shotControl.ownButts = true;
    button::RegisteredButtVec.push_back( &shotControl );    */

    sf::Text label("SHOTS", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    buttonList* pList = new buttonList;
    sf::Vector2f pos0 = multiControl.pos;
    sf::Vector2f buttSz = multiControl.sz;

    pList->init( pos0.x, pos0.y + 2.2f*buttSz.y, buttSz.x, buttSz.y, label );
    pList->persist = 3;
    pList->ownButts = true;
    multiControl.pButtVec.push_back( pList );

    label.setString("speed");
    std::function<void(float)> pFuncCall = [this]( float x ) { shotVel = x; numLoops = (size_t)( 1.0f + shotVel/(shotStepSz*pShotVec[0]->r) ); std::cout << "\n numLoops = " << numLoops; };
 //   shotControl.pButtVec.push_back( new buttonValOnHit( PGCx - 200.0f, PGCy + 0.0f*PGCdy, 120, 20, label, vMin, vMax, shotVel, pFuncCall ) );
    float fctY = 0.7f, dFctY = 1.5f;
    pList->pButtVec.push_back( new buttonValOnHit( pos0.x + 60.0f, pos0.y + fctY*buttSz.y, 150, 20, label, vMin, vMax, shotVel, pFuncCall ) );

    label.setString("gravity");
 //   shotControl.pButtVec.push_back( new buttonValOnHit( PGCx - 200.0f, PGCy + 1.0f*PGCdy, 120, 20, label, gMin, gMax, shotGrav.y, &(shotGrav.y) ) );
    pList->pButtVec.push_back( new buttonValOnHit( pos0.x + 60.0f, pos0.y + (fctY+=dFctY)*buttSz.y, 150, 20, label, gMin, gMax, shotGrav.y, &(shotGrav.y) ) );

    label.setString("radius");
    pFuncCall = [this]( float x )
    {
        for( auto& pShot : pShotVec )
        {
            pShot->r = x;
            pShot->img.setRadius(x);
            pShot->img.setOrigin(x,x);
        }
        numLoops = (size_t)( 1.0f + shotVel/(shotStepSz*x) );
        std::cout << "\n numLoops = " << numLoops;
    };
  //  shotControl.pButtVec.push_back( new buttonValOnHit( PGCx - 200.0f, PGCy + 2.0f*PGCdy, 120, 20, label, rMin, rMax, pShotVec[0]->r, pFuncCall ) );
    pList->pButtVec.push_back( new buttonValOnHit( pos0.x + 60.0f, pos0.y + (fctY+=dFctY)*buttSz.y, 150, 20, label, rMin, rMax, pShotVec[0]->r, pFuncCall ) );

 //   shotCbkRect.setPosition( PGCx - 205.0f, PGCy - 15.0f );
 //   shotCbkRect.setSize( sf::Vector2f( 190.0f, 3.0f*PGCdy + 15.0f ) );
 //   shotCbkRect.setFillColor( sf::Color(50,50,50) );

    listBK_rectVec.push_back( sf::RectangleShape() );
    listBK_rectVec.back().setPosition( pos0.x + 55.0f, pos0.y );
 //   listBK_rectVec.back().setSize( sf::Vector2f( 215.0f, 5.0f*buttSz.y ) );
    listBK_rectVec.back().setSize( sf::Vector2f( 215.0f, (fctY+dFctY)*buttSz.y ) );
    listBK_rectVec.back().setFillColor( sf::Color(50,50,50) );
    pListVec.push_back( pList );

    return 0;// all good
}

bool lvl_platformer::fireShot( vec2d pos, vec2d dir, float speed )
{
    if( pShotVec.size() == 0 ) return false;

    size_t bulletIdx = 0;// will iterate once through for an available shot, then give up
    do
    {
        bulletIdx = ( 1 + bulletIdx )%pShotVec.size();// std::cout << "\nfire!";
    } while( bulletIdx != 0 && !pShotVec.at(bulletIdx)->onFire() );

    if( bulletIdx != 0 )// found one!
    {
        mvHit* pMh = pShotVec.at(bulletIdx);
        pMh->v = speed*dir;
        pMh->setPosition( pos );
        return true;
    }

    return false;
}

/*
void lvl_platformer::shots_hit_allMvHits( float dt )
{
        for( auto& pShot : pShotVec )
        {
            if( !pShot->inUse )
            {
                if( pShot->pSS && pShot->frIdx < pShot->pSS->getFrCount( pShot->setNum ) )
                    pShot->update(dt);// run death animation
                continue;
            }

            pShot->v += gravity;
            float shotVmag = pShot->v.mag();
            if( shotVmag < 1.0f ) { pShot->inUse = false; continue;}
            vec2d shotVu = pShot->v/shotVmag;

            // find how many steps to the nearest target the shot is moving  towards
        //    vec2d sep;// from shot to target
            float stepsMin = 2.0f;
            target* pNearTgt = nullptr;// this may remain true. shot may then full step. No collision will ocurr
            float sepMag = 1.0f;
            float vt = 0.0f;
            for( auto& pTgt : pTgt_pathVec )// find stepsMin
            {
                if( !pTgt->inUse ) continue;
                // find # of steps to this target
                vec2d sep = pTgt->pMH->pos - pShot->pos;
                sepMag = sep.mag();
                vec2d sep_u = sep/sepMag;// unit vector toward target
                sepMag -= pTgt->pMH->r + pShot->r;// now distance between surfaces not centers
              //  if( sepMag < 0.0f ) { std::cout << "\n already overlapped"; pNearTgt = pTgt; break; }// hit test this one right away!
              //  if( sepMag < 0.0f ) { pNearTgt = pTgt; break; }// hit test this one right away! Already overlapped
             //   vec2d sep_u = sep/sepMag;// unit vector toward target
                vt = sep_u.dot( pShot->v - pTgt->pMH->v );// velocity component toward target
                if( vt > 0.0f )// shot is moving towards target
                {
                    if( vt < 0.1f ) vt = 0.1f;// reasonable minimum. Avoid div by 0 on line below
                    float h = sep.cross( shotVu );
                    if( h < 0.0f ) h *= -1.0f;// positive distance
                    h -= pTgt->pMH->r + pShot->r;
                    if( h < 0.0f )
                    {
                        if( sepMag < 0.0f ) { pNearTgt = pTgt; break; }
                        float steps = sepMag/vt;
                        if( !pNearTgt ) { stepsMin = steps; pNearTgt = pTgt; }// no initial value yet
                        else if( steps < stepsMin ) { stepsMin = steps; pNearTgt = pTgt; }// new minimum
                    }
                }
            }

            mvHit* pNearMH = nullptr;
            if( sepMag > 0.0f )// no target on shot. Try other mvHits. Total fail. Does not work
            {
                for( auto& pMH : pMvHitVec )
                {
                    vec2d sep = pMH->pos - pShot->pos;
                    sepMag = sep.mag();
                    vec2d sep_u = sep/sepMag;// unit vector toward target
                    sepMag -= pMH->r + pShot->r;// now distance between surfaces not centers
                    vt = sep_u.dot( pShot->v - pMH->v );// velocity component toward target
                    if( vt > 0.0f )// shot is moving towards target
                    {
                        if( vt < 0.1f ) vt = 0.1f;// reasonable minimum. Avoid div by 0 on line below
                        float h = sep.cross( shotVu );
                        if( h < 0.0f ) h *= -1.0f;// positive distance
                        h -= pMH->r + pShot->r;
                        if( h < 0.0f )
                        {
                            if( sepMag < 0.0f ) { pNearMH = pMH; break; }// new location
                            float steps = sepMag/vt;

                            if( !(pNearMH || pNearTgt) ) { stepsMin = steps; pNearMH = pMH; }// no initial value yet
                            else if( steps < stepsMin ) { stepsMin = steps; pNearMH = pMH; pNearTgt = nullptr; }// new minimum
                        }
                    }
                }
            }

            // some data collection. How often do shots full step vs baby step?
            static int fullStepCnt = 0, babyStepCnt = 0;
            if( sepMag > 0.0f && (stepsMin > 1.0f || !(pNearMH || pNearTgt) ) ) { pShot->update(dt); ++fullStepCnt; }// full step OK
            else
            {
                // baby step it
             //   std::cout << " baby";
                float dt_lo = dt/(float)numLoops;
                if( sepMag < 0.0f )
                {
                    std::cout << "\n sepMag < 0";
                    if( !(pNearMH || pNearTgt) ) std::cout << " no target";
                 //   if( pNearMH )  { if( pShot->hit( *pNearMH ) ) { pShot->update(dt); continue; } }// already too close. test before moving shot
                 //   if( pNearTgt ) { if( pShot->hit( *pNearTgt ) ) { pShot->update(dt); continue; } }
                    stepsMin = 0;
                }

                if( pNearMH || pNearTgt )
                {
                    size_t i0 = (size_t)(stepsMin*numLoops);
                    if( i0 > 0 ) pShot->update(i0*dt_lo);
                    for( size_t i=i0+1; i< numLoops; ++i )
                    {
                        pShot->update(dt_lo);
                        if( pNearMH ) { if( pShot->hit( *pNearMH ) ) { std::cout << "\n i0 = " << i0; pShot->update( (numLoops - i)*dt_lo); break; } } // test against only the nearest target
                        else if( pNearTgt ) pShot->hit( *pNearTgt );// test against only the nearest target
                    }
                    // stats out
                    if( pNearMH ) std::cout << "\n mvHit baby. vt = " << vt;
                    if( ++babyStepCnt%10 == 0 ) std::cout << "\n fullSteps: " << fullStepCnt << "  babySteps: " << babyStepCnt << " F/B = " << fullStepCnt/babyStepCnt;
                }
            }
        }// end of for each shot

    return;
}   */


void lvl_platformer::shots_hit_allMvHits( float dt )
{
        for( auto& pShot : pShotVec )
        {
            if( !pShot->inUse )
            {
           //     if( pShot->pSS && pShot->frIdx < pShot->pSS->getFrCount( pShot->setNum ) )
                    pShot->update(dt);// run death animation
                continue;
            }

            pShot->v += shotGrav*dt;
            float shotVmag = pShot->v.mag();
            if( shotVmag < 1.0f ) { pShot->inUse = false; continue;}
            vec2d shotVu = pShot->v/shotVmag;

            // find how many steps to the nearest target the shot is moving  towards
            float stepsMin = 2.0f;
            target* pNearTgt = nullptr;// this may remain true. shot may then full step. No collision will ocurr
            mvHit* pNearMH = nullptr;
            float sepMag = 1.0f;
            float vt = 0.0f;

            for( auto& pTgt : pTgt_pathVec )
                if( pTgt->inUse && isNearestMvHit( pShot, pTgt->pMH, pNearMH, stepsMin, sepMag, vt, shotVu ) ) pNearTgt = pTgt;

            if( sepMag > 0.0f )// no target on shot. Try other mvHits. Total fail. Does not work
                for( auto& pMH : pMvHitVec )
                    if( isNearestMvHit( pShot, pMH, pNearMH, stepsMin, sepMag, vt, shotVu ) ) pNearTgt = nullptr;

            if( pNearTgt ) pNearMH = nullptr;

            // some data collection. How often do shots full step vs baby step?
            static int fullStepCnt = 0, babyStepCnt = 0;
            if( sepMag > 0.0f && (stepsMin > 1.0f || !(pNearMH || pNearTgt) ) ) { pShot->update(dt); ++fullStepCnt; }// full step OK
            else
            {
                float dt_lo = dt/(float)numLoops;
                if( sepMag < 0.0f )
                {
                    std::cout << "\n sepMag < 0";
                    if( !(pNearMH || pNearTgt) ) { pShot->update(dt); std::cout << " no target"; continue; }
                    if( pNearMH ) { if( pShot->hit( *pNearMH ) ) { pShot->update(dt); continue; } }
                    if( pNearTgt ) { if( pShot->hit( *pNearTgt ) ) { pShot->update(dt); continue; } }

                    stepsMin = 0;
                }

                if( pNearMH || pNearTgt )
                {
                    size_t i0 = (size_t)(stepsMin*numLoops);
                    if( i0 > 0 ) pShot->update(i0*dt_lo);
                    size_t i=i0;
                    for( ; i< numLoops; ++i )
                    {
                        pShot->update(dt_lo);
                     //   if( pNearMH ) { if( pShot->hit( *pNearMH ) ) { std::cout << "\n i0 = " << i0; pShot->update( (numLoops - i)*dt_lo); break; } }
                    //    if( pNearTgt ) { if( pShot->hit( *pNearTgt ) ) { std::cout << "\n i0 = " << i0; pShot->update( (numLoops - i)*dt_lo); break; } }

                        if( pNearMH ) { if( pShot->hit( *pNearMH ) ) { std::cout << "\n i0 = " << i0 << " i = " << i; break; } }
                        if( pNearTgt ) { if( pShot->hit( *pNearTgt ) ) { std::cout << "\n i0 = " << i0 << " i = " << i; break; } }
                    }
                    if( i < numLoops ) pShot->update( (numLoops - i)*dt_lo);
                    // stats out
                    if( pNearMH ) std::cout << "\n mvHit baby. vt = " << vt;
                    if( ++babyStepCnt%10 == 0 ) std::cout << "\n fullSteps: " << fullStepCnt << "  babySteps: " << babyStepCnt << " F/B = " << fullStepCnt/babyStepCnt;
                }
            }
        }// end of for each shot

    return;
}

float fastsqrt( const float x )
{
    union
    {
        int i;
        float x;
    } u;
    u.x = x;
    u.i = (1<<29) + (u.i>>1) - (1<<22);
    return u.x;
}

bool lvl_platformer::isNearestMvHit( shot* pShot, mvHit* pMH, mvHit*& pNearMH, float& stepsMin, float& sepMag, float& vt, vec2d shotVu )
{
    vec2d sep = pMH->pos - pShot->pos;
    sepMag = sep.mag();

 /*   static LARGE_INTEGER totalElapTime;
    static int funcCalls = 0;
    if( ++funcCalls == 1 ) totalElapTime.QuadPart = 0;
    LARGE_INTEGER elapTime, stTime, endTime, freq;
    QueryPerformanceFrequency( &freq );
    QueryPerformanceCounter( &stTime );

    sepMag = sep.mag();

    QueryPerformanceCounter( &endTime );
    elapTime.QuadPart = endTime.QuadPart - stTime.QuadPart;
    elapTime.QuadPart *= 1000000;
    elapTime.QuadPart /= freq.QuadPart;
    totalElapTime.QuadPart += elapTime.QuadPart;

    if( funcCalls%100 == 0 ) std::cout << "\n totalElapTime = " << totalElapTime.QuadPart ; */

    vec2d sep_u = sep/sepMag;// unit vector toward target
    sepMag -= pMH->r + pShot->r;// now distance between surfaces not centers
    vt = sep_u.dot( pShot->v - pMH->v );// velocity component toward target
    if( vt > 0.0f )// shot is moving towards target
    {
        if( vt < 0.1f ) vt = 0.1f;// reasonable minimum. Avoid div by 0 on line below
        float h = sep.cross( shotVu );
        vec2d shotVtu( -shotVu.y, shotVu.x );// as LH norm
        if( h < 0.0f ) { h *= -1.0f; shotVtu *= -1.0f; }// positive distance. Other sense for projection direction
        h -= pMH->r + pShot->r;
        if( h < 0.0f )
        {
            h += pMH->r - pMH->project( shotVtu );// refine the calc
            if( h < 0.0f )// ok. They really will hit
            {
                if( sepMag < 0.0f )
                {
                    sepMag += pMH->r - pMH->project( -sep_u );// refine the calc
                    if( sepMag < 0.0f ) { pNearMH = pMH; stepsMin = 0.0f; return true; }// handle this hit
                }

                float steps = sepMag/vt;// finally. a nearest mvHit candidate
                if( !pNearMH || steps < stepsMin ) { stepsMin = steps; pNearMH = pMH; return true; }// new minimum
            }
        }
    }
    return false;
}

/*
bool lvl_platformer::isNearestMvHit( shot* pShot, mvHit* pMH, mvHit*& pNearMH, float& stepsMin, float& sepMag, float& vt, vec2d shotVu )
{
    vec2d sep = pMH->pos - pShot->pos;
    sepMag = sep.mag();
    vec2d sep_u = sep/sepMag;// unit vector toward target
    sepMag -= pMH->r + pShot->r;// now distance between surfaces not centers
    vt = sep_u.dot( pShot->v - pMH->v );// velocity component toward target
    if( vt > 0.0f )// shot is moving towards target
    {
        if( vt < 0.1f ) vt = 0.1f;// reasonable minimum. Avoid div by 0 on line below
        float h = sep.cross( shotVu );
        if( h < 0.0f ) h *= -1.0f;// positive distance
        h -= pMH->r + pShot->r;
        if( h < 0.0f )
        {
            if( sepMag < 0.0f ) { pNearMH = pMH; stepsMin = 0.0f; return true; }// new location
            float steps = sepMag/vt;
            if( !pNearMH || steps < stepsMin ) { stepsMin = steps; pNearMH = pMH; return true; }// new minimum
        }
    }
    return false;
}
*/
