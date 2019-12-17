#include "test1_obj.h"

quadPack::quadPack( std::ifstream& fin, spriteSheet* p_SS ) { init( fin, p_SS );}

void quadPack::init( std::ifstream& fin, spriteSheet* p_SS )
{
    pSS = p_SS;

    size_t numLayers = 0, setNum = 0;
    float scale = 1.0f;
    fin >> scale;
    fin >> numLayers >> setNum;
    for( size_t i=0; i<numLayers; ++i )
    {
        float x0, y0; fin >> x0 >> y0;// start position
        if( i == 0 ) { pos.x = x0; pos.y = y0; }

        char dir = 'n';// direction to expand layer in: U,D,L,R
        size_t n; fin >> n >> dir;// number this layer
        float x = x0;
        sf::Vertex vt;
        for( size_t j=0; j<n; ++j )// each quad in this layer
        {
            size_t frIdx; fin >> frIdx;// area rendered
            sf::IntRect frRect = pSS->getFrRect( frIdx, setNum );

            vt.position.x = x;// up lt
            vt.position.y = y0;
            vt.texCoords.x = frRect.left;
            vt.texCoords.y = frRect.top;
            vtxVec.push_back( vt );
            vt.position.x = x + frRect.width*scale;// tp rt
            vt.position.y = y0;
            vt.texCoords.x = frRect.left + frRect.width;
            vt.texCoords.y = frRect.top;
            if(  i == 0 && j+1 == n ){ L.x = vt.position.x; L.y = vt.position.y; L -= pos; }
            vtxVec.push_back( vt );
            vt.position.x = x + frRect.width*scale;// bt rt
            vt.position.y = y0 + frRect.height*scale;
            vt.texCoords.x = frRect.left + frRect.width;
            vt.texCoords.y = frRect.top + frRect.height;
            vtxVec.push_back( vt );
            vt.position.x = x;// bt lt
            vt.position.y = y0 + frRect.height*scale;
            vt.texCoords.x = frRect.left;
            vt.texCoords.y = frRect.top + frRect.height;
            vtxVec.push_back( vt );

            if( dir == 'R' ) x += frRect.width*scale;
            else if( dir == 'D' ) y0 += frRect.height*scale;
        }
    }
    std::cerr << "quadPack.init(): L = " << L.mag() << '\n';
}

void quadPack::draw( sf::RenderWindow& rRW )
{
    rRW.draw( &(vtxVec[0]), vtxVec.size(), sf::Quads, &(pSS->img) );
}

bool test1_obj::init( const sf::Font& font )
{
    ball::wdwCf = 0.3f;

    msg1.setFont(font);
    msg1.setCharacterSize(16);
    msg1.setString("setNum: ");// = "Ek = ";
    msg1.setPosition( 100.0f, 10.0f );
    msgNum1.setFont(font);
    msgNum1.setCharacterSize(16);
    msgNum1.setString("0");// = "0.0f";
    msgNum1.setPosition( 160.0f, 10.0f );

    msg2.setFont(font);
    msg2.setCharacterSize(16);
    msg2.setString("frIdx : ");// = "Ek = ";
    msg2.setPosition( 100.0f, 30.0f );
    msgNum2.setFont(font);
    msgNum2.setCharacterSize(16);
    msgNum2.setString("0");// = "0.0f";
    msgNum2.setPosition( 160.0f, 30.0f );

    pullLine[0].color = pullLine[1].color = sf::Color(0,0,255);// for launching the blue ball

    mapView.reset(sf::FloatRect( 0.0f, 0.0f, ball::wdwW, ball::wdwH ));
    old_wdwW = ball::wdwW;
    old_wdwH = ball::wdwH;
    iPos_viewCtr.x = old_wdwW/2.0f;
    iPos_viewCtr.y = old_wdwH/2.0f;

    // the ball_go encasing platform guy
    std::ifstream fin("data/test1_obj/ball_data_1.txt");
    if( !fin ) return false;
    B1.init( fin );
    maxSpeed = B1.maxVel;
    B1.maxVel = 0.0f;
    fin >> ball::wdwCf;// extra data from file
    gravity_on = false;
    fin >> gravity_on;
    if( gravity_on ) fin >> gravity.x >> gravity.y;
    fin >> jumpSpeed >> bounceAmp;
    fin >> climbSpeed;
    numReps = 1;
    fin.close();

    fin.open( "data/test1_obj/platform_SSdata.txt" );
    if( !fin ) return false;
    platformSS.init(fin);
    fin.close();

    fin.open( "data/test1_obj/quadPack_data.txt" );
    if( !fin ) return false;
    size_t numqp; fin >> numqp;
    qpVec.reserve( numqp );
    vec2d ofst( 0.0f, 5.0f );
    for( size_t i=0; i<numqp; ++i ) qpVec.push_back( quadPack( fin, &platformSS ) );
    fin.close();

    // load some segs
    fin.open( "data/test1_obj/segData.txt" );
    if( !fin ) return false;
    if( !loadSegArray( fin, pCBvec ) )
    {
        fin.close();
        std::cerr << "seg load failed" << std::endl;
        return false;
    }
    fin.close();

    for( auto& x : pCBvec )
    {
        x->testEnd1 = true;
        x->testEnd2 = true;
        x->friction_on = true;
        x->Cf = ball::wdwCf;
    }

    ball::wdwW = worldWidth = pCBvec.back()->getExtent_x().y;
    if( worldWidth > old_wdwW ) bigView_x = true;
    else bigView_x = false;

    fin.open( "data/test1_obj/M484SpaceSoldier_SSdata.txt" );
    if( !fin ) return false;

    spaceManSS.init(fin);
    // other platformGuy data
    fin >> setRunLt >> setRunRt >> setStill;
    fin >> frFaceLt >> frFaceRt >> frJumpLt >> frJumpRt;
    fin >> frHitLt >> frHitRt >> frCrouchLt >> frCrouchRt;
    fin.close();

    // ladder Leg
    fin.open( "data/test1_obj/legData.txt" );
    if( !fin ) return false;
    float a,b,c,d;
    fin >> a >> b >> c >> d;
    ladderLeg.init(a,b,c,d);
    fin.close();

    // ani and ball final inits
    float scale = 2.0f;
    aniSS.init( &spaceManSS, setStill, 100.0f, 100.0f, scale );
    aniSS.frIdx = frFaceRt;
    aniSS.spr.setTextureRect( aniSS.getFrRect() );
    aniSS.loopSet = false;
    B1.r = (float)spaceManSS.getFrRect( frFaceRt, setStill ).height*scale/2.0f;
    B1.img.setRadius( B1.r );
    B1.img.setOrigin( B1.r, B1.r );

    return true;
}

bool test1_obj::handleEvent( sf::Event& rEvent )// mouse input launches rB
{
    static vec2d msePosUp, msePosDn;

    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::Left )// run left
        {
            if( aniSS.setNum != setRunLt )
            {
                B1.maxVel = -maxSpeed;
                aniSS.setFrames( setRunLt, false, 0 );
            }
        }
        else if ( rEvent.key.code == sf::Keyboard::Right )// run right
        {
            if( aniSS.setNum != setRunRt )
            {
                B1.maxVel = maxSpeed;
                aniSS.setFrames( setRunRt, false, 0 );
            }
        }
        else if ( rEvent.key.code == sf::Keyboard::Up )// climb up
        {
            if( aniSS.pLeg ) climbState = -1;
        }
        else if ( rEvent.key.code == sf::Keyboard::Down )// climb down
        {
            if( aniSS.pLeg ) climbState = 1;
        }
        else if ( rEvent.key.code == sf::Keyboard::J )// jump
        {
            jump = true;
            if( aniSS.setNum == setStill )
            {
                if( aniSS.frIdx == frFaceLt || aniSS.frIdx == frHitLt || aniSS.frIdx == frCrouchLt )
                    aniSS.setFrames( setStill, false, frJumpLt );
                else aniSS.setFrames( setStill, false, frJumpRt );
            }
            else if( B1.maxVel < 0.0f ) aniSS.setFrames( setStill, false, frJumpLt );
            else aniSS.setFrames( setStill, false, frJumpRt );
        }
        else if ( rEvent.key.code == sf::Keyboard::F5 )// get on or off of ladder
        {
            climb_onOff();
        }
        else if ( rEvent.key.code == sf::Keyboard::L ) aniSS.loopSet = !aniSS.loopSet;// toggle
        else if ( rEvent.key.code == sf::Keyboard::S ) { aniSS.incrSet(); to_sf_string(msgNum1, aniSS.setNum); to_sf_string(msgNum2, aniSS.frIdx); }
        else if ( rEvent.key.code == sf::Keyboard::F ) { aniSS.incrFrame(); to_sf_string(msgNum2, aniSS.frIdx); }
     //   else if ( rEvent.key.code == sf::Keyboard::F5 ) snapShot = true;
    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        if ( rEvent.key.code == sf::Keyboard::Left )// stop running left
        {
            aniSS.setFrames( setStill, false, frFaceLt );
            B1.maxVel = 0.0f;
        }
        else if ( rEvent.key.code == sf::Keyboard::Right )// stop running right
        {
            aniSS.setFrames( setStill, false, frFaceRt );
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
            if( aniSS.frIdx == frJumpLt ) aniSS.setFrames( setStill, false, frFaceLt );
            else aniSS.setFrames( setStill, false, frFaceRt );
        }
    }
    else if (rEvent.type == sf::Event::MouseMoved)
    {
        msePosUp.x = rEvent.mouseMove.x;
        msePosUp.y = rEvent.mouseMove.y;

        if( bigView_x )
        {
            msePosUp.x += mapView.getCenter().x - iPos_viewCtr.x;
            msePosUp.y += mapView.getCenter().y - iPos_viewCtr.y;
        }

        if( lbuttHeld ) { pullLine[1].position.x = msePosUp.x; pullLine[1].position.y = msePosUp.y; }
    }
    else if (rEvent.type == sf::Event::MouseButtonPressed)// lbutt down
    {
        if (rEvent.mouseButton.button == sf::Mouse::Left)
        {
            msePosDn.x = rEvent.mouseButton.x;
            msePosDn.y = rEvent.mouseButton.y;
            lbuttHeld = true;

            if( bigView_x )
            {
                msePosDn.x += mapView.getCenter().x - iPos_viewCtr.x;
                msePosDn.y += mapView.getCenter().y - iPos_viewCtr.y;
            }

            B1.setPosition( msePosDn );
            B1.v.x = B1.v.y = 0.0f;
            pullLine[0].position.x = msePosDn.x; pullLine[0].position.y = msePosDn.y;
            pullLine[1].position.x = msePosDn.x; pullLine[1].position.y = msePosDn.y;
        }
    }
    else if (rEvent.type == sf::Event::MouseButtonReleased)// lbutt up
    {
        if (rEvent.mouseButton.button == sf::Mouse::Left)
        {
            msePosUp.x = rEvent.mouseButton.x;
            msePosUp.y = rEvent.mouseButton.y;
            lbuttHeld = false;

            if( bigView_x )
            {
                msePosUp.x += mapView.getCenter().x - iPos_viewCtr.x;
                msePosUp.y += mapView.getCenter().y - iPos_viewCtr.y;
            }

            pullLine[1].position.x = msePosUp.x; pullLine[1].position.y = msePosUp.y;
            B1.v = msePosUp - msePosDn;
            B1.v *= 0.05f;
            B1.v /= numReps;// scale launch speed
            std::cout << "ball.v = " << B1.v.mag() << '\n';
        }
    }

    return true;
}

void test1_obj::logic()
{

    PG_update();

    // view position
    if( bigView_x && !lbuttHeld )
    {
        const float shiftMargin = 300.0f;
     //   std::cerr << "bigView! ";

        if( B1.pos.x > mapView.getCenter().x + shiftMargin  )// move right
        {
            if( B1.pos.x - shiftMargin > ball::wdwW - iPos_viewCtr.x )
                mapView.setCenter( ball::wdwW - iPos_viewCtr.x, iPos_viewCtr.y );
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
    }

    return;
}

void test1_obj::draw( sf::RenderWindow& RW )
{
    if( bigView_x ) RW.setView( mapView );

    for( quadPack& qp : qpVec ) qp.draw(RW);
    B1.draw(RW);
    aniSS.draw(RW);
 //   for( const lineSeg& ls : lsVec ) ls.draw(RW);
    for( const auto& x : pCBvec ) x->draw(RW);

// save a screenshot when F5 is pressed
    if ( collideSim::snapShot )
    {
        sf::Image Screen = RW.capture();
        Screen.saveToFile("screenshot.jpg");
        collideSim::snapShot = false;
    }

    if( lbuttHeld ) RW.draw(pullLine, 2, sf::Lines);

    if( bigView_x ) RW.setView( RW.getDefaultView() );

    RW.draw(msg1);
    RW.draw(msgNum1);
    RW.draw(msg2);
    RW.draw(msgNum2);

    return;
}

void test1_obj::cleanup()
{
return;
}

test1_obj::~test1_obj()
{
    ball::wdwW = old_wdwW;
    ball::wdwH = old_wdwH;
    ball::wdwCf = 0.0f;
    for( auto& x : pCBvec ) delete x;
    std::cout << "Bye-bye from test1_obj dtor\n";
}

void test1_obj::reset()
{
    return;
}

// ladder use
void test1_obj::climb_onOff()
{
    if( aniSS.pLeg )// climbing off
    {
        aniSS.pLeg = nullptr;
        climbState = 0;
    }
    else// climbing on
    {
        vec2d Pt1( ladderLeg.x0, ladderLeg.y0 );// try this end
        vec2d Pt2( ladderLeg.xf, ladderLeg.yf );// try this end
        if( B1.is_inMe(Pt1) )
        {
            aniSS.pLeg = &ladderLeg;
            aniSS.s = 0.0f;
            climbState = 0;
            aniSS.setPosition( Pt1.x, Pt1.y );
            B1.setPosition( Pt1 );
        }
        else if( B1.is_inMe(Pt2) )
        {
            aniSS.pLeg = &ladderLeg;
            aniSS.s = aniSS.pLeg->len;
            climbState = 0;
            aniSS.setPosition( Pt2.x, Pt2.y );
            B1.setPosition(Pt2);
        }
    }
}

void test1_obj::PG_update()
{
    if( aniSS.pLeg )// climbing
    {
        if( climbState != 0 )// moving
        {
            aniSS.v = climbState*climbSpeed;
            aniSS.pLeg->update( aniSS.x, aniSS.y, aniSS.s, aniSS.v );
            aniSS.update( aniSS.x, aniSS.y );
            B1.setPosition( vec2d(aniSS.x, aniSS.y) );
        }
    }
   else
   {
        if( !lbuttHeld ) B1.update( gravity );
        aniSS.update( B1.pos.x, B1.pos.y );
        PG_hit();
        PG_step();
   }
}

void test1_obj::PG_hit()
{
    for( auto& seg : pCBvec )
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
}

void test1_obj::PG_step()
{
    static float dStep = 0.0f;
    if(  aniSS.setNum != setStill )
    {
        dStep += 5.0f*(float)numReps*B1.maxVel;
        if( aniSS.setNum == setRunRt )// running right
        {
            if( dStep > aniSS.w )
            {
                aniSS.incrFrame();
                dStep = 0.0f;
            //    std::cerr << "frame incr right\n";
            }
            else if( dStep < -1.0f*aniSS.w )// should never occur
            {
                aniSS.decrFrame();
                dStep = 0.0f;
                std::cerr << "frame decr right\n";
            }
        }
        else// running left
        {
            if( dStep < -1.0f*aniSS.w )
            {
                aniSS.incrFrame();
                dStep = 0.0f;
            //    std::cerr << "frame incr left\n";
            }
            else if( dStep > aniSS.w )// should never occur
            {
                aniSS.decrFrame();
                dStep = 0.0f;
                std::cerr << "frame decr left\n";
            }
        }
    }
}
