#include "lvlB.h"

bool lvlB::init()
{
    std::ifstream fin("include/level/lvlB_data/all_data.txt");
    if( !fin ) { std::cout << "no data\n"; return false; }
    fin >> pos0.x >> pos0.y;
    fin >> dSep.x >> dSep.y;
    fin >> rows >> cols; std::cout << "\nrows: " << rows << " cols: " << cols;
    float Fk_min, Fk_max;
    fin >> Fk_min >> Fk >> Fk_max;
    float Fd_min, Fd_max;
    fin >> Fd_min >> Fd >> Fd_max;
    float Fapp_min, Fapp_max;
    fin >> Fapp_min >> Fapp >> Fapp_max;
    float Reff_min, Reff_max;
    fin >> Reff_min >> Reff >> Reff_max;
    float Rfull_min, Rfull_max;
    fin >> Rfull_min >> Rfull >> Rfull_max;
    float vWave_min, vWave_max;
    fin >> vWave_min >> vWave >> vWave_max;

    fin.close();

    ball tmpB;
    tmpB.v.x = tmpB.v.y = 0.0f;
    tmpB.r = 7.0f;
    tmpB.m = 5.0f; tmpB.Cr = 0.8f;
    tmpB.img.setRadius(tmpB.r);
    tmpB.img.setFillColor( sf::Color(100,0,150) );
    tmpB.img.setOrigin( tmpB.r,tmpB.r );
    pMvHitVec.reserve(rows*cols);
    for( size_t r = 0; r < rows; ++r )
    for( size_t c = 0; c < cols; ++c )
    {
        tmpB.setPosition( pos0 + vec2d( c*dSep.x, r*dSep.y ) );
        pMvHitVec.push_back( tmpB.clone() );
    }

    sf::Text label("restore", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    restoreForceControl.init( 20.0f, 80.0f, 70, 25, label );
    restoreForceControl.persist = 3;
    restoreForceControl.ownButts = true;
    sf::Vector2f pos0 = restoreForceControl.pos;
    label.setString("spring stiff");
    restoreForceControl.pButtVec.push_back( new buttonValOnHit( pos0.x, pos0.y+47.0f, 150, 20, label, Fk_min, Fk_max, Fk, &Fk ) );
 //   FkStrip.init( pos0.x, pos0.y+50.0f, 150, 20, label, Fk_min, Fk_max, Fk, &Fk );
    label.setString("damping");
    restoreForceControl.pButtVec.push_back( new buttonValOnHit( pos0.x, pos0.y+94.0f, 150, 20, label, Fd_min, Fd_max, Fd, &Fd ) );
 //   FdStrip.init( pos0.x, pos0.y+100.0f, 150, 20, label, Fd_min, Fd_max, Fd, &Fd );
 //   button::RegisteredButtVec.push_back( &FkStrip );
    button::RegisteredButtVec.push_back( &restoreForceControl );

    label.setString("boom");
    boomForceControl.init( 20.0f, 220.0f, 70.0f, 25.0f, label );
    boomForceControl.persist = 3;
    boomForceControl.ownButts = true;
    pos0 = boomForceControl.pos;
    label.setString("Max Force");
    boomForceControl.pButtVec.push_back( new buttonValOnHit( pos0.x, pos0.y+47.0f, 150, 20, label, Fapp_min, Fapp_max, Fapp, &Fapp ) );
 //   FappStrip.init( 100.0f, 350.0f, 150, 20, label, Fapp_min, Fapp_max, Fapp, &Fapp );
 //   button::RegisteredButtVec.push_back( &FappStrip );
    label.setString("Reff");
    boomForceControl.pButtVec.push_back( new buttonValOnHit( pos0.x, pos0.y+94.0f, 150, 20, label, Reff_min, Reff_max, Reff, &Reff ) );
 //   ReffStrip.init( 100.0f, 400.0f, 150, 20, label, Reff_min, Reff_max, Reff, &Reff );
//    button::RegisteredButtVec.push_back( &ReffStrip );
    label.setString("Rfull");
    boomForceControl.pButtVec.push_back( new buttonValOnHit( pos0.x, pos0.y+141.0f, 150, 20, label, Rfull_min, Rfull_max, Rfull, &Rfull ) );
 //   RfullStrip.init( 100.0f, 450.0f, 150, 20, label, Rfull_min, Rfull_max, Rfull, &Rfull );
 //   button::RegisteredButtVec.push_back( &RfullStrip );
    label.setString("wave speed");
    boomForceControl.pButtVec.push_back( new buttonValOnHit( pos0.x, pos0.y+188.0f, 150, 20, label, vWave_min, vWave_max, vWave, &vWave ) );
 //   vWaveStrip.init( 100.0f, 500.0f, 150, 20, label, vWave_min, vWave_max, vWave, &vWave );
 //   button::RegisteredButtVec.push_back( &vWaveStrip );
    button::RegisteredButtVec.push_back( &boomForceControl );

    rec::init( 1000, 1, std::bind( lvlB::init_rec, this, std::placeholders::_1), &recorder_on );
    ctUtil::init_recorderControl( recorderControl, 20.0f, 480.0f, 80.0f, 20.0f );
    button::RegisteredButtVec.push_back( &recorderControl );

    button::RegisteredButtVec.push_back( &Level::goto_MMButt );

    assignMessages();

    return true;
}

bool lvlB::handleEvent( sf::Event& rEvent )
{
     if ( rEvent.type == sf::Event::KeyPressed )
     {
        if ( rEvent.key.code == sf::Keyboard::F3 )// toggle both forces
        {
            isDamped = !isDamped;
            isRestored = isDamped;
        }
        else if ( rEvent.key.code == sf::Keyboard::F4 )// toggle spring restore only
        {
            isDamped = true;
            isRestored = !isRestored;
        }
     }

    return true;
}

void lvlB::update( float dt )
{
 //   if( recorder_on ) ctUtil::updateRecorderDisplay();

 //   if( button::clickEvent_Rt() == 1 && !isDamped )// reset positions
    if( button::clickEvent_Rt() == 1 )// reset positions
    {
        for( size_t i = 0; i < pMvHitVec.size(); ++i )
        {
            pMvHitVec[i]->setPosition( pos0 + vec2d( dSep.x*(i%cols), dSep.y*(i/cols) ) );
            pMvHitVec[i]->v.x = pMvHitVec[i]->v.y = 0.0f;
        }

        R0 = Rmax + 1.0f;// stop wave spread
        return;
    }

    // reset center
    if( button::clickEvent_Lt() == 1 && !button::pButtMse )
    {
        expPt.x =  button::mseX; expPt.y =  button::mseY;
        R0 = 0.0f;// start wave spread
        vec2d sep = pos0 - expPt;// up left field corner
        Rmax = sep.mag();
        sep.x += dSep.x*cols;// up right
        if( sep.mag() > Rmax ) Rmax = sep.mag();
        sep.y += dSep.y*rows;// bottom rt
        if( sep.mag() > Rmax ) Rmax = sep.mag();
        sep.x -= dSep.x*cols;// bottom lt
        if( sep.mag() > Rmax ) Rmax = sep.mag();
    }

//    if( !(run_update || step_update) ) return;

 //   const float Reff = 100.0f;
    size_t idx = 0;
    for( auto& pMH : pMvHitVec )
    {
        vec2d Ftot;// = Fk*( posBase - pMH->pos ) - Fd*pMH->v;
        if( isDamped ) Ftot -= Fd*pMH->v;
        if( isRestored )
        {
            vec2d posBase = pos0 + vec2d( dSep.x*(idx%cols), dSep.y*(idx/cols) );
            Ftot += Fk*( posBase - pMH->pos );// - Fd*pMH->v;
            ++idx;
        }

        if( R0 < Rmax )
        {
            // is mh in annular region?
            vec2d sep = pMH->pos - expPt;
            float Rmh = sep.mag(); if( Rmh < 1.0f ) Rmh = 1.0f;// safe for div by 0 below
            if( Rmh > R0-Reff && Rmh < R0+Reff )
            {
               if( R0 > Rfull ) Ftot += (Fapp/Rmh)*(Rfull/R0)*sep;
               else Ftot += (Fapp/Rmh)*sep;
            }
        }

        pMH->update( dt, Ftot/pMH->m );
    }

    if( R0 < Rmax ) R0 += vWave*dt;

 //   if( !isDamped )
    if( !( isDamped && isRestored ) )
    {
        for( size_t i=0; i+1< pMvHitVec.size(); ++i )// mvHits vs mvHits
        for( size_t j=i+1; j< pMvHitVec.size(); ++j )
            pMvHitVec[i]->hit( *pMvHitVec[j] );
    }

    return;
}

/*
void lvlB::update()
{
    if( button::clickEvent_Rt() == 1 && !isDamped )// reset positions
    {
        for( size_t i = 0; i < pMvHitVec.size(); ++i )
        {
            pMvHitVec[i]->setPosition( pos0 + vec2d( dSep.x*(i%cols), dSep.y*(i/cols) ) );
            pMvHitVec[i]->v.x = pMvHitVec[i]->v.y = 0.0f;
        }

        return;
    }

 //   const float Reff = 100.0f;
    size_t idx = 0;
    for( auto& pMH : pMvHitVec )
    {
        vec2d Ftot;// = Fk*( posBase - pMH->pos ) - Fd*pMH->v;
        if( isDamped )
        {
            vec2d posBase = pos0 + vec2d( dSep.x*(idx%cols), dSep.y*(idx/cols) );
            Ftot = Fk*( posBase - pMH->pos ) - Fd*pMH->v;
            ++idx;
        }

        if( button::clickEvent_Lt() == 1 )
        {
            vec2d sep = pMH->pos - vec2d( button::mseX, button::mseY );
            if( sep.dot(sep) < Reff*Reff ) Ftot += Fapp*sep/sep.mag();
        }

        pMH->update( Ftot/pMH->m );
    }

    if( !isDamped )
    {
        for( size_t i=0; i+1< pMvHitVec.size(); ++i )// mvHits vs mvHits
        for( size_t j=i+1; j< pMvHitVec.size(); ++j )
            pMvHitVec[i]->hit( *pMvHitVec[j] );
    }

    return;
}
*/

void lvlB::draw( sf::RenderTarget& RT ) const
{
    for( auto& pMH : pMvHitVec ) pMH->draw(RT);
    for( const auto& msg : msgVec ) RT.draw(msg);
 //   if( recorder_on ) ctUtil::drawRecorderDisplay( RT );

    return;
}

void lvlB::cleanup()
{
    return;
}

void lvlB::init_rec( std::vector<state_ab*>& p_stateVec )
{
    p_stateVec.reserve( pMvHitVec.size() );

    state_ab* pSt = nullptr;
    for( auto& mh : pMvHitVec )// regular mvHits
    {
        pSt = mh->newState();
        if( pSt ) p_stateVec.push_back( pSt );
    }

    // add control states
    int NbuttStates = 0;
    for( auto& pButt : button::RegisteredButtVec )
        NbuttStates += ctUtil::addButtonState( *pButt, p_stateVec );
    std::cout << "added " << NbuttStates << " button states\n";

    return;
}

void lvlB::assignMessages()
{
    const sf::Font& rFont = *button::pFont;
    size_t fontSz = 12;
    float x0 = 20.0f;
    float y = 10.0f;
    float dy = 15.0f;

    msgVec.push_back( sf::Text("Left click for explosion", rFont, fontSz) );
    msgVec.push_back( sf::Text("Right click to restore positions", rFont, fontSz) );
    msgVec.push_back( sf::Text("F3 toggles free motion", rFont, fontSz) );
    msgVec.push_back( sf::Text("F4 toggles spring restore force", rFont, fontSz) );

    for( auto& msg : msgVec )
    {
        msg.setColor( sf::Color::White );
        msg.setPosition( x0, y );
        y += dy;
    }
}
