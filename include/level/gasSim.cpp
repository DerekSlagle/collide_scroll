#include "gasSim.h"

bool gasSim::init( const sf::Font& font )
{

    // load a ball. Controlled by mouse
    std::ifstream fin("data/gasSim/data_1.txt");// blue ball data
    if( !fin ) return false;

    float R;
    unsigned int rd,gn,bu;
    fin >> R >> membraneX;
    fin >> rd >> gn >> bu;
    sf::Color ballClr(rd,gn,bu);
    fin >> rd >> gn >> bu;
    sf::Color wallClr(rd,gn,bu);// a, c segments
    fin >> rd >> gn >> bu;
    sf::Color valveClr(rd,gn,bu);// b segment

    fin >> rd >> gn >> bu;
    sf::Color sinkClr(rd,gn,bu);

     // load balls
    fin >> nBalls;// >> nWallSegs;
    float a, b;
    fin >> a >> b;
    std::cout << "nBalls = " << nBalls << '\n';
    std::cout << "a = " << a  << "  b = " << b << '\n';

    fin >> coolCoeff >> heatCoeff;
    std::cout << "coolCoeff = " << coolCoeff  << "  heatCoeff = " << heatCoeff << '\n';

    fin.close();

    // wall segs
    nWallSegs = 3;
    lineSeg* pLS = new lineSeg[nWallSegs];
    pLS[0].pos.x = membraneX; pLS[0].pos.y = 0.0f;// seg a
    pLS[0].L.x = 0.0f; pLS[0].L.y = a;
    pLS[0].vtx[1].color = pLS[0].vtx[0].color = wallClr;
    pLS[0].vtx[0].position.x = membraneX; pLS[0].vtx[0].position.y = 0.0f;
    pLS[0].vtx[1].position.x = membraneX; pLS[0].vtx[1].position.y = a;
    pLS[0].N = pLS[0].L.get_LH_norm();

    pLS[1].pos.x = membraneX; pLS[1].pos.y = a;// seg b
    pLS[1].L.x = 0.0f; pLS[1].L.y = b;
    pLS[1].vtx[1].color = pLS[1].vtx[0].color = valveClr;
    pLS[1].vtx[0].position.x = membraneX; pLS[1].vtx[0].position.y = a;
    pLS[1].vtx[1].position.x = membraneX; pLS[1].vtx[1].position.y = a + b;
    pLS[1].N = pLS[1].L.get_LH_norm();

    pLS[2].pos.x = membraneX; pLS[2].pos.y = a + b;// seg c
    pLS[2].L.x = 0.0f; pLS[2].L.y = ball::wdwH - (a + b);
    pLS[2].vtx[1].color = pLS[2].vtx[0].color = wallClr;
    pLS[2].vtx[0].position.x = membraneX; pLS[2].vtx[0].position.y = a + b;
    pLS[2].vtx[1].position.x = membraneX; pLS[2].vtx[1].position.y = ball::wdwH;
    pLS[2].N = pLS[2].L.get_LH_norm();

    pCol = pLS;


    // balls
    vec2d iPos( 2.0f*R, 2.0f*R );
    pB = new ball[nBalls];

    for(size_t i=0; i<nBalls; ++i)
    {
        pB[i].r = R;
        pB[i].pos = iPos;
        pB[i].v.x = pB[i].v.y = 0.0f;
        pB[i].img.setRadius(R);
        pB[i].img.setPosition(iPos.x, iPos.y);
        ballClr.b = ( ballClr.b + 10 )%255;
        pB[i].img.setFillColor( ballClr );
        pB[i].img.setOrigin( R,R );
        pB[i].m = 1.0f; pB[i].Cr = 1.0f;

        iPos.x += 3.0f*R;
        if( iPos.x > membraneX - 2.0f*R )
        {
            iPos.x = 2.0f*R;
            iPos.y += 3.0f*R;
        }
    }

    valve_open = false;

    // the sink
    sinkMode = 0;
    sink.pos.x = 8.0f; sink.pos.y = 200.0f;// seg b
    sink.L.x = 0.0f; sink.L.y = 300.0f;
    sink.vtx[1].color = sink.vtx[0].color = sinkClr;
    sink.vtx[0].position.x = sink.pos.x; sink.vtx[0].position.y = sink.pos.y;
    sink.vtx[1].position.x = sink.pos.x; sink.vtx[1].position.y = sink.pos.y + sink.L.y;
    sink.N = sink.L.get_LH_norm();

    // the messages
//    float msgSpc = 30.0f;

    numltMsg.setFont(font);
    numltMsg.setColor( sf::Color::Green );
    numltMsg.setCharacterSize(20);
    numltQtyMsg = EltMsg = EltQtyMsg = EltAvgMsg = EltAvgQtyMsg = numltMsg;
    numrtMsg = numrtQtyMsg = ErtMsg = ErtQtyMsg = ErtAvgMsg = ErtAvgQtyMsg = numltMsg;


    numltMsg.setString( "num:" );    numltMsg.setPosition( 40.0f, 20.0f );
    numltQtyMsg.setString( "0" );         numltQtyMsg.setPosition( 120.0f, 20.0f );
    EltMsg.setString( "Ek:" );       EltMsg.setPosition( 40.0f, 60.0f );
    EltQtyMsg.setString( "0" );           EltQtyMsg.setPosition( 120.0f, 60.0f );
    EltAvgMsg.setString( "Eavg:" );  EltAvgMsg.setPosition( 40.0f, 100.0f );
    EltAvgQtyMsg.setString( "0.0" );      EltAvgQtyMsg.setPosition( 120.0f, 100.0f );

    numrtMsg.setString( "num:" );    numrtMsg.setPosition( membraneX + 40.0f, 20.0f );
    numrtQtyMsg.setString( "0" );         numrtQtyMsg.setPosition( membraneX + 120.0f, 20.0f );
    ErtMsg.setString( "Ek:" );       ErtMsg.setPosition( membraneX + 40.0f, 60.0f );
    ErtQtyMsg.setString( "0" );           ErtQtyMsg.setPosition( membraneX + 120.0f, 60.0f );
    ErtAvgMsg.setString( "Eavg:" );  ErtAvgMsg.setPosition( membraneX + 40.0f, 100.0f );
    ErtAvgQtyMsg.setString( "0.0" );      ErtAvgQtyMsg.setPosition( membraneX + 120.0f, 100.0f );

    return true;
}

void gasSim::reset()
{
    if( pB )
    {
        float R = pB[0].r;
        vec2d iPos( 2.0f*R, 2.0f*R );
        for( size_t i=0; i<nBalls; ++i )
        {
            pB[i].pos = iPos;
            pB[i].v.x = pB[i].v.y = 0.0f;
            pB[i].img.setPosition(iPos.x, iPos.y);
            iPos.x += 3.0f*R;
            if( iPos.x > membraneX - 2.0f*R )
            {
                iPos.x = 2.0f*R;
                iPos.y += 3.0f*R;
            }
        }
    }

    valve_open = false;

    sink.vtx[1].color = sink.vtx[0].color = sinkClr;
    sinkMode = 0;

    return;
}

bool gasSim::handleEvent( sf::Event& rEvent )// mouse input launches rB
{
 //   static vec2d msePosUp, msePosDn;
 //   if( !collideSim::handleEvent( rEvent ) ) return false;

    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::I )
        {
            pB[4].v.x = 5.0f; pB[4].v.y = 2.0f;
            pB[nBalls-4].v.x = -5.0f; pB[nBalls-4].v.y = -3.0f;
        }
        else if ( rEvent.key.code == sf::Keyboard::R ) reset();// reset to start conditions
        else if ( rEvent.key.code == sf::Keyboard::V ) valve_open = !valve_open;// operate valve
        else if ( rEvent.key.code == sf::Keyboard::H ) { sinkMode = 1; sink.vtx[1].color = sink.vtx[0].color = sf::Color(255,0,0); }// heat
        else if ( rEvent.key.code == sf::Keyboard::C ) { sinkMode = -1; sink.vtx[1].color = sink.vtx[0].color = sf::Color(0,0,255); }// cool
        else if ( rEvent.key.code == sf::Keyboard::O ) { sinkMode = 0; sink.vtx[1].color = sink.vtx[0].color = sinkClr; }// off
    }

    return true;
}

void gasSim::logic()
{

  // update() and hit() calls
    for( size_t k=0; k<numReps; ++k)
    {
        for( size_t i=0; i< nBalls; ++i ) pB[i].update();

        for( size_t i=0; i< nBalls - 1; ++i )
            for( size_t j=i+1; j< nBalls; ++j )
//                if( pB[i].hit(pB[j]) ) break;// stopping at 1st hit
                if( (((pB[i].pos.x < membraneX) && (pB[j].pos.x < membraneX)) || ((pB[i].pos.x > membraneX) && (pB[j].pos.x > membraneX))) && pB[i].hit(pB[j]) ) break;// stopping at 1st hit

        // sink hits
     /*   if( sinkMode != 0 )
        {
       //     for( size_t i=0; i< nBalls; ++i ) sink.hit( pB[i] );
            for( size_t i=0; i< nBalls; ++i )
                if(  sink.hit( pB[i] ) )
                {
                    float magV = pB[i].v.mag();
                    float newMagV = sinkMode == 1 ? (magV + heatCoeff)/2.0f : (magV + coolCoeff)/2.0f;
                    pB[i].v *= (newMagV/magV);

               //     if( sinkMode == 1 ) pB[i].v *= heatCoeff;// heat
               //     else pB[i].v *= coolCoeff;// cool
                }
        }   */

        // wall collisions
        for( size_t i=0; i< nBalls; ++i )
        {
            if( pB[i].pos.x > membraneX - pB[0].r && pB[i].pos.x < membraneX + pB[0].r )
            {
                if(  pCol[0].hit( pB[i] ) ) continue;
                if(  pCol[2].hit( pB[i] ) ) continue;

                if(  !valve_open )
                    pCol[1].hit( pB[i] );
                else// test two points
                {
                    if(  pB[i].hit( pCol[1].pos ) ) continue;
                    pB[i].hit( pCol[2].pos );
                }
            }
            else if( sinkMode != 0 && pB[i].pos.x < sink.pos.x + pB[0].r && sink.hit( pB[i] ) )
            {
                float magV = pB[i].v.mag();
                float newMagV = sinkMode == 1 ? (magV + heatCoeff)/2.0f : (magV + coolCoeff)/2.0f;
                pB[i].v *= (newMagV/magV);
            }
        }

        if( k == 0 )// once per render
        {
            int nBallsLt = 0, nBallsRt = 0;
            float Ert = 0.0f,  Elt = 0.0f;
            for( size_t i=0; i< nBalls; ++i )
            {
                if( pB[i].pos.x < membraneX ){ ++nBallsLt; Elt += pB[i].v.dot(pB[i].v)*(0.5f*pB[i].m); }
                else{ ++nBallsRt; Ert += pB[i].v.dot(pB[i].v)*(0.5f*pB[i].m); }
            }

            to_sf_string(EltQtyMsg, Elt);// float to sf::Text
            to_sf_string(ErtQtyMsg, Ert);// float to sf::Text
            to_sf_string(numltQtyMsg, nBallsLt);// int to sf::Text
            to_sf_string(numrtQtyMsg, nBallsRt);// int to sf::Text
            if( nBallsLt > 0 ) to_sf_string(EltAvgQtyMsg, Elt/(float)nBallsLt);// float to sf::Text
            if( nBallsRt > 0 ) to_sf_string(ErtAvgQtyMsg, Ert/(float)nBallsRt);// float to sf::Text
        }


    }

    return;
}

void gasSim::draw( sf::RenderWindow& RW )
{
    if( pB ) for( size_t i=0; i<nBalls; ++i) pB[i].draw(RW);
    if( pCol )
    {
        pCol[0].draw(RW);
        pCol[2].draw(RW);
        if( !valve_open ) pCol[1].draw(RW);
    }

    sink.draw(RW);

    // the 12 messages
    RW.draw(numltMsg);  RW.draw(numltQtyMsg);
    RW.draw(EltMsg);    RW.draw(EltQtyMsg);
    RW.draw(EltAvgMsg); RW.draw(EltAvgQtyMsg);

    RW.draw(numrtMsg);  RW.draw(numrtQtyMsg);
    RW.draw(ErtMsg);    RW.draw(ErtQtyMsg);
    RW.draw(ErtAvgMsg); RW.draw(ErtAvgQtyMsg);
}

void gasSim::cleanup()
{
    if( pB ) delete [] pB;
    pB = nullptr;
    if( pCol ) delete [] pCol;
    pCol = nullptr;
}

gasSim::~gasSim()
{
    std::cout << "Bye-bye from gasSim dtor\n";
    if( pB ) delete [] pB;
    if( pCol ) delete [] pCol;
}

