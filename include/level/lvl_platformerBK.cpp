#include "lvl_platformer.h"

bool lvl_platformer::init()
{
    mvHit::wdwCf = 0.3f;
 //   Level::usingMapView = true;
    mvHit::windowTopOpen = true;

    msg1.setFont(*button::pFont);
    msg1.setCharacterSize(16);
    msg1.setString("setNum: ");// = "Ek = ";
    msg1.setPosition( 100.0f, 10.0f );
    msgNum1.setFont(*button::pFont);
    msgNum1.setCharacterSize(16);
    msgNum1.setString("0");// = "0.0f";
    msgNum1.setPosition( 160.0f, 10.0f );

    msg2.setFont(*button::pFont);
    msg2.setCharacterSize(16);
    msg2.setString("frIdx : ");// = "Ek = ";
    msg2.setPosition( 100.0f, 30.0f );
    msgNum2.setFont(*button::pFont);
    msgNum2.setCharacterSize(16);
    msgNum2.setString("0");// = "0.0f";
    msgNum2.setPosition( 160.0f, 30.0f );

    mapView.reset(sf::FloatRect( 0.0f, 0.0f, mvHit::wdwW, mvHit::wdwH ));
    old_wdwW = mvHit::wdwW;
    old_wdwH = mvHit::wdwH;
    iPos_viewCtr.x = old_wdwW/2.0f;
    iPos_viewCtr.y = old_wdwH/2.0f;

    // the ball_go encasing platform guy
    std::ifstream fin("include/level/platformer_data/ball_data_1.txt");
    if( !fin ) return false;
    B1.init( fin );
    maxSpeed = B1.maxVel;
    B1.maxVel = 0.0f;
    fin >> mvHit::wdwCf;// extra data from file
    gravity_on = false;
    fin >> gravity_on;
    if( gravity_on ) fin >> gravity.x >> gravity.y;
    fin >> jumpSpeed >> bounceAmp;
    fin >> climbSpeed;
    numReps = 1;
    fin.close();

    fin.open( "include/level/platformer_data/platform_SSdata.txt" );
    if( !fin ) return false;
    platformSS.init(fin);
    fin.close();

    fin.open( "include/level/platformer_data/quadPack_data.txt" );
    if( !fin ) return false;
    size_t numqp; fin >> numqp;
    qpVec.reserve( numqp );
    vec2d ofst( 0.0f, 5.0f );
    for( size_t i=0; i<numqp; ++i ) qpVec.push_back( quadPack( fin, &platformSS ) );
    fin.close();

    // load some segs
    fin.open( "include/level/platformer_data/segData.txt" );
    if( !fin ) return false;
    if( !loadSegs( fin, pCBvec ) )
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
        x->Cf = mvHit::wdwCf;
    }

    mvHit::wdwW = worldWidth = pCBvec.back()->pos.x + 500.0f;
    if( worldWidth > old_wdwW ) bigView_x = true;
    else bigView_x = false;

    fin.open( "include/level/platformer_data/M484SpaceSoldier_SSdata.txt" );
    if( !fin ) return false;

    spaceManSS.init(fin);
    // other platformGuy data
    fin >> setRunLt >> setRunRt >> setStill;
    fin >> frFaceLt >> frFaceRt >> frJumpLt >> frJumpRt;
    fin >> frHitLt >> frHitRt >> frCrouchLt >> frCrouchRt;
    fin.close();

    // ladder Leg
    fin.open( "include/level/platformer_data/legData.txt" );
    if( !fin ) return false;
    float a,b,c,d;
    fin >> a >> b >> c >> d;
    ladderLeg.init(a,b,c,d);
    fin.close();

    // ani and ball final inits
 //   B1.r = (float)spaceManSS.getFrRect( frFaceRt, setStill ).height/2.0f;
 //   B1.img.setRadius( B1.r );
 //   B1.img.setOrigin( B1.r, B1.r );
    float scl = 2.0f;
    platGuy.init( nullptr, &B1, 0.0f, 0.0f );
    platGuy.init2( &spaceManSS, setStill, scl );
    platGuy.setPosition( 100, 100 );
    platGuy.frIdx = frFaceRt;
    platGuy.spr.setTextureRect( platGuy.getFrRect() );
    platGuy.loopSet = false;

//    B1.r = (float)spaceManSS.getFrRect( frFaceRt, setStill ).height/2.0f;
    B1.r = scl*(float)platGuy.getFrRect().height/2.0f;
    B1.img.setRadius( B1.r );
    B1.img.setOrigin( B1.r, B1.r );

    Level::pMHfling = platGuy.pMH;

 //   float scale = 2.0f;
 //   platGuy.init( &spaceManSS, setStill, 100.0f, 100.0f, scale );
 //   platGuy.frIdx = frFaceRt;
 //   platGuy.spr.setTextureRect( platGuy.getFrRect() );
  //  platGuy.loopSet = false;
 //   B1.r = (float)spaceManSS.getFrRect( frFaceRt, setStill ).height*scale/2.0f;
 //   B1.img.setRadius( B1.r );
  //  B1.img.setOrigin( B1.r, B1.r );
 //   platGuy.pMH = &B1;

    return true;
}

bool lvl_platformer::handleEvent( sf::Event& rEvent )
{
    static vec2d msePosUp, msePosDn;

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
            jump = true;
            if( platGuy.setNum == setStill )
            {
                if( platGuy.frIdx == frFaceLt || platGuy.frIdx == frHitLt || platGuy.frIdx == frCrouchLt )
                    platGuy.setFrames( setStill, false, frJumpLt );
                else platGuy.setFrames( setStill, false, frJumpRt );
            }
            else if( B1.maxVel < 0.0f ) platGuy.setFrames( setStill, false, frJumpLt );
            else platGuy.setFrames( setStill, false, frJumpRt );
        }
        else if ( rEvent.key.code == sf::Keyboard::F5 )// get on or off of ladder
        {
            climb_onOff();
        }
        else if ( rEvent.key.code == sf::Keyboard::L ) platGuy.loopSet = !platGuy.loopSet;// toggle
        else if ( rEvent.key.code == sf::Keyboard::S ) { platGuy.incrSet(); to_sf_string(msgNum1, platGuy.setNum); to_sf_string(msgNum2, platGuy.frIdx); }
        else if ( rEvent.key.code == sf::Keyboard::F ) { platGuy.incrFrame(); to_sf_string(msgNum2, platGuy.frIdx); }
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
            if( platGuy.frIdx == frJumpLt ) platGuy.setFrames( setStill, false, frFaceLt );
            else platGuy.setFrames( setStill, false, frFaceRt );
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

    }
    else if (rEvent.type == sf::Event::MouseButtonPressed)// lbutt down
    {
        if (rEvent.mouseButton.button == sf::Mouse::Left)
        {
            msePosDn.x = rEvent.mouseButton.x;
            msePosDn.y = rEvent.mouseButton.y;
        //    lbuttHeld = true;

            if( bigView_x )
            {
                msePosDn.x += mapView.getCenter().x - iPos_viewCtr.x;
                msePosDn.y += mapView.getCenter().y - iPos_viewCtr.y;
            }
        }
    }
    else if (rEvent.type == sf::Event::MouseButtonReleased)// lbutt up
    {
        if (rEvent.mouseButton.button == sf::Mouse::Left)
        {
            msePosUp.x = rEvent.mouseButton.x;
            msePosUp.y = rEvent.mouseButton.y;
        //    lbuttHeld = false;

            if( bigView_x )
            {
                msePosUp.x += mapView.getCenter().x - iPos_viewCtr.x;
                msePosUp.y += mapView.getCenter().y - iPos_viewCtr.y;
            }
        }
    }

    return true;
}

void lvl_platformer::update()
{

    PG_update();

    // view position
    if( bigView_x && !button::mseDnLt )
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
    }

    return;
}
void lvl_platformer::draw_map( sf::RenderTarget& RT ) const
{
    for( const quadPack& qp : qpVec ) qp.draw(RT);
//    B1.draw(RT);
    platGuy.draw(RT);
    for( const auto& x : pCBvec ) x->draw(RT);
}

void lvl_platformer::draw( sf::RenderTarget& RT ) const
{
    if( bigView_x ) RT.setView( mapView );

    for( const quadPack& qp : qpVec ) qp.draw(RT);
    B1.draw(RT);
    platGuy.draw(RT);
    for( const auto& x : pCBvec ) x->draw(RT);

// save a screenshot when F5 is pressed
 //   if ( collideSim::snapShot )
 //   {
 //       sf::Image Screen = RT.capture();
 //       Screen.saveToFile("screenshot.jpg");
 //       collideSim::snapShot = false;
 //   }

//    if( bigView_x ) RT.setView( RT.getDefaultView() );

    RT.draw(msg1);
    RT.draw(msgNum1);
    RT.draw(msg2);
    RT.draw(msgNum2);

    return;
}

void lvl_platformer::cleanup()
{
return;
}

lvl_platformer::~lvl_platformer()
{
    mvHit::wdwW = old_wdwW;
    mvHit::wdwH = old_wdwH;
    mvHit::wdwCf = 0.0f;
    for( auto& x : pCBvec ) delete x;
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

void lvl_platformer::PG_update()
{
    if( platGuy.pLeg )// climbing
    {
        if( climbState != 0 )// moving
        {
            platGuy.v = climbState*climbSpeed;
            platGuy.update();
        }
    }
   else
   {
        platGuy.update( gravity );
        PG_hit();
        PG_step();
   }
}

void lvl_platformer::PG_hit()
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

void lvl_platformer::PG_step()
{
    static float dStep = 0.0f;
    float pgW = 2.0f*B1.r;

    if(  platGuy.setNum != setStill )
    {
        dStep += 5.0f*(float)numReps*B1.maxVel;
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
                std::cerr << "frame decr right\n";
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
                std::cerr << "frame decr left\n";
            }
        }
    }
}
