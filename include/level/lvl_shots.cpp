#include "lvl_shots.h"

//const size_t lvl_shots::bodyCntDigits = 3;

bool lvl_shots::init( )
{
 //   Level::usingMapView = true;
    std::ifstream fin( "include/images/shipsFlapWedgeUfo_SSdata.txt" );
    if( !fin ) { std::cout << "no shipSS data\n"; return false; }
    ship_SS.init(fin);
    ship_SS.img.createMaskFromColor( sf::Color::Black );
    ship_SS.txt.update( ship_SS.img );
    fin.close();

//    mvHit::windowTopOpen = true;

   fin.open("include/level/lvl_shots_data/gun_shot_data.txt");
    if( !fin ) { std::cout << "\nNo gun_shot data"; return false; }
    // load the gun and shots
    theGun.init(fin);
    shot shotTmp(fin);
    shotTmp.init2( &ship_SS, 7, 1.0f );
    fin >> shot::minVelSq;
    fin >> floorDrag;
    pGuidedShot = new guidedShot;
    pGuidedShot->init(fin);
    pGuidedShot->init2( &ship_SS, 7, 1.0f );
    pGuidedShot->pTgt = nullptr;
    clusterShot* pCS = new clusterShot;
    pCS->init(fin);
    pCS->init2( &ship_SS, 7, 1.0f );
    pCS->p_FireFunc = [this](vec2d pos, vec2d dir, float speed) { return shot::fireShot(pRegShotVec,pos,dir,speed); };
    fin.close();

    theLaser.init( theGun.pos + theGun.wDir*theGun.W/2.0f, theGun.wDir, 1.0f, sf::Color::Red );

    size_t numShots = 20;
    pRegShotVec.reserve(numShots);
    pBadShotVec.reserve(numShots);
    pShotVec.reserve(numShots+2);
    for( size_t i=0; i< numShots; ++i )
    {
        shot* pShot = new shot(shotTmp);
        pShot->init2( &ship_SS, 7, 1.0f );
        pRegShotVec.push_back( pShot );
        pShotVec.push_back( pShot );
        pBadShotVec.push_back( new shot(shotTmp) );
        pBadShotVec.back()->img.setFillColor( sf::Color::Red );
    //    p_mhVec.push_back( static_cast<mvHit*>(pShot) );
    }
    pShotVec.push_back( pGuidedShot );
    pXtraShotVec.push_back( pGuidedShot );
    pShotVec.push_back( pCS );
    pXtraShotVec.push_back( pCS );

    XtraShotIdx = pXtraShotVec.size();
    theGun.p_FireFunc = [this](vec2d pos, vec2d dir, float speed)
    {
        if( pXtraShotVec.size() > XtraShotIdx )
        {
            if( pXtraShotVec[XtraShotIdx]->onFire() )
            {
                pXtraShotVec[XtraShotIdx]->v = speed*dir;
                pXtraShotVec[XtraShotIdx]->setPosition( pos + pXtraShotVec[XtraShotIdx]->project(dir)*dir );
                return static_cast<mvHit*>( pXtraShotVec[XtraShotIdx] );
            }
        }
        else if( pXtraShotVec.size() == XtraShotIdx ) return shot::fireShot(pRegShotVec,pos,dir,speed);
        else if( pXtraShotVec.size()+1 == XtraShotIdx ) { theLaser.state = true; }

        return static_cast<mvHit*>(nullptr);
    };

    joyButton* pJoyStk = ctUtil::init_gunControl( gunControl, theGun, 30.0f, 30.0f, theGun.bulletSpeed );
    if( pJoyStk )
    {
        pJoyStk->pFunc_v2d = [this](vec2d r) { if( theGun.is_free ) theGun.v = 0.2f*r; };
    }
    gunControl.pHitFunc = [this](){ ship::pFireMh = gunControl.sel ? &theGun : nullptr; Level::pMHfling = ship::pFireMh; };
    button::RegisteredButtVec.push_back( &gunControl );
    numLoops = (size_t)( 1.0f + theGun.bulletSpeed/(0.8f*pShotVec[0]->r) );
    std::cout << "\n init: numLoops = " << numLoops;

    sf::Text label("shot type", *button::pFont, 12);
    label.setColor( sf::Color(0,255,0) );
    shotTypeList.init( 30.0f, 80.0f, 80.0f, 17.0f, 2+(size_t)pXtraShotVec.size(), label, (size_t)pXtraShotVec.size() );
    shotTypeList.pFuncIdx = std::bind( [this](size_t i) { XtraShotIdx = i; }, std::placeholders::_1 );
    button::RegisteredButtVec.push_back( &shotTypeList );

    Level::pMHfling = nullptr;//&theGun;
    target::pKillCount = &numFlips;
    // an image
    gun::pTxtr = new sf::Texture;
    if( gun::pTxtr->loadFromFile( "include/images/guns/assaultRifle1.png" ) )
    {
    //    std::cout << "\n gun::Txtr loaded";
        float szx = (float)gun::pTxtr->getSize().x, szy = (float)gun::pTxtr->getSize().y;
        theGun.vtxVec[0].texCoords.x = szx-1.0f;     theGun.vtxVec[0].texCoords.y = 1.0f;// up lt
        theGun.vtxVec[1].texCoords.x = 1.0f; theGun.vtxVec[1].texCoords.y = 1.0f;// up rt
        theGun.vtxVec[2].texCoords.x = 1.0f; theGun.vtxVec[2].texCoords.y = szy-1.0f;// dn rt
        theGun.vtxVec[3].texCoords.x = szx-1.0f;     theGun.vtxVec[3].texCoords.y = szy-1.0f;// dn lt
        for( auto& vtx : theGun.vtxVec ) vtx.color = sf::Color::White;
    }
    else
    {
        delete gun::pTxtr;
        gun::pTxtr = nullptr;
    }

    // fire pattern
    fin.open("include/level/lvl_shots_data/firePattern_1.txt");
    if( !fin ) { std::cout << "\nNo fire pattern data"; return false; }
    firePatt1.init(fin);
    fin.close();

    int retVal = initTargetPath();
    if( retVal != 0 ){ std::cout << "pathMover init fail..\n"; return false; }

    // fire pattern reset
    label.setColor( sf::Color(0,0,0) );
    label.setString("FP reset");
    firePattButt.init( 50.0f, 600.0f, 50.0f, 25.0f, label );
    firePattButt.mode = 'I';
    firePattButt.pHitFunc = [this](void)
    {
        std::ifstream finFP("include/level/lvl_shots_data/firePattern_1b.txt");
        if( !finFP ) { std::cout << "\nNo fire pattern data"; return; }
        firePatt1b.velVec.clear();
        firePatt1b.delayVec.clear();
        firePatt1b.init(finFP);
        finFP.close();
        for( auto& s : pTargetVec )
        {
            ship* pShip = dynamic_cast<ship*>(s);
            if( pShip ){ pShip->pFirePat = &firePatt1b; pShip->fp_time = 0.0f; pShip->fp_idx = 0; break; }// 1st ship only
        }
    };
    button::RegisteredButtVec.push_back( &firePattButt );

    // circular fire pattern
    label.setString("FP circle");
    circFPButt.init( 50.0f, 640.0f, 50.0f, 25.0f, label );
    circFPButt.mode = 'I';
    circFPButt.pHitFunc = [this](void)
    {
        std::ifstream finFP("include/level/lvl_shots_data/firePattern_1circ.txt");
        if( !finFP ) { std::cout << "\nNo fire pattern data"; return; }
        size_t Nshots; float vMag, dt, dtLong;
        finFP >> Nshots >> vMag >> dt >> dtLong;
        finFP.close();
        makeCircleFP( firePatt1b, Nshots, vMag, dt, dtLong );
        for( auto& s : pTargetVec )
        {
            ship* pShip = dynamic_cast<ship*>(s);
            if( pShip ){ pShip->pFirePat = &firePatt1b; pShip->fp_time = 0.0f; pShip->fp_idx = 0; break; }// 1st ship only
        }
    };
    button::RegisteredButtVec.push_back( &circFPButt );

    // star field
    starArea.left = starArea.top = 0.0f;
    starArea.width = Level::winW;
    starArea.height = Level::winH;
    fin.open("include/level/lvl_shots_data/starField_1.txt");
    if( !fin ) { std::cout << "\nNo star field data"; return false; }
    fin >> starLayerPop[0] >> starLayerPop[1] >> starLayerPop[2];
    fin >> starLayerVel[0] >> starLayerVel[1] >> starLayerVel[2];
    fin.close();
    starFieldInit( starLayerVec, 3, starLayerPop, starArea );
    label.setString("STARS");
    starButt.init( 50.0f, 680.0f, 50.0f, 25.0f, label );
    starButt.setSel(true);
    button::RegisteredButtVec.push_back( &starButt );

    // floor drag
    label.setString("floor drag");
    dragStrip.init( 150.0f, 680.0f, 150.0f, 20.0f, label, 0.0f, 3.0f, floorDrag, &floorDrag );
    button::RegisteredButtVec.push_back( &dragStrip );

    // body cont
    fin.open( "include/images/flipDigitsSS_data.txt" );
    if( !fin ) { std::cout << "\nNo flipDigit SS data"; return false; }
    flipDigit_SS.init(fin);
    for( auto& bc : bodyCnt ) { bc.scale = 0.4f; bc.callRecursive = true; }
    initFlipArray( bodyCnt, bodyCntDigits, flipDigit_SS, 0, sf::Vector2f(1050.0f,640.0f), 5.0f, 0.1f, 0.01f );

    // goto main menu button
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );

    return true;
}

bool lvl_shots::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        sf::Keyboard::Key K = rEvent.key.code;
        if ( K == sf::Keyboard::F6 )// cycle target designated for guidedShot
        {
            if( pTargetVec.size() > 0 && pGuidedShot )
            {
                static size_t tgtIdx = 0;
             //   if( pTargetVec[tgtIdx]->inUse ) { pGuidedShot->pTgt = pTargetVec[tgtIdx]; pGuidedShot->pTgt->onHit(0.1f); }
             //   else pGuidedShot->pTgt = nullptr;
             //   tgtIdx = (1+tgtIdx)%pTargetVec.size();

                // xcode
                pGuidedShot->pTgt = nullptr;
                size_t tgtIdx_curr = tgtIdx;
                tgtIdx = (1+tgtIdx)%pTargetVec.size();
                // try until wrap around on tgtIdx value
                while( tgtIdx != tgtIdx_curr && !pTargetVec[tgtIdx]->inUse ) tgtIdx = (1+tgtIdx)%pTargetVec.size();
//                if( tgtIdx != tgtIdx_curr ) { pGuidedShot->pTgt = pTargetVec[tgtIdx]; pGuidedShot->pTgt->onHit(0.1f); }
                if( pTargetVec[tgtIdx]->inUse ) { pGuidedShot->pTgt = pTargetVec[tgtIdx]; pGuidedShot->pTgt->onHit(0.1f); }
            }
        }
        else if ( K == sf::Keyboard::K )// kill all
        {
            for( auto& pTgt : pTargetVec ) pTgt->inUse = false;
        }
        else if( K == sf::Keyboard::Num1 ) numFlips += 1;
        else if( K == sf::Keyboard::Num5 ) numFlips += 5;
    }

    return true;
}

void lvl_shots::update( float dt )
{
    if( starButt.sel ) starFieldUpdate( starLayerVec, starLayerVel, starArea );

    // v based numReps!
    static float shotVelLast = theGun.bulletSpeed;
    if( gunControl.sel && (theGun.bulletSpeed - shotVelLast)*(theGun.bulletSpeed - shotVelLast) > 1.0f )
    {
        numLoops = (size_t)( 1.0f + theGun.bulletSpeed/(0.8f*pShotVec[0]->r) );
        std::cout << "\n init: numLoops = " << numLoops;
        shotVelLast = theGun.bulletSpeed;
    }

    bool anyAliveNow = false;
    static bool anyAliveLast = false;
    for( auto& pTgt : pTargetVec )// pTgt->update();
    {
        if( !( pTgt->pLeg || pTgt->pVelPer ) )// apply drag
        {
            float magVel = pTgt->pMH->v.mag();
            if( magVel > dt*floorDrag )
                pTgt->pMH->v -= dt*floorDrag*pTgt->pMH->v/magVel;
            else// just stop
                pTgt->pMH->v *= 0.0f;
        }

        pTgt->update(dt);
        if( pTgt->inUse ) anyAliveNow = true;
    }

    if( anyAliveLast && !anyAliveNow ) pathVtxVec.clear();// last Tgt just died
    anyAliveLast = anyAliveNow;// maintain last value

    if( gunControl.sel )
    {
        if( theGun.is_free )
        {
            float magVel = theGun.v.mag();
            if( magVel > dt*floorDrag )
                theGun.v -= dt*floorDrag*theGun.v/magVel;
            else// just stop
                theGun.v *= 0.0f;
        }

        theGun.update(dt);
        theLaser.update( theGun.pos + theGun.wDir*theGun.W/2.0f, theGun.wDir );
        if( pXtraShotVec.size()+1 == XtraShotIdx ) theLaser.state = theGun.is_firing;
    }

    // collision testing and response

    // theLaser vs all targets
    if( theLaser.state )
        for( auto& pTgt : pTargetVec ) theLaser.hit( *pTgt );

    for( size_t i=0; i+1< pTargetVec.size(); ++i )
        for( size_t j=i+1; j< pTargetVec.size(); ++j )
            if( pTargetVec[i]->inUse && pTargetVec[j]->inUse )
                pTargetVec[i]->pMH->hit( *pTargetVec[j]->pMH );

    // targets vs theGun
    if( gunControl.sel ) for( auto& pTgt : pTargetVec ) if( pTgt->inUse ) pTgt->pMH->hit( theGun );

    // xcode. Targets vs shots
    if( pTargetVec.size() > 0 )
        for( auto& pShot : pShotVec )
        {
            if( pShot->inUse ) update_hit_shot( pShot, dt );
            else pShot->update(dt);// run death animation
        }

    // bad shots vs theGun
    for( auto& pShot : pBadShotVec )
    {
        if( pShot->inUse )
        {
            float step = 2.0f;
            if( gunControl.sel && willHitMvHit( pShot, &theGun, step ) )
            {
                vec2d vRel = pShot->v - theGun.v;
                float vRelMag = vRel.mag();
                float babyStepNum = vRelMag/(0.8f*pShot->r);
                float i = step*vRelMag;
                float dt_lo = dt/babyStepNum;
                pShot->update( i*dt_lo );// big step
                // baby steps
                for( ; i<vRelMag; i += vRelMag/babyStepNum )
                {
                    pShot->update(dt_lo);
                    if( pShot->hit(theGun) ) break;
                }

                if( i < vRelMag ) pShot->update( (vRelMag-i)*dt_lo );
            }
            else pShot->update(dt);
        }
        else pShot->update(dt);
    }

    // body count
//    int currCnt = 0;
 //   for( const auto& pTgt : pTargetVec ) if( pTgt->inUse ) ++currCnt;
 //   if( currCnt < lastTgtCnt ) numFlips += lastTgtCnt - currCnt;
 //   lastTgtCnt = currCnt;
    if( numFlips > 0 ) if( !bodyCnt->flipping ) { bodyCnt->initFlip(); --numFlips; }
//    updateFlipArray( bodyCnt, bodyCntDigits );
    bodyCnt->updateFlipDisplay();

    return;
}

// used by update() above
void lvl_shots::update_hit_shot( shot* pShot, float dt )
{
    // find how many steps to the nearest target the shot is moving  towards
    float stepsMin = 2.0f;
    target* pNearTgt = nullptr;// this may remain true. shot may then full step. No collision will ocurr
    for( auto& pTgt : pTargetVec )// find stepsMin
    {
        if( !pTgt->inUse ) continue;

        // new
        float steps = 1.0f;
        if( willHitMvHit( pShot, pTgt->pMH, steps ) )
        {
            if( !pNearTgt ) { stepsMin = steps; pNearTgt = pTgt; }// no initial value yet
            else if( steps < stepsMin ) { stepsMin = steps; pNearTgt = pTgt; }
        }
    }

    // hit test against a lineSeg
 /*   if( !Lseg.is_bulletProof )
    {
        vec2d sepSeg = Lseg.getNearestPoint( pShot->pos );
        float sepSegMag = sepSeg.mag();
        sepSegMag -= pShot->r;
        vec2d sep_u = sepSeg/sepSegMag;// unit vector toward target
        float vt = sep_u.dot( pShot->v );// velocity component toward target
        if( vt > 0.0f )// shot is moving towards target
        {
            if( vt < 0.1f ) vt = 0.1f;// reasonable minimum. Avoid div by 0 on line below
            float steps = sepSegMag/vt;
            if( !pNearTgt || steps < stepsMin )
            {
                if( steps < 1.0f )// baby step
                {
                    float dt_lo = dt/(float)numLoops;
                    for( size_t i=0; i< numLoops; ++i )
                    {
                        pShot->update(dt_lo);
                        Lseg.hit( *pShot );// test against only the nearest target
                    }
                    return;// next shot
                }
            }
        }
    }// end vs non bullet proof Lseg    */

    // some data collection. How often do shots full step vs baby step?
    static int fullStepCnt = 0, babyStepCnt = 0;
    if( stepsMin > 1.0f || !pNearTgt ) { pShot->update(dt); ++fullStepCnt; }// full step OK
    else
    {
        // baby step it
        float dt_lo = dt/(float)numLoops;
        size_t i = (size_t)(numLoops*stepsMin);// big initial step
        pShot->update(dt_lo*(float)i);
        for( ; i< numLoops; ++i )// baby steps until hit occurs
        {
            pShot->update(dt_lo);
            if( pShot->hit( *pNearTgt ) ) break;// test against only the nearest target
        }

        if( numLoops > i ) pShot->update( dt_lo*(float)(numLoops-i) );// remainder of step
        // stats out
     //   if( ++babyStepCnt%10 == 0 ) std::cout << "\n fullSteps: " << fullStepCnt << "  babySteps: " << babyStepCnt << " F/B = " << fullStepCnt/babyStepCnt;
    }
    return;
}

// non member
bool willHitMvHit( shot* pShot, mvHit* pMh, float& stepFrac )
{
    vec2d sep = pMh->pos - pShot->pos;
    float sepMag = sep.mag();
    sepMag -= pMh->r + pShot->r;// now distance between surfaces not centers
    if( sepMag < 0.0f ) { stepFrac = 0.0f; return true; }// hit test this one right away! Already overlapped
    vec2d sep_u = sep/sepMag;// unit vector toward target
    vec2d vRel = pShot->v - pMh->v;
    float vt = sep_u.dot( vRel );// velocity component toward target
    if( vt > 0.0f )// shot is moving towards target
    {
        if( vt < 0.1f ) vt = 0.1f;// reasonable minimum. Avoid div by 0 on line below
        float steps = sepMag/vt;
        if( steps < 1.0f )
        {
            vec2d vRelu = vRel/vRel.mag();
            float miss = sep.cross( vRelu );
            if( miss < pShot->r+pMh->r && miss > -(pShot->r+pMh->r) )// will hit
            {
                stepFrac = steps;
                return true;
            }
        }
    }

    return false;
}

void lvl_shots::draw( sf::RenderTarget& RT ) const
{
    if( starButt.sel ) starFieldDraw( starLayerVec, RT );
    if( pathVtxVec.size() > 1 ) RT.draw( &pathVtxVec[0], pathVtxVec.size(), sf::LinesStrip );
    for( auto& pTgt : pTargetVec ) pTgt->draw(RT);
    for( auto& pShot : pShotVec ) pShot->draw(RT);
    for( auto& pShot : pBadShotVec ) pShot->draw(RT);
    if( gunControl.sel ) { theGun.draw(RT);  theLaser.draw(RT); }
//    drawFlipArray( bodyCnt, bodyCntDigits, RT );
    bodyCnt->draw(RT);
    return;
}

void lvl_shots::cleanup()
{
    std::cout << "\nlvl_shots: deleting " << pRegShotVec.size() << " regular shots";
    for( auto& x : pRegShotVec ) delete x;
    pRegShotVec.clear();

    std::cout << "\nlvl_shots: deleting " << pXtraShotVec.size() << " extra shots";
    for( auto& x : pXtraShotVec ) delete x;
    pXtraShotVec.clear();

    std::cout << "\nlvl_shots: deleting " << pRegShotVec.size() << " baddie shots";
    for( auto& x : pBadShotVec ) delete x;
    pBadShotVec.clear();

    std::cout << "\nlvl_shots: deleting " << pTargetVec.size() << " targets from path";
    for( auto& x : pTargetVec ) { if( x->pMH ) delete x->pMH; delete x; }
    pTargetVec.clear();

    for( auto& x : pathVec ) { if( x ) std::cout << "\ndestroying " << destroyPath( x ) << " Legs"; }
    pathVec.clear();

    for( auto& x : velPathVec ) { if( x ) std::cout << "\ndestroying " << destroyVelPath( x ) << " velPeriods\n"; }
    velPathVec.clear();

    for( auto& x : blockPathVec ) { if( x ) std::cout << "\ndestroying " << destroyPath( x ) << "block paths\n"; }
    velPathVec.clear();

    if( gun::pTxtr )// see init() and pGun
    {
        delete gun::pTxtr;
        gun::pTxtr = nullptr;
        std::cout << "\nlvl1 cleanup: gun::Txtr deleted";
    }

    return;
}

int lvl_shots::initTargetPath()
{
  /*  std::ifstream fin("include/level/lvl_shots_data/velPeriod_list.txt");
    if( !fin ) { std::cout << "\nNo velPeriod data"; return 17; }
    std::string pathFname;
    std::ifstream finPath;
    while( fin >> pathFname )
    {
        pathFname = "include/level/lvl_shots_data/velPeriodData/" + pathFname + ".txt";
        finPath.open( pathFname.c_str() );
        if( !finPath ) continue;
        velPathVec.push_back( makeVelPath(finPath) );
        finPath.close();
    }
    fin.close();    */

    // load pPath, load targets
 //   fin.open( "include/level/lvl_shots_data/path_data1.txt" );
    std::ifstream fin( "include/level/lvl_shots_data/path_data1.txt" );
    if( !fin ) return 3;

    std::vector<sf::Text> TitleVec;
    std::vector< std::function<void(void)> > pFuncVec;
    sf::Text label("path select", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
 //   TitleVec.push_back( label );
    float posx = 30.0f, posy = 120.0f, W = 70.0f, H = 30.0f;
    pathSelect.init( posx, posy, W, H, label );
    pathSelect.persist = 3;
    pathSelect.ownButts = true;

 //   for( const auto& str : pathNameVec ) { label.setString( str.c_str() ); TitleVec.push_back( label ); }
 //   for( size_t i=0; i<velPathVec.size(); ++i) { label.setString("velPer"); TitleVec.push_back( label ); }
 //   pFuncVec.push_back( [](void) { std::cout << "\n path selector"; } );

    std::function<void(size_t)> pFuncAll = [this](size_t i)// regular paths
    {
        float s_st = 0.0f;
        if( pathVec.size() > i && pathVec[i] )
        {
            float vel = pTargetVec[0]->v;
            if( vel < 0.0f ) vel *= -1.0f;
            float dt = pathLength(*pathVec[i])/(vel*NshipsVec[i]);
            size_t j = 0;// loop counter
            for( auto& pTgt : pTargetVec )
            {
                if( !pTgt->inUse && pTgt->tLaunch < 0.0f )// target is avail
                {

                    pTgt->pVelPer = nullptr;// path only
                    pTgt->reset( pathVec[i], s_st );
                    s_st+=dt;
                    if( ++j >= NshipsVec[i] ) break;// all ships found
                }
            }
            if( button::registerUnique( &pathControl ) ) pathControl.setSel(false);

            pathVtxVec.clear();
            tracePath( *pathVec[i], 5.0f, pathVtxVec, sf::Color::Blue, sf::Color::Red );
         //   lastTgtCnt = 0;
        }
    };

    size_t numRegPaths=0, numFuncPaths=0, numFlyPaths = 0;
    fin >> numRegPaths >> numFuncPaths >> numFlyPaths;
    size_t totalPaths = numRegPaths + numFuncPaths + numFlyPaths + 1;

    pathVec.reserve(totalPaths);// extra for 2 flyPaths + 3*func path
    pathNameVec.reserve(totalPaths);// extra for 2 flyPaths + 3*func path
    NshipsVec.reserve(totalPaths);

    std::string pathFname;
    std::ifstream finPath;

    // reg path list
    label.setString("reg Paths");
    TitleVec.push_back( label );
    for( size_t i=0; i<numRegPaths; ++i )
    {
        fin >> pathFname;
        label.setString( pathFname.c_str() );
        TitleVec.push_back( label );
        pathFname = "include/Leg_types/paths/" + pathFname + ".txt";
        finPath.open( pathFname.c_str() );
        if( !finPath ) return 4;
        pathVec.push_back(nullptr);
        makePath( finPath, pathVec.back() );
        finPath.close();

        float Nships = 0; fin >> Nships;
        NshipsVec.push_back( Nships );
    }
    pFuncVec.push_back( nullptr );// for reg paths home butt
    for( size_t i=0; i<pathVec.size(); ++i) pFuncVec.push_back( [pFuncAll, i](void){ pFuncAll(i); } );
    buttonList* pList_1 = new buttonList;
    pList_1->init( posx, posy+H, W, H, H, TitleVec, pFuncVec, 'R' );
    pList_1->persist = 2;
    pList_1->ownButts = true;
    pathSelect.pButtVec.push_back( pList_1 );

    // func paths
    pFuncVec.clear();
    TitleVec.clear();
    label.setString("funcPaths");
    TitleVec.push_back( label );

    // by hand
 /*   label.setString("ellipse");
    TitleVec.push_back( label );
    NshipsVec.push_back( 1 );
    float a = 80.0f, b = 150.0f;
    funcLeg* pFL = new funcLeg( 600.0f, 350.0f, -a*0.99f, a*0.99f, 'c', [a,b](float x){ return b*sqrtf( 1 - x*x/(a*a) ); }, [a,b](float x){ return -2.0f*b*x/( a*a*sqrtf( 1 - x*x/(a*a) ) ); } );
    b *= -1.0f; a *= -1.0f; new funcLeg( *pFL, -a*0.99f, a*0.99f, 'c', [a,b](float x){ return b*sqrtf( 1 - x*x/(a*a) ); }, [a,b](float x){ return -2.0f*b*x/( a*a*sqrtf( 1 - x*x/(a*a) ) ); } );
    pFL->prev = pFL->next;
    if( pFL->prev ) pFL->prev->next = pFL;
    pathVec.push_back(pFL); */

    for( size_t i=0; i<numFuncPaths; ++i )
    {
        float Nships = 0;
        fin >> pathFname >> Nships;
        label.setString( pathFname.c_str() );
        TitleVec.push_back( label );
        NshipsVec.push_back( Nships );
        pathFname = "include/Leg_types/paths/" + pathFname + ".txt";
        finPath.open( pathFname.c_str() );
        if( !finPath ) return 5;
        pathVec.push_back(nullptr);
        makeFuncPath( finPath, pathVec.back() );
        finPath.close();
    }
    pFuncVec.push_back( nullptr );// for func paths home butt
    for( size_t i=numRegPaths; i<pathVec.size(); ++i) pFuncVec.push_back( [pFuncAll, i](void){ pFuncAll(i); } );
    pList_1 = new buttonList;
    pList_1->init( posx, posy+2*H, W, H, H, TitleVec, pFuncVec, 'R' );
    pList_1->persist = 2;
    pList_1->ownButts = true;
    pathSelect.pButtVec.push_back( pList_1 );

    // other
    pFuncVec.clear();
    TitleVec.clear();
    label.setString("other");
    TitleVec.push_back( label );
//    size_t numFlyPaths = 0;
 //   fin >> numFlyPaths;
    for( size_t i=0; i<numFlyPaths; ++i )
    {
        float Nships = 0;
        fin >> pathFname >> Nships;
        label.setString( pathFname.c_str() );
        TitleVec.push_back( label );
        NshipsVec.push_back( Nships );
        pathFname = "include/Leg_types/paths/" + pathFname + ".txt";
        finPath.open( pathFname.c_str() );
        if( !finPath ) return 5;
        pathVec.push_back(nullptr);
        makeFlyPath_lin( finPath, pathVec.back() );
        finPath.close();
    }
    pFuncVec.push_back( nullptr );// for func paths home butt
    for( size_t i=numRegPaths+numFuncPaths; i<pathVec.size(); ++i) pFuncVec.push_back( [pFuncAll, i](void){ pFuncAll(i); } );// fly paths
    // velPers
    std::ifstream finVP("include/level/lvl_shots_data/velPeriod_list.txt");
    if( !finVP ) { std::cout << "\nNo velPeriod data"; return 17; }
    while( finVP >> pathFname )
    {
        pathFname = "include/level/lvl_shots_data/velPeriodData/" + pathFname + ".txt";
        finPath.open( pathFname.c_str() );
        if( !finPath ) continue;
        velPathVec.push_back( makeVelPath(finPath) );
        finPath.close();
        label.setString( "velPer" );
        TitleVec.push_back( label );
    }
    finVP.close();
    pFuncAll =  [this](size_t i)// velPeriod paths
    {
        if( velPathVec.size() < i+1 ) return;
        if( linLegVec.size() < 1 ) return;
        button::unRegisterMe( &pathControl );
        float t0 = 0.0f;
        size_t j = 0;// loop counter
        for( auto& pTgt : pTargetVec )
        {
            if( !pTgt->inUse && pTgt->tLaunch < 0.0f )// target is avail
            {
                pTgt->reset( &linLegVec[i], velPathVec[i], vec2d( 200.0f, 400.0f ), t0+=50.0f );
                if( ++j >= 4 ) break;// all ships found
            }
        }
    };
    for( size_t i=0; i<velPathVec.size(); ++i) pFuncVec.push_back( [pFuncAll, i](void){ pFuncAll(i); } );
    // block
    label.setString("block"); TitleVec.push_back( label );
    pFuncVec.push_back( [this](void)
    {
        size_t j=0;
        float s_st = 0.0f, dt = 30.0f;
        size_t iPath = 0, rows = blockPathVec.size();
        Leg* pLeg = blockPathVec[iPath];
        for( auto& pTgt : pTargetVec )
        {
            if( !pTgt->inUse && pTgt->tLaunch < 0.0f )// target is avail
            {
                pTgt->pVelPer = nullptr;// path only
                pTgt->reset( pLeg, s_st );
                s_st+=dt;
                if( ++j >= 9 ) break;// all ships found
                iPath = (iPath+1)%rows;
                pLeg = blockPathVec[iPath];
            }
        }
        if( button::registerUnique( &pathControl ) ) pathControl.setSel(false);
    } );
    // free
    label.setString("free"); TitleVec.push_back( label );
    pFuncVec.push_back( [this](void)
    {
     //   lastTgtCnt = 0;
        float x0 = 0.0f;
        size_t j=0;
        for( auto& pTgt : pTargetVec )
        {
            if( !pTgt->inUse && pTgt->tLaunch < 0.0f )// target is avail
            {
                pTgt->pVelPer=nullptr; pTgt->pLeg=nullptr;
                pTgt->reset( nullptr, nullptr, vec2d(x0+=250.0f,300.0f) );
                if( ++j >= 4 ) break;// all ships found
            }
        }
        button::unRegisterMe( &pathControl );
    } );

    pList_1 = new buttonList;
    pList_1->init( posx, posy+3*H, W, H, H, TitleVec, pFuncVec, 'R' );
    pList_1->persist = 2;
    pList_1->ownButts = true;
    pathSelect.pButtVec.push_back( pList_1 );
    button::RegisteredButtVec.push_back( &pathSelect );


    // mvHit target and ship loading
    std::vector< mvHit* > pMH_pathVec;
    if( !load_mvHits( fin, pMH_pathVec ) ) return 8;

    size_t numMvHits = pMH_pathVec.size();
    for( size_t i=0; i<velPathVec.size(); ++i ) linLegVec.push_back( linLeg( 200.0f*(i+1), 40.0f, 200.0f*(i+1), 300.0f ) );

    size_t iPathIdx = 0;
    Leg* pLeg = pathVec[iPathIdx];
    float v = 4.0f;
 //   size_t Nships = 3;
    float dt = pathLength(*pLeg)/(v*NshipsVec[iPathIdx]);
    float s_st = 0.0f;

    pTargetVec.reserve( 2*numMvHits );
    for( size_t i=0; i < numMvHits; ++i )
    {
        size_t setnum = 0;
        fin >> setnum;

        char tgtType = 't';// or 's' for ship
        fin >> tgtType;
        ship* pShip = nullptr;
        target* pTgt = nullptr;

        if( tgtType == 's' )// ship
        {
            pShip = new ship( pLeg, pMH_pathVec[i], v, 0.0f );//s_st );
            pTgt = static_cast<target*>(pShip);
            pShip->shotOfst = vec2d( 0.0f, pShip->pMH->r + 5.0f );
            pShip->pFirePat = &firePatt1;
            pShip->fp_time = -100.0f;
            fin >> pShip->fireOnTarget;
            pShip->pFireFunc = [this](vec2d pos, vec2d dir, float speed) { return shot::fireShot(pBadShotVec,pos,dir,speed); };
        }
        else // target
            pTgt = new target( pLeg, pMH_pathVec[i], 4.0f, 0.0f );//s_st );

        if( pTgt )
        {
            pTargetVec.push_back( pTgt );
            pTgt->pVelPer = velPathVec.size() > 0 ? velPathVec[0] : nullptr;
            if( !pLeg && pTgt->pVelPer ) pTgt->oldPos = pTgt->pVelPer->v0;
            pTgt->mode = 'd';
            pTgt->init2( &ship_SS, setnum );
            pTgt->setH = 1 + setnum;
            pTgt->setK = 8;
        //    if( i < NshipsVec[iPathIdx] ) pTgt->tLaunch = s_st;
        //    else pTgt->tLaunch = -1.0f;
            pTgt->tLaunch = -1.0f;
            pTgt->inUse = false;
            pTgt->frIdx = pTgt->pSS->getFrCount( setnum );
            s_st += dt;
        }
    }

    fin.close();

    for( size_t i=0; i < numMvHits; ++i )
        pTargetVec.push_back( pTargetVec[i]->clone() );

    // block path
    fin.open( "include/level/lvl_shots_data/blockPath1.txt" );
    if( !fin ) { std::cout << "\nNo block path data"; return 17; }
    makeBlockPath( fin, blockPathVec );
    fin.close();

    // init control
    fin.open("include/level/lvl_shots_data/control_data.txt");
    if( !fin ) return 9;

    char mvMode = 'd';
    fin >> mvMode;
    float Fk_min, Fk_max;
    fin >> Fk_min >> pathMover::Fk >> Fk_max;
    float Fd_min, Fd_max;
    fin >> Fd_min >> pathMover::Fd >> Fd_max;
    float v_min, v_0, v_max;
    fin >> v_min >> v_0 >> v_max;

    fin.close();

    vec2d Cpos;
    label.setString("Path");
    label.setColor( sf::Color(0,0,0) );
    Cpos.x = 50.0f; Cpos.y = 560.0f;
    pathControl.init( Cpos.x, Cpos.y, 50.0f, 25.0f, label );
    pathControl.persist = 3;
    pathControl.ownButts = true;
 //   pathControl.inDefView = false;
    label.setString("path speed");
    pathControl.pButtVec.push_back( new slideBar( Cpos.x+70, Cpos.y-50, 150, 20, label, v_min, v_max, v_0, std::bind( [this](float v) { for( auto& x : this->pTargetVec ) x->v = v; }, std::placeholders::_1 ) ) );
    label.setString("spring K");
    pathControl.pButtVec.push_back( new buttonValOnHit( Cpos.x+70, Cpos.y, 150, 20, label, Fk_min, Fk_max, pathMover::Fk, &pathMover::Fk ) );
    label.setString("damping");
    pathControl.pButtVec.push_back( new buttonValOnHit( Cpos.x+70, Cpos.y+50, 150, 20, label, Fd_min, Fd_max, pathMover::Fd, &pathMover::Fd ) );
    button::RegisteredButtVec.push_back( &pathControl );

    return 0;// all good!
}
