#include "makeObstacleMap.h"

bool makeObstacleMap::init( const sf::Font& font )
{
    ball::wdwCf = 0.2f;
    pullLine[0].color = pullLine[1].color = sf::Color(0,0,255);// for launching the blue ball

    // load a ball. Controlled by mouse
    std::ifstream fin("data/makeObstacleMap/data_1.txt");// blue ball data
    if( !fin ) return false;
    B1.init( fin );// the cue ball
    Bgo.init( fin );

    fin >> rollFriction >> gravity.y; gravity.x = 0.0f;

    fin >> showPoints;
    fin >> ball::wdwCf;

    loadObstacles( fin );

    fin.close();

    fin.open("data/makeObstacleMap/datum_1.txt");
    if( !fin ) return false;
    getDat[0].init( font, fin );
    fin.close();

    float dy = getDat[0].entryRect.getSize().y + 10.0f;
    for( size_t i=1; i<maxDatum; ++i )
    {
        getDat[i] = getDat[0];
        getDat[i].entryRect.move( 0.0f, (float)i*dy );
        getDat[i].stateRect.move( 0.0f, (float)i*dy );
        getDat[i].entry.move( 0.0f, (float)i*dy );
        getDat[i].title.move( 0.0f, (float)i*dy );
        getDat[i].open = true;
    }

    // slideBars!
    float* pf[] = { &Bgo.maxVel, &Bgo.maxAcc, &Cfric, &gravity.y };
    fin.open("data/makeObstacleMap/slide_data.txt");
    if( !fin ) return false;
    size_t nSliders = 0;
    fin >> nSliders;
    for( size_t i=0; i< nSliders; ++i )
        sliderVec.push_back( slideBar( &collideSim::sliderSS, font, pf[i], fin ) );
    fin.close();

    // other inits
    operMode = 'T';// testing with B1
 //   get_datum::pGD_focus = nullptr;
    get_datum::set_focus( nullptr );

    return true;
}

void makeObstacleMap::reset()
{
    B1.pos.x = 200.0f;
    B1.pos.y = 300.0f;
    B1.v.x = 0.0f;
    B1.v.y = 0.0f;

    return;
}

bool makeObstacleMap::handleEvent( sf::Event& rEvent )// mouse input launches rB
{
    static vec2d msePosUp, msePosDn;

 //   if( !collideSim::handleEvent( rEvent ) ) return false;

    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if( operMode == 'N' )
        {
            handleKey_addNew( rEvent.key.code );
        }
        else if( operMode == 'C' )
        {
            handleKey_change( rEvent.key.code );
        }
        else if( operMode == 'T' )// operMode = 'T'
        {
            handleKey_test( rEvent.key.code );
        }

        if ( rEvent.key.code == sf::Keyboard::T )
        { operMode = 'T'; B1.pos = msePosUp; B1.v.x = 0.0f; B1.v.y = 0.0f; showPoints = false; numDatum = 0; }// test segs with B1
  //      if ( rEvent.key.code == sf::Keyboard::S )// toggle use of slide bars in 'T' mode
  //      { sliders_inuse = !sliders_inuse; }
        else if ( rEvent.key.code == sf::Keyboard::N )
        { operMode = 'N'; got_ptA = false; got_ptB = false; showPoints = true; addType = 'n'; }// add new segs
        else if ( rEvent.key.code == sf::Keyboard::C )
        { operMode = 'C'; got_ptA = false; got_ptB = false; showPoints = true; }// change existing segs
        else if ( rEvent.key.code == sf::Keyboard::W )
        { write_to_file(); }
    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {

    }
    else if (rEvent.type == sf::Event::MouseMoved)
    {
        msePosUp.x = rEvent.mouseMove.x;
        msePosUp.y = rEvent.mouseMove.y;

        if( operMode == 'T' )
        {
            if( sliders_inuse ) for( slideBar& sb : sliderVec ) sb.MseOver();
            else { pullLine[1].position.x = msePosUp.x; pullLine[1].position.y = msePosUp.y; }
        }
    }
    else if (rEvent.type == sf::Event::MouseButtonPressed)// lbutt down
    {
        if (rEvent.mouseButton.button == sf::Mouse::Left)
        {
            msePosDn.x = rEvent.mouseButton.x;
            msePosDn.y = rEvent.mouseButton.y;
            lbuttHeld = true;

            if( operMode == 'T' )// B1 testing
            {

           //     B1.setPosition( msePosDn );
           //     B1.v.x = B1.v.y = 0.0f;

                if( sliders_inuse )
                    for( slideBar& sb : sliderVec ) sb.hitLtDn();
                else
                {
                    Bgo.setPosition( msePosDn );
                    Bgo.v.x = Bgo.v.y = 0.0f;
                    pullLine[0].position.x = msePosDn.x; pullLine[0].position.y = msePosDn.y;
                }
            }
            else if( operMode == 'C' )
                handleClick_change( msePosDn );// change a segment
            else if( operMode == 'N' )
                handleClick_addNew( msePosDn );// add a new seg
        }
    }
    else if (rEvent.type == sf::Event::MouseButtonReleased)// lbutt up
    {
        if (rEvent.mouseButton.button == sf::Mouse::Left)
        {
            msePosUp.x = rEvent.mouseButton.x;
            msePosUp.y = rEvent.mouseButton.y;

            lbuttHeld = false;
            if( operMode == 'T' )// B1 testing
            {
            //    B1.v = msePosUp - msePosDn;
            //    B1.v *= 0.1f;
            //    B1.v /= numReps;// scale launch speed
                if( sliders_inuse )
                    for( slideBar& sb : sliderVec ) sb.hitLtUp();
                else
                {
                    Bgo.v = msePosUp - msePosDn;
                    Bgo.v *= 0.1f;
                    Bgo.v /= numReps;// scale launch speed
                    pullLine[1].position.x = msePosUp.x; pullLine[1].position.y = msePosUp.y;
                }
            }
        }
    }

    return true;
}

void makeObstacleMap::handleClick_change( vec2d pt )
{
    if( got_ptA )
    {
        for( lineSeg* pS : pLSvec )
            if( nearPt( ptA, pS->pos, dr ) ) { pS->init( pt, pS->pos + pS->L, pS->vtx[0].color ); }
            else if( nearPt( ptA, pS->pos + pS->L, dr ) ) { pS->init( pS->pos, pt, pS->vtx[0].color ); }

        for( lineSegElevator* pS : pLSEvec )// Elevators
            if( nearPt( ptA, pS->pos, dr ) ) { pS->init( pt, pS->pos + pS->L, pS->vtx[0].color ); }
            else if( nearPt( ptA, pS->pos + pS->L, dr ) ) { pS->init( pS->pos, pt, pS->vtx[0].color ); }

        for( lineSegRotate* pS : pLSRvec )
            if( nearPt( ptA, pS->pos, dr ) ) { pS->init( pt, pS->pos + pS->L, pS->rotVel, pS->vtx[0].color ); }
            else if( nearPt( ptA, pS->pos + pS->L, dr ) ) { pS->init( pS->pos, pt, pS->rotVel, pS->vtx[0].color ); }

        for( arcSeg* pS : pASvec )
        {
            if( nearPt( ptA, pS->pos, dr ) )// new center
            {
                vec2d uDir = pS->s[0] + pS->s[1];
                uDir /= uDir.mag();// unit vector
                float disp = uDir.dot( pt - pS->pos );// displacement along uDir
                float newR = ( pS->s[0] - uDir*disp ).mag();
                pS->init( pS->pos + pS->s[0], pS->pos + pS->s[1], newR, pS->Q[0].color );
            }
            else if( nearPt( ptA, pS->pos + pS->s[0], dr ) )// new s[0]
                { pS->init(  pt, pS->pos + pS->s[1], pS->R, pS->Q[0].color ); }
            else if( nearPt( ptA, pS->pos + pS->s[1], dr ) )// new s[1]
                { pS->init( pS->pos + pS->s[0], pt, pS->R, pS->Q[0].color ); }
        }
    }
    else// get ptA
    {
        got_ptA = is_exist_pt( pt );
        if( got_ptA )// an end will be moved. No datum to be collected
        {
            pLSEcurr = nullptr;
      //      get_datum::pGD_focus = nullptr;
            get_datum::set_focus( nullptr );
            numDatum = 0;
            ptA = pt;
            return;
        }

        // maybe hit line to adjust rotVel ?
        ball B;
        B.r = dr;
        B.pos = pt;
        for( lineSegRotate* pS : pLSRvec )
            if( pS->hit(B) )
            {
                set_datum_for_Rotate( pS, false );
                return;
            }

        // Elevator datum change ?
        for( lineSegElevator* pS : pLSEvec )
            if( pS->hit(B) )
            {
                set_datum_for_Elevator( pS, false );
                return;
            }

        // all missed
        pLSEcurr = nullptr;
   //     get_datum::pGD_focus = nullptr;
        get_datum::set_focus( nullptr );
        numDatum = 0;
    }

    if( got_ptA ) got_ptA = false;
}

void makeObstacleMap::handleKey_change( sf::Keyboard::Key K )
{
//    if( numDatum > 0 && get_datum::pGD_focus )
    if( numDatum > 0 && get_datum::get_focus() )
    {
    //    if( numDatum > 0 && get_datum::pGD_focus && get_datum::pGD_focus->handleKey( K ) )
        if( K == sf::Keyboard::Tab )// next gatDat
        {
            idxDatum = ( 1 + idxDatum )%numDatum;
        //    get_datum::pGD_focus = getDat + idxDatum;
            get_datum::set_focus( getDat + idxDatum );
        }
//        else if( get_datum::pGD_focus->handleKey( K ) )// valid entry just made
//        else if( get_datum::get_focus()->handleKey( K ) )// valid entry just made
        else if( get_datum::handleKey( K ) )// valid entry just made
        {
            std::cerr << "idxDatum = " << idxDatum << '\n';
            if( pLSEcurr && idxDatum < 2 ) pLSEcurr->setVel( vec2d(getDat[0].fVal, getDat[1].fVal) );// special Elevator member dependence workaround
         //   idxDatum = ( 1 + idxDatum )%numDatum;
         //   get_datum::pGD_focus = getDat + idxDatum;
        }
    }
}

void makeObstacleMap::handleKey_test( sf::Keyboard::Key K )
{
    if( K == sf::Keyboard::E )// operate elevator
    {
        if( pLSEvec.size() > 0 )
        {
            if( pLSEvec.back()->is_dn() ) pLSEvec.back()->init_up();
            else pLSEvec.back()->init_dn();
        }
    }
    else if ( K == sf::Keyboard::S )// toggle use of slide bars in 'T' mode
        sliders_inuse = !sliders_inuse;
    else if ( K == sf::Keyboard::B && sliders_inuse && sliderVec.size() > 0 ) sliderVec[0].setVal(0.0f);// full stop
}

bool makeObstacleMap::is_exist_pt( vec2d& pt )const// writes to pt and returns true if pt within dr
{
    for( lineSeg* pS : pLSvec )
    {
        if( nearPt( pt, pS->pos, dr ) ) { pt = pS->pos; return true; }
        if( nearPt( pt, pS->pos + pS->L, dr ) ) { pt = pS->pos + pS->L; return true; }
    }

    for( lineSegRotate* pS : pLSRvec )
    {
        if( nearPt( pt, pS->pos, dr ) ) { pt = pS->pos; return true; }
        if( nearPt( pt, pS->pos + pS->L, dr ) ) { pt = pS->pos + pS->L; return true; }
    }

    for( lineSegElevator* pS : pLSEvec )
    {
        if( nearPt( pt, pS->pos, dr ) ) { pt = pS->pos; return true; }
        if( nearPt( pt, pS->pos + pS->L, dr ) ) { pt = pS->pos + pS->L; return true; }
    }

    for( arcSeg* pS : pASvec )
    {
        if( nearPt( pt, pS->pos, dr ) ) { pt = pS->pos; return true; }
        if( nearPt( pt, pS->pos + pS->s[0], dr ) ) { pt = pS->pos + pS->s[0]; return true; }
        if( nearPt( pt, pS->pos + pS->s[1], dr ) ) { pt = pS->pos + pS->s[1]; return true; }
    }

    return false;
}

void makeObstacleMap::set_datum_for_Rotate( lineSegRotate* pS, bool setDefaultVals )
{
    numDatum = 1;
    idxDatum = 0;
    if( setDefaultVals ) pS->rotVel = 0.0f;
    getDat[0].Assign( 'F', "rotVel", &( pS->rotVel ) );
    getDat[0].open = true;
//    get_datum::pGD_focus = getDat + idxDatum;
    get_datum::set_focus( getDat + idxDatum );
}

void makeObstacleMap::set_datum_for_Elevator( lineSegElevator* pS, bool setDefaultVals )
{
    numDatum = 4;
    idxDatum = 0;
    if( setDefaultVals )
    {
        pS->setVel( vec2d() );
        pS->uVel.x = pS->uVel.y = pS->sMin = pS->sMax = 0.0f;
    }
//    getDat[0].Assign( 'F', "v.x", &( pS->uVel.x ) ); getDat[0].open = true;
//    getDat[1].Assign( 'F', "v.y", &( pS->uVel.y ) ); getDat[1].open = true;
    getDat[0].fVal = pS->magVel*pS->uVel.x;
    getDat[0].Assign( 'F', "v.x", nullptr ); getDat[0].open = true;
    getDat[1].fVal = pS->magVel*pS->uVel.y;
    getDat[1].Assign( 'F', "v.y", nullptr ); getDat[1].open = true;
    getDat[2].Assign( 'F', "sMin", &( pS->sMin ) );  getDat[2].open = true;
    getDat[3].Assign( 'F', "sMax", &( pS->sMax ) );  getDat[3].open = true;

 //   get_datum::pGD_focus = getDat + idxDatum;
    get_datum::set_focus( getDat + idxDatum );
    pLSEcurr = pS;
}

void makeObstacleMap::handleClick_addNew( vec2d pt )
{
    if( addType != 'L' && addType != 'A' && addType != 'R' && addType != 'E' ) return;

    if( !got_ptA )
    {
        is_exist_pt( pt );
        ptA = pt;
        got_ptA = true;
    }
    else if( !got_ptB )
    {
        is_exist_pt( pt );

        if( addType == 'L' )// have data
        {
            pLSvec.push_back( new lineSeg );
            pLSvec.back()->init( ptA, pt, sf::Color(200,200,200) );
            pLSvec.back()->friction_on = friction_on;
            pLSvec.back()->Cf = Cfric;
            pLSvec.back()->testEnd1 = true;
            pLSvec.back()->testEnd2 = true;
            got_ptA = false;
            numDatum = 0;
            return;
        }
        else if( addType == 'R' )// have point data
        {
            pLSRvec.push_back( new lineSegRotate );
            pLSRvec.back()->init( ptA, pt, 0.0f, sf::Color(200,200,200) );
            pLSRvec.back()->testEnd1 = true;
            pLSRvec.back()->testEnd2 = true;
            got_ptA = false;
            set_datum_for_Rotate( pLSRvec.back(), true );
            return;
        }
        else if( addType == 'E' )// have point data
        {
            pLSEvec.push_back( new lineSegElevator );
            pLSEvec.back()->init( ptA, pt, sf::Color(200,200,200) );
            pLSEvec.back()->friction_on = friction_on;
            pLSEvec.back()->Cf = Cfric;
            pLSEvec.back()->testEnd1 = true;
            pLSEvec.back()->testEnd2 = true;
            got_ptA = false;
            set_datum_for_Elevator( pLSEvec.back(), true );
            return;
        }

        ptB = pt;
        got_ptB = true;
    }
    else if( addType == 'A' )// arcSeg - 3rd point required to find new radius
    {
        vec2d C = ( ptA + ptB )/2.0f;
        vec2d uDir = ( ptB - ptA ).get_LH_norm();
        uDir /= uDir.mag();// unit vector
        float disp = uDir.dot( pt - C );// displacement along uDir

        if( disp < 0.0f )// ptA, ptB in wrong order
        {
            disp *= -1.0f;
            uDir *= -1.0f;
            vec2d temp = ptA;
            ptA = ptB;
            ptB = temp;
        }

        vec2d newPos = C + uDir*disp;
        float newR = ( ptB - newPos ).mag();
        pASvec.push_back( new arcSeg );
        pASvec.back()->init( ptA, ptB, newR, sf::Color(100,200,100) );
        pASvec.back()->friction_on = friction_on;
        pASvec.back()->Cf = Cfric;
        pASvec.back()->testEnd1 = true;
        pASvec.back()->testEnd2 = true;
        numDatum = 0;
        got_ptA = false;
        got_ptB = false;
    }

    return;
}

void makeObstacleMap::handleKey_addNew( sf::Keyboard::Key K )
{
    if( K == sf::Keyboard::A )// new arcSeg
    {
        std::cerr << "A\n";
        addType = 'A';
        numDatum = 0;
   //     get_datum::pGD_focus = nullptr;
        get_datum::set_focus( nullptr );
        pLSEcurr = nullptr;
        got_ptA = got_ptB = false;
    }
    else if( K == sf::Keyboard::L )// new lineSeg
    {
        std::cerr << "L\n";
        addType = 'L';
        numDatum = 0;
  //      get_datum::pGD_focus = nullptr;
        get_datum::set_focus( nullptr );
        pLSEcurr = nullptr;
        got_ptA = got_ptB = false;
    }
    else if( K == sf::Keyboard::R )// new lineSegRotate
    {
        std::cerr << "R\n";
        addType = 'R';
   //     getDat[0].title.setString( "rotVel" );
        numDatum = 0;//1;
        pLSEcurr = nullptr;
        got_ptA = got_ptB = false;
    }
    else if( K == sf::Keyboard::E )// new lineSegElevator
    {
        std::cerr << "E\n";
        addType = 'E';
        numDatum = 0;// will = 4 post creation
        pLSEcurr = nullptr;
        got_ptA = got_ptB = false;
    }
//    else if( numDatum > 0 && get_datum::pGD_focus )
    else if( numDatum > 0 && get_datum::get_focus() )
    {
        if( K == sf::Keyboard::Tab )// next gatDat
        {
            idxDatum = ( 1 + idxDatum )%numDatum;
        //    get_datum::pGD_focus = getDat + idxDatum;
            get_datum::set_focus( getDat + idxDatum );
        }
//        else if( get_datum::pGD_focus->handleKey( K ) )// valid entry just made
//        else if( get_datum::get_focus()->handleKey( K ) )// valid entry just made
        else if( get_datum::handleKey( K ) )// valid entry just made
        {
            std::cerr << "idxDatum = " << idxDatum << "  pLSEcurr = " << pLSEcurr << '\n';
            if( pLSEcurr && idxDatum < 2 ) pLSEcurr->setVel( vec2d(getDat[0].fVal, getDat[1].fVal) );// special Elevator member dependence workaround
         //   idxDatum = ( 1 + idxDatum )%numDatum;
         //   get_datum::pGD_focus = getDat + idxDatum;
        }
    }
}

void makeObstacleMap::logic()
{
    if( operMode == 'T' )
    {
        for( size_t j=0; j<numReps; ++j)
        {
//            if( !lbuttHeld ) B1.update( gravity, rollFriction );// blue ball
            if( sliders_inuse || !lbuttHeld ) Bgo.update( gravity );// blue ball

            if( friction_on && sliders_inuse != sliders_inuse_last )
            {
                // update friction coeffs
                for( lineSeg* pS : pLSvec ) { pS->Cf = Cfric; pS->friction_on = true; }
                for( lineSegRotate* pS : pLSRvec ) { pS->Cf = Cfric; pS->friction_on = true; }
                for( lineSegElevator* pS : pLSEvec ) { pS->Cf = Cfric; pS->friction_on = true; }
                for( arcSeg* pS : pASvec ) { pS->Cf = Cfric; pS->friction_on = true; }

                sliders_inuse_last = sliders_inuse;
            }

            for( lineSeg* pLS : pLSvec ) pLS->update();
            for( lineSegRotate* pLSR : pLSRvec ) pLSR->update();
            for( lineSegElevator* pS : pLSEvec ) pS->update();
            for( arcSeg* pAS : pASvec ) pAS->update();

            for( lineSeg* pS : pLSvec ) pS->hit(Bgo);
            for( lineSegRotate* pS : pLSRvec ) pS->hit(Bgo);
            for( lineSegElevator* pS : pLSEvec ) pS->hit(Bgo);
            for( arcSeg* pS : pASvec ) pS->hit(Bgo);

        //    for( lineSeg* pS : pLSvec ) pS->hit(B1);
         //   for( lineSegRotate* pS : pLSRvec ) pS->hit(B1);
        //    for( lineSegElevator* pS : pLSEvec ) pS->hit(B1);
        //    for( arcSeg* pS : pASvec ) pS->hit(B1);

        }


    }
    else if( operMode == 'N' )
    {
        for( size_t j=0; j<numReps; ++j)
        {
            for( lineSegRotate* pS : pLSRvec ) pS->update();
            for( lineSegElevator* pS : pLSEvec ) pS->update();
        }
    }

    return;
}

void makeObstacleMap::loadObstacles( std::ifstream& fin )
{
    fin >> friction_on >> Cfric;

    size_t nSegs = 0;
    fin >> nSegs;

    if( nSegs > 0 )
    {
        char type = 'n';

        for( size_t i=0; i<nSegs; ++i )
        {
            fin >> type;
            switch( type )
            {
                case 'A' :// arcSeg
                pASvec.push_back( new arcSeg(fin) );
                pASvec.back()->friction_on = friction_on;
                pASvec.back()->Cf = Cfric;
                break;

                case 'L' :// lineSeg
                pLSvec.push_back( new lineSeg(fin) );
                pLSvec.back()->friction_on = friction_on;
                pLSvec.back()->Cf = Cfric;
                break;

                case 'R' :// lineSegRotate
                pLSRvec.push_back( new lineSegRotate(fin) );
                break;

                case 'E' :// lineSegElevator
                pLSEvec.push_back( new lineSegElevator(fin) );
                pLSEvec.back()->friction_on = friction_on;
                pLSEvec.back()->Cf = Cfric;
                break;

                default:
                break;
            }
        }
    }
 //   else
 //       ppCB = nullptr;
}

void makeObstacleMap::draw( sf::RenderWindow& RW )
{
    for( lineSeg* pS : pLSvec ) pS->draw(RW);
    for( arcSeg* pS : pASvec ) pS->draw(RW);
    for( lineSegRotate* pS : pLSRvec ) pS->draw(RW);
    for( lineSegElevator* pS : pLSEvec ) pS->draw(RW);

    if( showPoints ) drawPoints(RW);

    for(size_t i=0; i<numDatum; ++i ) getDat[i].draw(RW);

    if( operMode == 'T' )
    {
        Bgo.draw(RW);// B1.draw(RW);
        if( sliders_inuse ) for( slideBar& sb : sliderVec ) sb.draw(RW);
        else if( lbuttHeld ) RW.draw(pullLine, 2, sf::Lines);
    }
}

void makeObstacleMap::drawPoints( sf::RenderWindow& RW )
{
    const sf::Color clrHit(0,255,0), clrNoHit(0,0,255), clrCtr(255,0,0);
    static sf::CircleShape img;// image
    img.setRadius(dr);
    img.setOrigin( dr,dr );

    for( lineSeg* pLS : pLSvec )
    {
        img.setPosition( pLS->pos.x, pLS->pos.y );
        img.setFillColor( pLS->testEnd1 ? clrHit : clrNoHit );
        RW.draw(img);
        img.setPosition( pLS->pos.x + pLS->L.x, pLS->pos.y + pLS->L.y );
        img.setFillColor( pLS->testEnd2 ? clrHit : clrNoHit );
        RW.draw(img);
    }

    for( lineSegRotate* pS : pLSRvec )
    {
        img.setPosition( pS->pos.x, pS->pos.y );
        img.setFillColor( pS->testEnd1 ? clrHit : clrNoHit );
        RW.draw(img);
        img.setPosition( pS->pos.x + pS->L.x, pS->pos.y + pS->L.y );
        img.setFillColor( pS->testEnd2 ? clrHit : clrNoHit );
        RW.draw(img);
    }

    for( lineSegElevator* pS : pLSEvec )
    {
        img.setPosition( pS->pos.x, pS->pos.y );
        img.setFillColor( pS->testEnd1 ? clrHit : clrNoHit );
        RW.draw(img);
        img.setPosition( pS->pos.x + pS->L.x, pS->pos.y + pS->L.y );
        img.setFillColor( pS->testEnd2 ? clrHit : clrNoHit );
        RW.draw(img);
    }

    for( arcSeg* pAS : pASvec )
    {
        img.setPosition( pAS->pos.x, pAS->pos.y );// center
        img.setFillColor( clrCtr );
        RW.draw(img);

        img.setPosition( pAS->pos.x + pAS->s[0].x, pAS->pos.y + pAS->s[0].y );
        img.setFillColor( pAS->testEnd1 ? clrHit : clrNoHit );
        RW.draw(img);

        img.setPosition( pAS->pos.x + pAS->s[1].x, pAS->pos.y + pAS->s[1].y );
        img.setFillColor( pAS->testEnd2 ? clrHit : clrNoHit );
        RW.draw(img);
    }

 /*   if( oldPt_valid )
    {
        img.setPosition( oldPt.x, oldPt.y );
        img.setFillColor( sf::Color(255,255,255) );
        RW.draw(img);
    }   */

    if( got_ptA )
    {
        img.setPosition( ptA.x, ptA.y );
        img.setFillColor( sf::Color(0,100,255) );
        RW.draw(img);
    }

    if( got_ptB )
    {
        img.setPosition( ptB.x, ptB.y );
        img.setFillColor( sf::Color(0,255,100) );
        RW.draw(img);
    }

}

void makeObstacleMap::cleanup()
{
    for( lineSeg* pS : pLSvec ) delete pS;
    for( lineSegRotate* pS : pLSRvec ) delete pS;
    for( lineSegElevator* pS : pLSEvec ) delete pS;
    for( arcSeg* pS : pASvec ) delete pS;
    pLSvec.clear();
    pLSRvec.clear();
    pASvec.clear();
}

makeObstacleMap::~makeObstacleMap()
{
    std::cout << "Bye-bye from makeObstacleMap dtor\n";
    std::cout << "deleting: " << pLSvec.size() << " lineSeg(s)\n";
    std::cout << "deleting: " << pLSRvec.size() << " lineSegRotate(s)\n";
    std::cout << "deleting: " << pLSEvec.size() << " lineSegElevator(s)\n";
    std::cout << "deleting: " << pASvec.size() << " arcSeg(s)\n";
    for( lineSeg* pS : pLSvec ) delete pS;
    for( lineSegRotate* pS : pLSRvec ) delete pS;
    for( lineSegElevator* pS : pLSEvec ) delete pS;
    for( arcSeg* pS : pASvec ) delete pS;
}

void makeObstacleMap::write_to_file()const
{
    std::ofstream fout("data/makeObstacleMap/data_out.txt");
    if( !fout ){ std::cerr << "no file write\n"; return; }

    std::cerr << "writing to file\n";

    fout << pLSvec.size() + pASvec.size() + pLSRvec.size() + pLSEvec.size();

    for( lineSeg* pS : pLSvec )
    {
        fout << '\n';
        pS->to_file(fout);
    }

    for( lineSegRotate* pS : pLSRvec )
    {
        fout << '\n';
        pS->to_file(fout);
    }

    for( lineSegElevator* pS : pLSEvec )
    {
        fout << '\n';
        pS->to_file(fout);
    }

    for( arcSeg* pS : pASvec )
    {
        fout << '\n';
        pS->to_file(fout);
    }

    return;
}

bool nearPt( vec2d a, vec2d b, float dR )
{
    vec2d s = b - a;
    return s.dot(s) < dR*dR;
}
