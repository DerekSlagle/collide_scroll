#include "mainHelper.h"

gun* pGun_1 = nullptr;
expandPolygon *pXpgA = nullptr;
waveSeg* p_wvSeg = nullptr;
lineSegConveyor* pLSconvey = nullptr;

buttonList waveControl;
buttonList gunControl;
buttonList recorderControl;

//waveTank wvTank_1;
 std::vector<waveTank> wvTankVec;

bool loadSegs( std::istream& fin, std::vector<segHit*>& pCBvec )
{
    lineSeg* pLS = nullptr;
    size_t nSegs = 0;
    fin >> nSegs;

    for( size_t i=0; i<nSegs; ++i )
    {
        char type = 'n';
        bool is_Hard = true;
        bool is_bpf = false;
        fin >> type >> is_Hard >> is_bpf;
        switch( type )
        {
            case 'A' :// arcSeg
            pCBvec.push_back( new arcSeg(fin) );
            break;

            case 'L' :// lineSeg
            pLS = new lineSeg(fin);
            pCBvec.push_back( pLS );
            if( !is_Hard ) pLS->vtx[1].color = pLS->vtx[0].color = sf::Color::Blue;
            break;

            case 'S' :// lineSegSwing
            pCBvec.push_back( new lineSegSwing(fin) );
            break;

            case 'C' :// lineSegConveyor
            pLSconvey = new lineSegConveyor(fin);
            pCBvec.push_back( pLSconvey );
            if( !is_Hard ) pLSconvey->vtx[1].color = pLSconvey->vtx[0].color = sf::Color::Blue;
            break;

            case 'W' :// waveSeg
            p_wvSeg = new waveSeg(fin);
            pCBvec.push_back( p_wvSeg );
            break;

            default:
            break;// totally inadequadte default response. Better hope nothing goes wrong here.
            // possible actual response to unsupported type found: Read until eof (so last object ignored)
            // or another char on this list is found, then assume back on track for next object?
        }

        pCBvec.back()->is_hard = is_Hard;
        pCBvec.back()->is_bulletProof = is_bpf;

        if( ball::wdwCf != 0.0f )// friction
        {
            pCBvec.back()->friction_on = true;
            pCBvec.back()->Cf = ball::wdwCf;
        }
    }

    return true;
}

bool load_mvHits( std::istream& is, std::vector<mvHit*>& pMhVec )
{
    size_t nMH = 0;
    is >> nMH;

    for( size_t i=0; i<nMH; ++i )
    {
        char type = 'n';
        is >> type;
        switch( type )
        {
            case 'C' :// circle
            pMhVec.push_back( new ball(is) );
            std::cout << "ball\n";
            break;

            case 'R' :// regPolygon
            pMhVec.push_back( new regPolygon(is) );
            std::cout << "regPoly\n";
            break;

            case 'B' :// block
            pMhVec.push_back( new block(is) );
            std::cout << "block\n";
            break;

            case 'G' :// gun
            pGun_1 = new gun(is);
            pMhVec.push_back( pGun_1 );
            std::cout << "gun\n";
            break;

            case 'D' :// decayPolygon
            pMhVec.push_back( new decayPolygon(is) );
            std::cout << "decayPoly\n";
            break;

            case 'E' :// expandPolygon
                pXpgA = new expandPolygon(is);
                pMhVec.push_back( pXpgA );
                std::cout << "expandPoly\n";
            break;

            default:
            std::cout << "bad mvHit i= " << i << "\n";
            break;// totally inadequadte default response. Better hope nothing goes wrong here.
            // possible actual response to unsupported type found: Read until eof (so last object ignored)
            // or another char on this list is found, then assume back on track for next object?
        }
    }

    return true;
}


bool loadChannel( std::istream& fin, std::vector<segHit*>& pSegVec )
{
    float x, y;// corner position
    float W, L;// width, length
    // load a straight segment
    fin >> x >> y >> W >> L;

    // horizontal
    pSegVec.push_back( new lineSeg( x, y, x+L, y ) );// top
    pSegVec.push_back( new lineSeg( x, y+W, x+L, y+W ) );// bottom
    x += L;

    // load an elbow
    float r;
    fin >> r;
    // right turn
    pSegVec.push_back( new arcSeg( x+r, y+W+r, x, y+W, r ) );// inside
    pSegVec.push_back( new arcSeg( x+r+W, y+W+r, x, y, r+W ) );// outside

    x += r;
    y += W + r;
    // straight segment
    fin >> L;
    // vertical
    pSegVec.push_back( new lineSeg( x, y, x, y+L ) );// left
    pSegVec.push_back( new lineSeg( x+W, y, x+W, y+L ) );// right

    y += L;

    // load an elbow
    fin >> r;
    // left turn
    pSegVec.push_back( new arcSeg( x+W, y, x+W+r, y+r, r ) );// inside
    pSegVec.push_back( new arcSeg( x,   y, x+W+r, y+W+r, r+W ) );// outside

    x += W + r;
    y += r;

    // straight segment
    fin >> L;
    // horixontal
    pSegVec.push_back( new lineSeg( x, y, x+L, y ) );// top
    pSegVec.push_back( new lineSeg( x, y+W, x+L, y+W ) );// bottom
    x += L;

    return true;
}

void loadMessages( sf::Text& helpMsg, sf::Text& pauseMsg, std::vector<sf::Text>& msgVec, const sf::Font& font )
{
    pauseMsg.setFont(font);
    pauseMsg.setColor( sf::Color::Red );
    pauseMsg.setCharacterSize(30);
    pauseMsg.setString( "Pause" );
    pauseMsg.setPosition( 1050.0f, 20.0f );

    helpMsg.setFont(font);
    helpMsg.setColor( sf::Color::White );
    helpMsg.setCharacterSize(20);
    helpMsg.setString( "H =  help" );
    helpMsg.setPosition( 50.0f, 20.0f );

    float x0 = 30.0f;
    float y = 20.0f;
    float dy = 15.0f;
    unsigned int fontSz = 12;
    msgVec.push_back( sf::Text("Esc =  Quit", font, fontSz) );
    msgVec.push_back( sf::Text("F1  pause / resume", font, fontSz) );
    msgVec.push_back( sf::Text("F2  frame step while paused", font, fontSz) );
    msgVec.push_back( sf::Text("F3  capture next mvHit in container", font, fontSz) );
    msgVec.push_back( sf::Text("F4  capture mvHit under mouse", font, fontSz) );
    msgVec.push_back( sf::Text("F5  make captured mvHit rigid (toggles)", font, fontSz) );
    msgVec.push_back( sf::Text("F6  produce clones of captured mvHit at blazing rate", font, fontSz) );
    msgVec.push_back( sf::Text("F7  produce one clone at a time", font, fontSz) );
    msgVec.push_back( sf::Text("F8  mvHit jump up at 5 px/fr", font, fontSz) );
    msgVec.push_back( sf::Text("F9  mouse coords output to console", font, fontSz) );
 //   msgVec.push_back( sf::Text("F8  spray mvHits at frame rate while lbuttheld. Cycles through container.", font, fontSz) );
 //   msgVec.push_back( sf::Text("F9  Pause and set all to previous saved state. Repeat to cahcheLimit", font, fontSz) );
    msgVec.push_back( sf::Text("Z   expand/contract expandPolygon in container at start", font, fontSz) );
 //   msgVec.push_back( sf::Text("X   expand/contract last expandPolygon in container at start", font, fontSz) );
 //   msgVec.push_back( sf::Text("C   expand/contract both at once", font, fontSz) );
    msgVec.push_back( sf::Text("R   rotate captured regPolygon type", font, fontSz) );
    msgVec.push_back( sf::Text("F   freeze all mvHits via v = 0", font, fontSz) );
    msgVec.push_back( sf::Text("mouse: Left button = place and fling captured mvHit", font, fontSz) );
 //   msgVec.push_back( sf::Text("mouse: Right button = produce clones of captured mvHit at blazing rate", font, fontSz) );

    for( auto& msg : msgVec )
    {
        msg.setColor( sf::Color::White );
        msg.setPosition( x0, y );
        y += dy;
    }
}

void do_spawn( const mvHit& mh, std::vector<mvHit*>& pMhv )
{
    static vec2d  spawnSep( 40.0f, 0.0f );
    spawnSep = spawnSep.Rotate( 0.3f );
    mvHit* pMH = mh.clone();
    pMhv.push_back( pMH );
    pMH->v = spawnSep*( 0.01f );
    pMH->pos.x = button::mseX + spawnSep.x;
    pMH->pos.y = button::mseY + spawnSep.y;
}

void init_waveControl( waveSeg& wave, float posx, float posy )
{
    sf::Text label("Wave", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    waveControl.init( posx, posy, 70.0f, 25.0f, label );
    waveControl.persist = 3;
    waveControl.ownButts = true;
    posy += 30.0f;

    float wvLen = wave.getWavelengthLt(), wvAmp = wave.Amp_wvLt;
    label.setString("Amplitude left");    // old below
    waveControl.pButtVec.push_back( new buttonValOnHit( posx-125, posy, 150, 22, label, 0.0, 50.0f, wvAmp, std::bind(waveSeg::setAmplitudeLt, &wave, std::placeholders::_1) ) );
    label.setString("Wavelength left");
    waveControl.pButtVec.push_back( new buttonValOnHit( posx-125, posy+40, 150, 22, label, 128.0f, 1280.0f, wvLen, std::bind(waveSeg::setWavelengthLt, &wave, std::placeholders::_1) ) );
    wvLen = wave.getWavelengthRt(), wvAmp = wave.Amp_wvRt;
    label.setString("Amplitude right");
    waveControl.pButtVec.push_back( new buttonValOnHit( posx+105, posy, 150, 22, label, 0.0, 50.0f, wvAmp, std::bind(waveSeg::setAmplitudeRt, &wave, std::placeholders::_1) ) );
    label.setString("Wavelength right");
    waveControl.pButtVec.push_back( new buttonValOnHit( posx+105, posy+40, 150, 22, label, 128.0f, 1280.0f, wvLen, std::bind(waveSeg::setWavelengthRt, &wave, std::placeholders::_1) ) );
}

void init_gunControl( gun& Gun, float posx, float posy )
{
    sf::Text label("GUN", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    gunControl.init( posx, posy, 70.0f, 25.0f, label );
    gunControl.persist = 3;
    gunControl.ownButts = true;
    joyButton* pJoyStk = new joyButton;
    pJoyStk->init( 70.0f, 7.0f, 400.0f, 590.0f );
    pJoyStk->pSel = &(Gun.is_firing);
    pJoyStk->pBoolRt = &(Gun.is_free);
    pJoyStk->pFunc_r = std::bind( gun::setRotation, &Gun, std::placeholders::_1);
    gunControl.pButtVec.push_back( pJoyStk );

    buttonList* pList = new buttonList;
    label.setString("OTHER");
    pList->init( pJoyStk->pos.x - pJoyStk->r_ring, pJoyStk->pos.y + pJoyStk->r_ring - 5.0f, 40.0f, 20.0f, label );
    pList->persist = 3;
    pList->ownButts = true;
    gunControl.pButtVec.push_back( pList );

    radioButton* pRB = new radioButton( pList->pos.x, pList->pos.y - 60.0f, 2.0f*pJoyStk->r_ring, 25.0f, 4, label, 0 );
    label.setString("Fire Mode");
    gun* pGun = &Gun;
    pRB->pFuncIdx = std::bind( [pGun](size_t i) { const int x[] = {0,10,6,3}; pGun->fireDelay = x[i%4]; }, std::placeholders::_1 );
    pRB->pFuncIdx(0);// set initial fire mode
    pList->pButtVec.push_back( pRB );

    label.setString("Bullet Speed");
    label.setColor( sf::Color(0,0,0) );
    pList->pButtVec.push_back( new buttonValOnHit( pList->pos.x, pList->pos.y - 30.0f, 2.0f*pJoyStk->r_ring, 17.0f, label, 0.0f, 80.0f, 40.0f, &(Gun.bulletSpeed) ) );
}

/*
void init_gunControl( gun& Gun, float posx, float posy )
{
    sf::Text label("GUN", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    gunControl.init( posx, posy, 70.0f, 25.0f, label );
    gunControl.persist = 3;
    gunControl.ownButts = true;
    joyButton* pJoyStk = new joyButton;
    pJoyStk->init( 70.0f, 7.0f, 400.0f, 590.0f );
    pJoyStk->pSel = &(Gun.is_firing);
    pJoyStk->pBoolRt = &(Gun.is_free);
    pJoyStk->pFunc_r = std::bind( gun::setRotation, &Gun, std::placeholders::_1);
    gunControl.pButtVec.push_back( pJoyStk );
    label.setString("Bullet Speed");
    label.setColor( sf::Color(0,0,0) );
    gunControl.pButtVec.push_back( new buttonValOnHit( pJoyStk->pos.x - pJoyStk->r_ring, pJoyStk->pos.y + pJoyStk->r_ring - 5.0f, 2.0f*pJoyStk->r_ring, 17.0f, label, 0.0f, 80.0f, 40.0f, &(Gun.bulletSpeed) ) );
}
*/

void init_recorderControl( float posx, float posy )
{
    std::vector<sf::Text> TitleVec;
    std::vector<std::string>* p_strTitleVec = nullptr;
    std::vector< std::function<void(void)> >* p_pFuncVec = nullptr;
    rec::initControl( p_strTitleVec, p_pFuncVec, 40 );
    if( p_strTitleVec && p_pFuncVec )
    {
        TitleVec.reserve( p_strTitleVec->size() );
        for( auto& str : *p_strTitleVec )
        {
            TitleVec.push_back( sf::Text( str.c_str(), *button::pFont, 10) );
            TitleVec.back().setColor( sf::Color::Black );
        }

        recorderControl.init( posx, posy, 70.0f, 25.0f, 25.0f, TitleVec, *p_pFuncVec );
        recorderControl.persist = 3;
    }
}
