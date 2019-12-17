#include "lvl1.h"

bool lvl1::init( )
{
    std::ifstream fin("include/level/lvl1_data/enviro_data.txt");
    if( !fin ) { std::cout << "no enviro\n"; return false; }

    fin >> gravity_on;
    fin >> gravity.y; gravity.x = 0.0f;// = 0.0f;
    fin >> mvHit::wdwCf;
    fin >> mvHit::windowTopOpen;
    fin >> mvHit::drag;
    float mapScaleX = 1.0f, mapScaleY = 1.0f;
    fin >> mapScaleX >> mapScaleY;
    mapW = mapScaleX*winW;
    mapH = mapScaleY*winH;
    fin >> viewOfstX >> viewOfstY >> viewX.grabFactor; viewY.grabFactor = viewX.grabFactor;// as fraction of winW
    viewOfstX *= winW; viewOfstY *= winH;
    mapView.reset( sf::FloatRect( 0.0f, 0.0f, winW, winH ) );
    size_t recorderFrames = 1000, saveFreq = 1;
    fin >> recorderFrames >> saveFreq;
    fin.close();

    fin.open( "include/level/lvl1_data/mvHit_data.txt" );
    if( !fin ) { std::cout << "no mvHit_data\n"; return false; }
    if( !load_mvHits( fin, pMvHitVec ) ) { std::cout << "no load_mvHits\n"; return false; }

    fin.close();
    // find the special ones
    if( pMvHitVec.size() > 0 ) pMHfling = pMvHitVec.front();// flinger
    for( auto& x : pMvHitVec ) { pGun_1 = dynamic_cast<gun*>(x); if( pGun_1 ) break; }// 1st will do
    for( auto& x : pMvHitVec ) { pXpgA = dynamic_cast<expandPolygon*>(x); if( pXpgA ) break; }// 1st will do

    fin.open( "include/level/lvl1_data/seg_data.txt" );
    if( !fin ) { std::cout << "no segHit_data\n"; return false; }
   if( !loadSegs( fin, pSegVec ) ) { std::cout << "no load_segHits\n"; return false; }
   std::cout << "\nloaded segHits: " << pSegVec.size();
    fin.close();
    // 1st will do

    fin.open( "include/level/lvl1_data/tank_data.txt" );
    if( !fin ) { std::cout << "no tank_data\n"; return false; }
    size_t Ntanks = 0; fin >> Ntanks;
    for( size_t i=0; i<Ntanks && fin; ++i )
    {
        wvTankVec.push_back( waveTank() );
        wvTankVec.back().init(fin);
    }
    float diam1 = 70.0f, fluidVel = 3.5f;
    fin >> diam1 >> fluidVel;
    fin.close();

    // safe!
    // controls?
    sf::Text label("gravity", *button::pFont, 12);
    std::function<void(float)> p_SetFunc = std::bind( [this](float x)
    { gravity.y = x;
      for( auto& tank : wvTankVec ) tank.wave.grav_N = x;
      for( auto& pPipe : pPipeVec ) pPipe->grav_N = x; }, std::placeholders::_1 );
  //  gravityStrip.init( 50, 650, 150, 22, label, -0.05, 1.0, 0.4, p_SetFunc );
    gravityStrip.init( 300, 30, 150, 22, label, -0.05, 1.0, 0.4, p_SetFunc );
    button::RegisteredButtVec.push_back( &gravityStrip );

    for( auto& x : pSegVec )
    {
        lineSegConveyor* pLSconvey = dynamic_cast<lineSegConveyor*>(x);
        if( pLSconvey )
        {
            vec2d fPos = pLSconvey->pos + 0.3f*pLSconvey->L;
            fSpotVec.push_back( floatSpot() );
            fSpotVec.back().init( sf::Vector2f( fPos.x, fPos.y ), sf::Vector2f( 15.0f, -8.0f ), 6.0f, "conveyor", 14, sf::Color::Cyan, sf::Color::White, sf::Color::Black );
            fSpotVec.back().init2( -5.0f, 5.0f, 2.0f, "speed", &(pLSconvey->vel) );
            fSpotVec.back().inDefView = false;
        }
    }

    if( wvTankVec.size() > 0 )// make wave controls
    {
        wvTankControlVec.reserve( wvTankVec.size() );
        bool is_s = true;
        for( auto& wt : wvTankVec )
        {
            wvTankControlVec.push_back( buttonList() );
            ctUtil::init_waveControl( wvTankControlVec.back(), wt.wave, wt.pos.x, wt.pos.y, ( is_s ? 'c' : 's' ) );
            wvTankControlVec.back().inDefView = false;
            button::RegisteredButtVec.push_back( &( wvTankControlVec.back() ) );
            is_s = false;
        }

        if( wvTankVec.size() > 1 )
        {
            float grav = wvTankVec[0].wave.grav_N;// for convenience subst
            float fluidDensity = wvTankVec[0].wave.fluidDensity;// not varying among tanks
            sf::Color h2oColor = wvTankVec[0].wave.underColor;
         //   float diam1 = 70.0f, fluidVel = 3.5f;
            // pipe connecting tanks 1 and 2
            pipeCap* pCap1 = new pipeCap( &(wvTankVec[0].sideRt), 160.0f, diam1, fluidVel, grav, fluidDensity );
            pipeCap* pCap2 = new pipeCap( &(wvTankVec[1].sideLt), 100.0f, diam1, -fluidVel, grav, fluidDensity );
            wvTankVec[0].vDrift = (pCap1->fluidVel*pCap1->diam/wvTankVec[0].wave.Depth)*pCap1->Tu;
            pCap2->Tu *= -1.0f;
            wvTankVec[1].vDrift = (pCap2->fluidVel*pCap2->diam/wvTankVec[1].wave.Depth)*pCap2->Tu;
            pPipeVec.push_back( pCap1 );
            pPipeVec.push_back( pCap2 );
            vec2d Tcl = pCap2->Pctr - pCap1->Pctr; Tcl /= Tcl.mag();
         //   pCap2->Tu *= -1.0f;
            float diam2 = pCap1->diam/pCap1->Tu.dot(Tcl);
            pipeStraight* pPS1 = new pipeStraight( pCap1->Pctr, pCap2->Pctr, pCap1->Tu, pCap2->Tu, -diam2, fluidVel, grav, fluidDensity, h2oColor );
            pPipeVec.push_back( pPS1 );
            // control fluid speed
            fSpotVec.push_back( floatSpot() );
            sf::Vector2f pos0_v2f( ((pCap1->Pctr + pCap2->Pctr)/2.0f).x, ((pCap1->Pctr + pCap2->Pctr)/2.0f).y );
            fSpotVec.back().init( pos0_v2f, sf::Vector2f( 15.0f, -8.0f ), 6.0f, "pipe", 14, sf::Color::Cyan, sf::Color::White, sf::Color::Black );
            std::function<void(float)> p_SetFunc = std::bind( [pCap1,pCap2,pPS1,this](float x)
             {
                float dVel = x - pCap1->fluidVel;
                pPS1->fluidVel = x; pCap1->fluidVel = x; pCap2->fluidVel = -x;
                this->wvTankVec[0].vDrift += (dVel*pCap1->diam/this->wvTankVec[0].wave.Depth)*pCap1->Tu;
                this->wvTankVec[1].vDrift -= (dVel*pCap2->diam/this->wvTankVec[1].wave.Depth)*pCap2->Tu;
             }, std::placeholders::_1 );
          //  fSpotVec.back().init2( -5.0f, 5.0f, fluidVel, "speed", std::bind( [pCap1,pCap2,pPS1](float x) { pPS1->fluidVel = x; pCap1->fluidVel = x; pCap2->fluidVel = -x; }, std::placeholders::_1 ) );
            fSpotVec.back().init2( -5.0f, 5.0f, fluidVel, "speed", p_SetFunc );
         //   fSpotVec.back().voh.pSetFunc = std::bind( [pCap1,pCap2,pPS](float x) { pPS->fluidVel = x; pCap1->fluidVel = x; pCap2->fluidVel = -x; }, std::placeholders::_1 );
        //    fSpotVec.back().voh.pSetFunc = std::bind( [](float x) { std::cout << "\n x = " << x; } );
            fSpotVec.back().inDefView = false;

            // pipe connecting tanks 2 and 3
            if( wvTankVec.size() > 2 )
            {
                fluidVel *= -1.0f;
                pCap1 = new pipeCap( &(wvTankVec[0].bottom), 70.0f, diam1, fluidVel, grav, fluidDensity );
                pCap2 = new pipeCap( &(wvTankVec[2].sideLt), 70.0f, diam1, -fluidVel, grav, fluidDensity );
                pCap2->Tu *= -1.0f;
                pCap1->Rmax = 200.0f;
                wvTankVec[0].vDrift += (pCap1->fluidVel*pCap1->diam/wvTankVec[0].width)*pCap1->Tu;// 1st contribution above
                wvTankVec[2].vDrift = (pCap2->fluidVel*pCap2->diam/wvTankVec[2].wave.Depth)*pCap2->Tu;// 1st contribution here

                pPipeVec.push_back( pCap1 );
                pPipeVec.push_back( pCap2 );
                vec2d joinPos( pCap1->Pctr.x, pCap2->Pctr.y );
                vec2d join_Tu( (pCap2->Tu - pCap1->Tu)/sqrtf(2.0f) );
                pPS1 = new pipeStraight( pCap1->Pctr, joinPos, pCap1->Tu, join_Tu, diam1, fluidVel, grav, fluidDensity, h2oColor );
                pPipeVec.push_back( pPS1 );
                pipeStraight* pPS2 = new pipeStraight( joinPos, pCap2->Pctr, -join_Tu, pCap2->Tu, diam1, fluidVel, grav, fluidDensity, h2oColor );
                pPipeVec.push_back( pPS2 );
                fSpotVec.push_back( floatSpot() );
                sf::Vector2f pos0_v2f( ((pPS2->pos0 + pCap2->Pctr)/2.0f).x, ((pPS2->pos0 + pCap2->Pctr)/2.0f).y );
                fSpotVec.back().init( pos0_v2f, sf::Vector2f( 15.0f, -8.0f ), 6.0f, "pipe", 14, sf::Color::Cyan, sf::Color::White, sf::Color::Black );
                fSpotVec.back().inDefView = false;
                std::function<void(float)> p_SetFunc = std::bind( [pCap1,pCap2,pPS1,pPS2,this](float x)
                 {
                    float dVel = x - pCap1->fluidVel;
                    pPS1->fluidVel = x; pPS2->fluidVel = x;
                    pCap1->fluidVel = x; pCap2->fluidVel = -x;
                    this->wvTankVec[0].vDrift += (dVel*pCap1->diam/this->wvTankVec[0].width)*pCap1->Tu;
                    this->wvTankVec[2].vDrift -= (dVel*pCap2->diam/this->wvTankVec[2].wave.Depth)*pCap2->Tu;
                 }, std::placeholders::_1 );
            //    fSpotVec.back().init2( -5.0f, 5.0f, fluidVel, "speed", std::bind( [pCap1,pCap2,pPS1,pPS2](float x) { pPS1->fluidVel = x; pPS2->fluidVel = x; pCap1->fluidVel = x; pCap2->fluidVel = -x; }, std::placeholders::_1 ) );
                fSpotVec.back().init2( -5.0f, 5.0f, fluidVel, "speed", p_SetFunc );

            }

            // pipe connecting tanks 3 and 4
            if( wvTankVec.size() > 3 )
            {
                fluidVel *= -1.0f;
                pCap1 = new pipeCap( &(wvTankVec[3].bottom), 170.0f, diam1, fluidVel, grav, fluidDensity );
                pCap2 = new pipeCap( &(wvTankVec[2].sideRt), 70.0f, diam1, -fluidVel, grav, fluidDensity );
                wvTankVec[3].vDrift = (pCap1->fluidVel*pCap1->diam/wvTankVec[0].width)*pCap1->Tu;// 1st contribution here
                wvTankVec[2].vDrift += (pCap2->fluidVel*pCap2->diam/wvTankVec[2].wave.Depth)*pCap2->Tu;// 1st contribution above
            //    pCap2->Tu *= -1.0f;
                std::cout << "\n Tu2.x = " << pCap2->Tu.x;
                std::cout << "\n Tu1.y = " << pCap1->Tu.y;
                pPipeVec.push_back( pCap1 );
                pPipeVec.push_back( pCap2 );
                vec2d pos1;
                pos1.x = pCap1->Pctr.x;
                pos1.y = pCap2->Pctr.y - ( pCap1->Pctr.x - pCap2->Pctr.x );
                pipeElbow* pPE = new pipeElbow( pCap2->Pctr, pos1, pCap2->Tu, pCap1->Tu, diam1, fluidVel, grav, fluidDensity, h2oColor );
           //     pipeElbow* pPE = new pipeElbow( pos1, pCap2->Pctr, pCap1->Tu, pCap2->Tu, diam1, fluidVel, grav, fluidDensity, h2oColor );
                pPipeVec.push_back( pPE );
             //   pos1 = pCap1->Pctr + 300.0f*vec2d(0.0f,1.0f);
                pipeStraight* pPS = new pipeStraight( pCap1->Pctr, pos1, pCap1->Tu, -pCap1->Tu, diam1, fluidVel, grav, fluidDensity, h2oColor );
                pPipeVec.push_back( pPS );
                fSpotVec.push_back( floatSpot() );
                sf::Vector2f pos0_v2f( ((pPS->pos0 + pPS->pos1)/2.0f).x, ((pPS->pos0 + pPS->pos1)/2.0f).y );
                fSpotVec.back().init( pos0_v2f, sf::Vector2f( 15.0f, -8.0f ), 6.0f, "pipe", 14, sf::Color::Cyan, sf::Color::White, sf::Color::Black );
                fSpotVec.back().inDefView = false;
                std::function<void(float)> p_SetFunc = std::bind( [pCap1,pCap2,pPS,pPE,this](float x)
                 {
                    float dVel = x - pCap1->fluidVel;
                    pPS->fluidVel = x; pPE->fluidVel = x;
                    pCap1->fluidVel = x; pCap2->fluidVel = -x;
                    this->wvTankVec[3].vDrift += (dVel*pCap1->diam/this->wvTankVec[0].width)*pCap1->Tu;
                    this->wvTankVec[2].vDrift -= (dVel*pCap2->diam/this->wvTankVec[2].wave.Depth)*pCap2->Tu;
                 }, std::placeholders::_1 );
              //  fSpotVec.back().init2( -5.0f, 5.0f, fluidVel, "speed", std::bind( [pCap1,pCap2,pPS,pPE](float x) { pPS->fluidVel = x; pPE->fluidVel = x; pCap1->fluidVel = x; pCap2->fluidVel = -x; }, std::placeholders::_1 ) );
                fSpotVec.back().init2( -5.0f, 5.0f, fluidVel, "speed", p_SetFunc );

            }

         /*   pipe1.init( &(wvTankVec[0].sideRt), 130.0f, &(wvTankVec[1].sideLt), 100.0f, 70.0f, 5.0f, wvTankVec[0].wave.grav_N, wvTankVec[0].wave.fluidDensity, wvTankVec[0].wave.underColor );
            vec2d fPos = pipe1.side[0].pos + 0.3f*pipe1.side[0].L;
            fSpotVec.push_back( floatSpot() );
            fSpotVec.back().init( sf::Vector2f( fPos.x, fPos.y ), sf::Vector2f( 15.0f, -8.0f ), 6.0f, "pipe", 14, sf::Color::Cyan, sf::Color::White, sf::Color::Black );
            fSpotVec.back().init2( -5.0f, 5.0f, 2.0f, "speed", &(pipe1.fluidVel) );
            fSpotVec.back().inDefView = false;  */
        }

    }

    for( auto& x : fSpotVec ) button::RegisteredButtVec.push_back( &x );

    if( pGun_1 )// make some bullets to use
    {
    //    pGun_1->ppBulletVec = nullptr;//&pBulletVec;
        pGun_1->p_FireFunc = [this](vec2d pos, vec2d dir, float speed) { return fireShot(pos,dir,speed); };
        if( pMHfling )
        {
            float dx = 2.0f + 2.0f*pMHfling->project( vec2d(1.0f,0.0f) );
            for( size_t i=0; i< 30; ++i )
            {
                pMHfling->pos.x += dx;
                pBulletVec.push_back( pMHfling->clone() );
            }
        }

        ctUtil::init_gunControl( gunControl, *pGun_1, 30.0f, 30.0f );
        button::RegisteredButtVec.push_back( &gunControl );

        // an image
        gun::pTxtr = new sf::Texture;
        if( gun::pTxtr->loadFromFile( "include/images/guns/assaultRifle2.png" ) )
        {
            std::cout << "\n gun::Txtr loaded";
            float szx = (float)gun::pTxtr->getSize().x, szy = (float)gun::pTxtr->getSize().y;
            pGun_1->vtxVec[0].texCoords.x = szx-1.0f;     pGun_1->vtxVec[0].texCoords.y = 1.0f;// up lt
            pGun_1->vtxVec[1].texCoords.x = 1.0f; pGun_1->vtxVec[1].texCoords.y = 1.0f;// up rt
            pGun_1->vtxVec[2].texCoords.x = 1.0f; pGun_1->vtxVec[2].texCoords.y = szy-1.0f;// dn rt
            pGun_1->vtxVec[3].texCoords.x = szx-1.0f;     pGun_1->vtxVec[3].texCoords.y = szy-1.0f;// dn lt
            for( auto& vtx : pGun_1->vtxVec ) vtx.color = sf::Color::White;
        }
        else
        {
            delete gun::pTxtr;
            gun::pTxtr = nullptr;
        }
    }

    if( mapW > winW && mapH > winH )// double scroll bars!
    {
        mvHit::wdwW = mapW;
        mvHit::wdwH = mapH;
        Level::usingMapView = true;

         // slideBars for view control
        label.setString("horiz");
        float buttDim = ( winW-26.0f )*winW/mapW;

    //    viewX.init( 10.0f, winH-35.0f, winW-20.0f, 30, label, winW/2.0f, mapW+35.0f - winW/2.0f, winW/2.0f+viewOfstX, nullptr, buttDim );
        viewX.init( 3.0f, winH-22.0f, winW-6.0f, 20, label, winW/2.0f, mapW+22.0f - winW/2.0f, winW/2.0f+viewOfstX, nullptr, buttDim );
        viewX.showNums = false;
    //    viewX.grabFactor = 5.0f;
     //   sf::View* pView = &mapView;
        viewX.pSetFunc = std::bind( [](float x) { mapView.setCenter( x, mapView.getCenter().y ); viewOfstX = x - winW/2.0f; }, std::placeholders::_1 );
        button::RegisteredButtVec.push_back( &viewX );
        label.setString("");
        buttDim = ( winH-56.0f )*winH/mapH;

   //     viewY.init( winW-35.0f, 10.0f, 30, winH-50.0f, label, winH/2.0f, mapH+35.0f - winH/2.0f, winH/2.0f+viewOfstY, nullptr, buttDim );
        viewY.init( winW-22.0f, 3.0f, 20, winH-25.0f, label, winH/2.0f, mapH+22.0f - winH/2.0f, winH/2.0f+viewOfstY, nullptr, buttDim );
        viewY.showNums = false;
    //    viewY.grabFactor = 5.0f;
        viewY.pSetFunc = std::bind( [](float y) { mapView.setCenter( mapView.getCenter().x, y ); viewOfstY = y - winH/2.0f; }, std::placeholders::_1 );
        button::RegisteredButtVec.push_back( &viewY );
        mapView.setCenter( winW/2.0f+viewOfstX, winH/2.0f+viewOfstY );
    }

    // the recorder
     rec::init( recorderFrames, saveFreq, std::bind( lvl1::init_rec, this, std::placeholders::_1), &recorder_on );
     ctUtil::init_recorderControl( recorderControl, 1150, 30.0f, -40.0f, 20.0f );
     button::RegisteredButtVec.push_back( &recorderControl );

     // goto main menu button
  //   Level::pCurrLvl = this;
    Level::goto_MMButt.setPosition( {600.0f,600.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );

    return true;
}

bool lvl1::handleEvent( sf::Event& rEvent )// mouse input launches rB
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        sf::Keyboard::Key K = rEvent.key.code;
        if ( K == sf::Keyboard::F3 )// fling next mvHit in container
        {
            if( pMvHitVec.size() > 0 )
            {
                launchIdx = (1 + launchIdx )%pMvHitVec.size();
                pMHfling = pMvHitVec[launchIdx];
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
                pMHfling = pMvHitVec[launchIdx];
                break;
            }
        }
    //    else if ( K == sf::Keyboard::F5 ) pMvHitVec[launchIdx]->is_free = !pMvHitVec[launchIdx]->is_free;// toggle value
        else if ( K == sf::Keyboard::F5 ){ if( pMHfling ) pMHfling->is_free = !pMHfling->is_free; }// toggle value
        else if ( K == sf::Keyboard::Z ) { if( pXpgA ) pXpgA->Transform(); }
    }
    else if (rEvent.type == sf::Event::MouseWheelScrolled)
    {
        float dR = 2.0f*(float) rEvent.mouseWheelScroll.delta;
        if( pXpgA ) pXpgA->set_rFull( pXpgA->rFull + dR );
    }

    return true;
}

void lvl1::update( float dt )// hit test buttons and write to pSelNum
{

    if( gravity_on ) for( auto& pMH : pMvHitVec ) pMH->update( dt, gravity );
    else for( auto& pMH : pMvHitVec ) pMH->update(dt);

     if( gravity_on ) for( auto& pMH : pBulletVec ) pMH->update( dt, gravity );
    else for( auto& pMH : pBulletVec ) pMH->update(dt);

    for( auto& pSeg : pSegVec ) pSeg->update(dt);
    for( auto& tank : wvTankVec ) tank.update(dt);
 //   for( auto& tank : wvTankVec ){  tank.wave.grav_N = gravity.y; tank.update(); }
 //   for( auto& pPipe : pPipeVec ) pPipe->grav_N = gravity.y;

     // collision testing
    for( size_t i=0; i+1< pMvHitVec.size(); ++i )// mvHits vs mvHits
    for( size_t j=i+1; j< pMvHitVec.size(); ++j )
        pMvHitVec[i]->hit( *pMvHitVec[j] );

    for( size_t i=0; i+1< pBulletVec.size(); ++i )// bullets vs bullets
    for( size_t j=i+1; j< pBulletVec.size(); ++j )
        pBulletVec[i]->hit( *pBulletVec[j] );

     // bullets
    for( auto& pMH : pMvHitVec )// mvHits vs bullets
    for( auto& pBt : pBulletVec )
        pMH->hit( *pBt );

    for( auto& tank : wvTankVec )
    {
        for( auto& pMH : pMvHitVec ) tank.hit( *pMH );// normal mvHits
        for( auto& pBt : pBulletVec ) tank.hit( *pBt );// vs bullets
    }

    // pipe
    for( auto& x : pPipeVec )
    {
        for( auto& pMH : pMvHitVec ) x->hit( *pMH );// normal mvHits
        for( auto& pBt : pBulletVec ) x->hit( *pBt );// vs bullets
    }

    for( auto& pSeg : pSegVec )// segs vs bullets
    for( auto& pBt : pBulletVec )
        pSeg->hit( *pBt );

     for( auto& pSeg : pSegVec )// segs vs mvHits
    for( auto& pMH : pMvHitVec )
        pSeg->hit( *pMH );

    return;
}

void lvl1::draw_map( sf::RenderTarget& RT ) const
{
    for( auto& x : pSegVec ) x->draw(RT);
    for( auto& tank : wvTankVec ) tank.draw(RT);

    if( wvTankVec.size() > 1 ) for( auto& x : pPipeVec ) x->draw(RT);
    for( auto& pMH : pMvHitVec ) pMH->draw(RT);
    for( auto& pMH : pBulletVec ) pMH->draw(RT);
}

void lvl1::draw( sf::RenderTarget& RT ) const
{
    (void)RT;
    return;
}

void lvl1::cleanup()
{
    if( pBulletVec.size() > 0 ) { for( auto& x : pBulletVec ) delete x; pBulletVec.clear(); }
    for( auto& x : pPipeVec ) delete x;// cleanup
    pPipeVec.clear();

    if( gun::pTxtr )// see init() and pGun
    {
        delete gun::pTxtr;
        gun::pTxtr = nullptr;
        std::cout << "\nlvl1 cleanup: gun::Txtr deleted";
    }

    rec::cleanup();
    return;
}

void lvl1::init_rec( std::vector<state_ab*>& p_stateVec )
{
    size_t resSz = pMvHitVec.size() + pSegVec.size();
 //   if( pGun_1 && pGun_1->ppBulletVec ) resSz += pGun_1->ppBulletVec->size();
    if( pGun_1 ) resSz += pBulletVec.size();
    if( wvTankVec.size() > 0 ) resSz += wvTankVec.size();
    p_stateVec.reserve( resSz );

    state_ab* pSt = nullptr;
    for( auto& mh : pMvHitVec )// regular mvHits
    {
        pSt = mh->newState();
        if( pSt ) p_stateVec.push_back( pSt );
    }

 //   if( pGun_1 && pGun_1->ppBulletVec )// bullets
    if( pGun_1 )// bullets
    {
    //    for( auto& mh : *(pGun_1->ppBulletVec) )// bullets
        for( auto& mh : pBulletVec )// bullets
        {
            pSt = mh->newState();
            if( pSt ) p_stateVec.push_back( pSt );
        }
    }


    for( auto& sh : pSegVec )// segHits
    {
        pSt = sh->newState();
        if( pSt ) p_stateVec.push_back( pSt );
    }

    for( auto& tank : wvTankVec )
    {
        pSt = tank.wave.newState();
        if( pSt ) p_stateVec.push_back( pSt );
    }

    // add control states
    int NbuttStates = 0;
    for( auto& pButt : button::RegisteredButtVec )
     NbuttStates += ctUtil::addButtonState( *pButt, p_stateVec );
     std::cout << "added " << NbuttStates << " button states\n";

    // record a slideBar position
 /*   if( pathControl.pButtVec.size() > 2 )
    {
        slideBar* pSB = dynamic_cast<slideBar*>( pathControl.pButtVec[0] );
        if( pSB ) p_stateVec.push_back( new state_f( &(pSB->fVal), std::bind( slideBar::set_fVal, pSB, std::placeholders::_1) ) );
        buttonValOnHit* pVoH = dynamic_cast<buttonValOnHit*>( pathControl.pButtVec[1] );
        if( pVoH ) p_stateVec.push_back( new state_f( &(pVoH->fVal), std::bind( buttonValOnHit::set_fVal, pVoH, std::placeholders::_1) ) );
        pVoH = dynamic_cast<buttonValOnHit*>( pathControl.pButtVec[2] );
        if( pVoH ) p_stateVec.push_back( new state_f( &(pVoH->fVal), std::bind( buttonValOnHit::set_fVal, pVoH, std::placeholders::_1) ) );
    }

    // record tank controls
    for( buttonList& buttList : wvTankControlVec )
    {
        for( auto& pButt : buttList.pButtVec )
        {
            slideBar* pSB = dynamic_cast<slideBar*>( pButt );
            if( pSB )
            {
                p_stateVec.push_back( new state_f( &(pSB->fVal), std::bind( slideBar::set_fVal, pSB, std::placeholders::_1) ) );
            }
            else
            {
                buttonValOnHit* pVoH = dynamic_cast<buttonValOnHit*>( pButt );
                if( pVoH ) p_stateVec.push_back( new state_f( &(pVoH->fVal), std::bind( buttonValOnHit::set_fVal, pVoH, std::placeholders::_1) ) );
            }
        }
    }

    // the view scrolloers
    p_stateVec.push_back( new state_f( &(viewX.fVal), std::bind( slideBar::set_fVal, &viewX, std::placeholders::_1) ) );
    p_stateVec.push_back( new state_f( &(viewY.fVal), std::bind( slideBar::set_fVal, &viewY, std::placeholders::_1) ) );    */

    return;
}

mvHit* lvl1::fireShot( vec2d pos, vec2d dir, float speed )
{
    static size_t bulletIdx = 0;

    mvHit* pMh = pBulletVec.at(bulletIdx);
    pMh->v = speed*dir;
    pMh->setPosition( pos + + pMh->project(dir)*dir );

    bulletIdx = ( 1 + bulletIdx )%pBulletVec.size();

    return pMh;
}
