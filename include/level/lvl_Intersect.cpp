#include "lvl_Intersect.h"

bool lvl_Intersect::init( )
{
   // load some mvHits
    std::ifstream fin( "include/level/lvl_Intersect_data/mvHit_data.txt" );
    if( !fin ) { std::cout << "no mvHit_data\n"; return false; }
    if( !load_mvHits( fin, pMvHitVec ) ) { std::cout << "no load_mvHits\n"; return false; }

    fin.close();

    launchIdx = 0;
    pMHfling = pMvHitVec[launchIdx];

    for( auto& x : pMvHitVec )
    {
        pXpgA = dynamic_cast<expandPolygon*>(x);
        if( pXpgA ) break;
    }// 1st will do

    sf::Text label("floor drag", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    // floor drag
    dragStrip.init( 150.0f, 680.0f, 150.0f, 20.0f, label, 0.0f, 3.0f, floorDrag, &floorDrag );
    button::RegisteredButtVec.push_back( &dragStrip );
    label.setString("hit on");
    hitButt.init( 50.0f, 680.0f, 50.0f, 25.0f, label );
    button::RegisteredButtVec.push_back( &hitButt );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );

    isectMsg.setFont( *button::pFont );
    isectMsg.setCharacterSize( 20 );
    isectMsg.setColor( sf::Color::Red );
    isectMsg.setPosition( sf::Vector2f( 400.0f, 20.0f ) );
    isectMsg.setString( "intersect :" );

    isectIdx1Msg = isectIdx2Msg = isectMsg;
    isectIdx1Msg.setPosition( sf::Vector2f( 500.0f, 20.0f ) );
    isectIdx1Msg.setString( "" );
    isectIdx2Msg.setPosition( sf::Vector2f( 550.0f, 20.0f ) );
    isectIdx2Msg.setString( "" );

 /*   label.setString("GO");
    goButt.init( 800.0f, 680.0f, 50.0f, 25.0f, label );
    goButt.pHitFunc = [this](){ if( pMHfling ){ flingPos0 = pMHfling->pos; kTime = 0.0f; } };
    button::RegisteredButtVec.push_back( &goButt );
    label.setString("da//dt");
    kStrip.init( 870.0f, 640.0f, 150.0f, 20.0f, label, 1.0f, 10.0f, 5.0f, nullptr );
    button::RegisteredButtVec.push_back( &kStrip );
    label.setString("time");
    tStrip.init( 870.0f, 680.0f, 150.0f, 20.0f, label, 2.0f, 10.0f, 5.0f, nullptr );
    button::RegisteredButtVec.push_back( &tStrip ); */

    float legLen = 200.0f;// fin >> legLen;
    float X0 = 500.0f, Y0 = 500.0f;
    maLeg[0].init( X0, Y0, X0+legLen, Y0 );
    float tL = 20.0f;
    float v0 = 0.1f, vf = 10.0f;
    ma[0].init( maLeg[0], v0, vf, tL );
    maLeg[1].init( maLeg[0], X0+2.0f*legLen, Y0 );
    ma[1].init( maLeg[1], vf, v0, tL );
    ma[0].next = ma+1;

    label.setString("GO_2");
    goButt_2.init( 400.0f, 680.0f, 50.0f, 25.0f, label );
    goButt_2.pHitFunc = [this]()
    {
        if( pMHfling )
        {
            std::ifstream fin2( "include/level/lvl_Intersect_data/mvAdapt_data.txt" );
            if( !fin2 ) { std::cout << "no mvAdapt_data\n"; return false; }
            float LL; fin2 >> LL;
            maLeg[0].init( pMHfling->pos.x, pMHfling->pos.y, pMHfling->pos.x+LL, pMHfling->pos.y );
            float v_0, v_1, v_2, t_L;
            fin2 >> v_0 >> v_1 >> v_2 >> t_L;
            fin2.close();
            ma[0].init( maLeg[0], v_0, v_1, t_L );
            maLeg[1].init( maLeg[0], pMHfling->pos.x+2.0f*LL, pMHfling->pos.y );
            ma[1].init( maLeg[1], v_1, v_2, t_L );

            ma[0].tElap = ma[1].tElap = 0.0f;
            pMA = ma;
        }
    };
    button::RegisteredButtVec.push_back( &goButt_2 );

 /*   ma[0].init( maLeg[0], v0, vf, tL );
    ma[0].next = nullptr;
 //   ma[1].prev = ma;

    label.setString("GO_2");
    goButt_2.init( 400.0f, 680.0f, 50.0f, 25.0f, label );
    goButt_2.pHitFunc = [this]()
    {
        if( pMHfling )
        {
            std::ifstream fin2( "include/level/lvl_Intersect_data/mvAdapt_data.txt" );
            if( !fin2 ) { std::cout << "no mvAdapt_data\n"; return false; }
            float LL; fin2 >> LL;
            maLeg[0].init( pMHfling->pos.x, pMHfling->pos.y, pMHfling->pos.x+LL, pMHfling->pos.y );
            float v_0, v_1, v_2, t_L;
            fin2 >> v_0 >> v_1 >> v_2 >> t_L;
            fin2.close();
            ma[0].init( maLeg[0], v_0, v_2, t_L );
            ma[0].tElap = 0.0f;
            ma[0].next = nullptr;
            pMA = ma;
        }
    };
    button::RegisteredButtVec.push_back( &goButt_2 );   */

    label.setString("SPIN");
    spinButt.init( 480.0f, 680.0f, 50.0f, 25.0f, label );
    spinButt.pHitFunc = [this]()
    {
        if( pMHfling )
        {
            std::ifstream fin2( "include/level/lvl_Intersect_data/mvAdaptSpin_data.txt" );
            if( !fin2 ) { std::cout << "no mvAdaptSpin_data\n"; return false; }
            float LL; fin2 >> LL;
            maLeg[0].init( pMHfling->pos.x, pMHfling->pos.y, pMHfling->pos.x+LL, pMHfling->pos.y );
            float v_0, v_1, v_2, t_L;
            fin2 >> v_0 >> v_1 >> v_2 >> t_L;
            fin2.close();
            ma[0].init( maLeg[0], v_0, v_1, t_L );
            maLeg[1].init( maLeg[0], pMHfling->pos.x+2.0f*LL, pMHfling->pos.y );
            ma[1].init( maLeg[1], v_1, v_2, t_L );

            ma[0].tElap = ma[1].tElap = 0.0f;
            ma[0].next = ma+1;
            pMA = ma;
        }
    };
    button::RegisteredButtVec.push_back( &spinButt );

    return true;
}

bool lvl_Intersect::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        sf::Keyboard::Key K = rEvent.key.code;
        if ( K == sf::Keyboard::F3 )// fling next mvHit in container
        {
            if( pMvHitVec.size() > 0 )
            {
                launchIdx = (1 + launchIdx )%pMvHitVec.size();
             //   selectMH.setIndex(launchIdx);
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

        else if ( K == sf::Keyboard::F5 ){ if( pMHfling ) pMHfling->is_free = !pMHfling->is_free; }// toggle value
        else if ( K == sf::Keyboard::F6 ){ for( auto& pMh : pMvHitVec ) pMh->v *= 0.0f; }// freeze all
        else if ( K == sf::Keyboard::F7 ){ doClone = true; }// clone pMHfling
        else if ( K == sf::Keyboard::F8 ){ doErase = true; }// clone pMHfling
        else if ( K == sf::Keyboard::Z ) { if( pXpgA ) pXpgA->Transform(); }
    //    else if ( K == sf::Keyboard::H ) { showMsgs = !showMsgs; }
        else if ( K == sf::Keyboard::R )// rotate current poly
        {
            if( pMHfling ) pMHfling->setRotation( pMHfling->getRotation() + 0.05f );
        //    {
         //       regPolygon* pPoly = dynamic_cast<regPolygon*>( pMHfling );
         //       if( pPoly ) pPoly->setRotation( pPoly->getRotation() + 0.05f );
         //   }
        }
    }
    else if (rEvent.type == sf::Event::MouseWheelScrolled)
    {
        float dR = 2.0f*(float) rEvent.mouseWheelScroll.delta;
        if( pXpgA ) pXpgA->set_rFull( pXpgA->rFull + dR );
    }

    return true;
}

void lvl_Intersect::update( float dt )
{
    for( auto& pMH : pMvHitVec )
    {
    //    if( pMH == pMHfling && goButt.sel ){ kMove(dt); continue; }// special motion for flinger
        if( pMH == pMHfling )
        {
        //    if( goButt.sel ) { kMove(dt);  continue; }
        //    else if( goButt_2.sel ) { maMove(dt);  continue; }
            if( goButt_2.sel ) { maMove(dt);  continue; }
            if( spinButt.sel ) { maSpin(dt);  continue; }
        }// special motion for flinger

        float magVel = pMH->v.mag();
        if( magVel > dt*floorDrag )
            pMH->v -= dt*floorDrag*pMH->v/magVel;
        else// just stop
            pMH->v *= 0.0f;

        pMH->update(dt);
    }

    size_t NmvHits = pMvHitVec.size();
    static std::vector<bool> isectVec( NmvHits*(NmvHits-1)/2, false );
    // cloning?
    if( doClone || doErase )
    {
        if( doClone )
        {
            if( pMHfling ) resizeHistory( isectVec, pMHfling->clone() );
            doClone = false;
        }
        else
        {
            resizeHistory( isectVec, pMHfling );
            if( pMHfling == pXpgA) pXpgA = nullptr;
            pMHfling = nullptr;
            doErase = false;
        }
        NmvHits = pMvHitVec.size();
    }

    std::vector<bool>::iterator it = isectVec.begin();
    anyIntersecting = false;
    for( size_t i=0; i+1< pMvHitVec.size(); ++i )// mvHits vs mvHits
    {
        for( size_t j=i+1; j< pMvHitVec.size(); ++j )
        {
            bool isectNow = pMvHitVec[i]->intersect( *pMvHitVec[j] );
            if( isectNow != *( it+j-(i+1) ) )// intersaect begin/end event
            {
                *( it+j-(i+1) ) = isectNow;
                if( isectNow )
                {
                    std::cout << "\nIsect begins for: " << i << ", " << j;
                    to_SF_string( isectIdx1Msg, i );
                    to_SF_string( isectIdx2Msg, j );
                }
                else std::cout << "\nIsect ends for: " << i << ", " << j;
            }
            if( hitButt.sel ) pMvHitVec[i]->hit( *pMvHitVec[j] );
            if( isectNow ) anyIntersecting = true;
        }
        it += NmvHits - (i+1);// next set
    }

    return;
}

/*
void lvl_Intersect::kMove( float dt )
{
    if( !goButt.sel ) return;
    if( !pMHfling ){ goButt.setSel(false); return; }

    kTime += dt;
    float t1 = tStrip.fVal;
    std::cout << "\nkTime " << kTime;
    if( kTime > 2.0f*t1 )
    {
        goButt.setSel(false);
        std::cout << "\nkMove end";
        return;
    }

    vec2d mhPos = pMHfling->pos;
    float k = kStrip.fVal;
    // wothless code removed
 //   mhPos.x = flingPos0.x + k*kTime*kTime*kTime;
  //  pMHfling->v.x = ( mhPos.x - pMHfling->pos.x )/(50.0f*dt);
  //  pMHfling->v.x = 3.0f*k*kTime*kTime/50.0f;

    mhPos.x = flingPos0.x + k*( kTime*t1*t1 - t1*t1*t1/3.0f - (kTime-t1)*(kTime-t1)*(kTime-t1)/3.0f )/2.0f;
    pMHfling->v.x = k*( t1*t1-(kTime-t1)*(kTime-t1) )/2.0f;
    mhPos.y += pMHfling->v.y*dt;

    pMHfling->setPosition( mhPos );
}   */

void lvl_Intersect::maMove( float dt )
{
    if( !goButt_2.sel ) return;
    if( !pMHfling ){ goButt_2.setSel(false); return; }
    if( !pMA ){ goButt_2.setSel(false); return; }

    vec2d posLast = pMHfling->pos;
    pMA = pMA->update( pMHfling->pos.x, pMHfling->pos.y, dt );
    pMHfling->setPosition( pMHfling->pos );
    pMHfling->v = ( pMHfling->pos - posLast )/dt;
}

void lvl_Intersect::maSpin( float dt )
{
    if( !spinButt.sel ) return;
    if( !pMHfling ){ spinButt.setSel(false); return; }
    if( !pMA ){ spinButt.setSel(false); return; }

    float ang;
    pMA = pMA->update( ang, dt );
    if( pMA == ma+1 && ma->pLeg ) ang += ma->pLeg->len;
    if( !pMA ) ang = 0.0f;
    pMHfling->setRotation( ang );
}

void lvl_Intersect::resizeHistory( std::vector<bool>& isVec, mvHit* pMH )
{
    if( !pMH ) return;

    // erase?

    std::vector<mvHit*>::iterator it = pMvHitVec.begin();
    for( ; it != pMvHitVec.end(); ++it )
        if( *it == pMH )// erasure
            break;

    if( it == pMvHitVec.end() )// add new mh
    {
        pMH->setPosition( pMH->pos + vec2d(0.0f,100.0f) );
        pMvHitVec.push_back(pMH);
    }
    else
    {
        delete *it;
        pMvHitVec.erase( it );
    }

    // re establish intersect states
    size_t NmvHits = pMvHitVec.size();
    std::vector<bool> isVecNew( NmvHits*(NmvHits-1)/2, false );
    // copy existing history
    isVec = isVecNew;
    // re establish values
    std::vector<bool>::iterator itb = isVec.begin();
    for( size_t i=0; i+2< pMvHitVec.size(); ++i )// skip last just added mvHit
    {
        for( size_t j=i+1; j+1< pMvHitVec.size(); ++j )
            *( itb+j-(i+1) ) = pMvHitVec[i]->intersect( *pMvHitVec[j] );

        itb += NmvHits - (i+1);// next set
    }
}

void lvl_Intersect::draw( sf::RenderTarget& RT ) const
{
    for( auto& pMH : pMvHitVec ) pMH->draw(RT);
    if( anyIntersecting )
    {
        RT.draw( isectMsg );
        RT.draw( isectIdx1Msg );
        RT.draw( isectIdx2Msg );
    }

    return;
}

void lvl_Intersect::cleanup()
{
    return;
}
