#include "controlUtils.h"

namespace ctUtil
{
 //   buttonList list_1, list_2;
 //   std::vector<buttonRect> rectButtVec;
    sf::Text recMsgTxt[2], recMsgNum[2];// display base index and frame index for recorder
}

void ctUtil::init_superList( buttonList& superList, float posx, float posy )
{

    if( !button::pFont ) { std::cerr << "\n ctUtil says no pFont no good!"; return; }
    const sf::Font& font = *button::pFont;

    float W = 60.0f, H = 25.0f;
    std::vector< std::function<void(void)> > pFuncVec;
    std::vector<sf::Text> TitleVec;

    // make list_2 all rectButts
    sf::Text txt( "list 2", font, 10);
    txt.setColor( sf::Color(0,0,0) );
    TitleVec.push_back( txt ); pFuncVec.push_back( std::bind( [](void) { std::cout << "\n list 2 dn"; } ) );
    buttonList* pSL = &superList;
    txt.setString("2A"); TitleVec.push_back(txt); pFuncVec.push_back( std::bind( [pSL](void) { std::cout << "\n 2A"; pSL->closeList(); } ) );
    txt.setString("2B"); TitleVec.push_back(txt); pFuncVec.push_back( std::bind( [](void) { std::cout << "\n 2B"; } ) );
    txt.setString("2C"); TitleVec.push_back(txt); pFuncVec.push_back( std::bind( [](void) { std::cout << "\n 2C"; } ) );
    txt.setString("2D"); TitleVec.push_back(txt); pFuncVec.push_back( std::bind( [](void) { std::cout << "\n 2D"; } ) );
    buttonList* pList_2 = new buttonList;
    pList_2->pHitFunc_up = std::bind( [](void) { std::cout << "\n list 2 up"; } );
    pList_2->init( posx+W, posy+4.0f*H, W, H, H, TitleVec, pFuncVec, 'R' );
    pList_2->persist = 1;
    pList_2->ownButts = true;// and per 1st line in init() above
    pList_2->openOnRight = true;// and per 1st line in init() above

    // make list_1 = 2rb + list_2 + rb
 //   TitleVec.clear(); pFuncVec.clear();
    txt.setString("list 1");
    buttonList* pList_1 = new buttonList;
    pList_1->init( posx, posy+3.0f*H, W, H, txt );// homeButt
    pList_1->pHitFunc = std::bind( [](void) { std::cout << "\n list 1 dn"; } );
    pList_1->pHitFunc_up = std::bind( [](void) { std::cout << "\n list 1 up"; } );
    buttonRect* pButt = new buttonRect;
    txt.setString("1A"); pButt->init( posx+W, posy+3.0f*H, W, H, txt ); pButt->pHitFunc = std::bind( [](void) { std::cout << "\n 1A"; } );
    pList_1->pButtVec.push_back( pButt );
    pList_1->pButtVec.push_back( pList_2 );
    pButt = new buttonRect;
    txt.setString("1C"); pButt->init( posx+W, posy+5.0f*H, W, H, txt ); pButt->pHitFunc = std::bind( [](void) { std::cout << "\n 1C"; } );
    pList_1->pButtVec.push_back( pButt );
    pButt = new buttonRect;
    txt.setString("1D"); pButt->init( posx+W, posy+6.0f*H, W, H, txt ); pButt->pHitFunc = std::bind( [](void) { std::cout << "\n 1D"; } );
    pList_1->pButtVec.push_back( pButt );
    pList_1->persist = 3;
    pList_1->ownButts = true;
    pList_1->openOnRight = true;

    // superList
    txt.setString("SUPER");
    superList.init( posx, posy, W, H, txt );// homeButt
    superList.pHitFunc = std::bind( [](void) { std::cout << "\n SUPER"; } );
    pButt = new buttonRect;
    txt.setString("SA"); pButt->init( posx, posy+H, W, H, txt ); pButt->pHitFunc = std::bind( [](void) { std::cout << "\n SA"; } );
    superList.pButtVec.push_back( pButt );
    pButt = new buttonRect;
    txt.setString("SB"); pButt->init( posx, posy+2.0f*H, W, H, txt ); pButt->pHitFunc = std::bind( [](void) { std::cout << "\n SB"; } );
    superList.pButtVec.push_back( pButt );

    superList.pButtVec.push_back( pList_1 );// +3.0f*H
    pButt = new buttonRect;
    txt.setString("SD"); pButt->init( posx, posy+4.0f*H, W, H, txt ); pButt->pHitFunc = std::bind( [](void) { std::cout << "\n SD"; } );
    superList.pButtVec.push_back( pButt );

    superList.persist = 3;
    superList.ownButts = true;
 //   superList.openOnRight = true;
}

void ctUtil::init_textList( buttonList& textList, float posx, float posy )
{
    sf::Text txt( "text", *button::pFont, 10);
    txt.setColor( sf::Color(0,0,0) );
    float wButt = 70.0f, hButt = 25.0f;
 //   waveControlList.init( posx, posy, 70.0f, 25.0f, txt );
    textList.init( posx, posy, wButt, hButt, txt );

    posy += hButt;
    txt.setString("text button 1");
    hButt = 30.0f; wButt = 150.0f;
    textButton* pTB = new textButton( posx, posy, wButt, hButt, txt );
    pTB->pStrFunc = std::bind( [](const char* str) { std::cout << "\n#1 " << str; }, std::placeholders::_1 );
    textList.pButtVec.push_back( pTB );
    txt.setString("text button 2");
    pTB = new textButton( posx, posy+=hButt + 10.0f, wButt, hButt, txt );
    pTB->pStrFunc = std::bind( [](const char* str) { std::cout << "\n#2 " << str; }, std::placeholders::_1 );
    textList.pButtVec.push_back( pTB );
    txt.setString("text button 3");
    pTB = new textButton( posx, posy+=hButt + 10.0f, wButt, hButt, txt );
    pTB->pStrFunc = std::bind( [](const char* str) { std::cout  << "\n#3 " << str; }, std::placeholders::_1 );
    textList.pButtVec.push_back( pTB );

    textList.persist = 3;
    textList.ownButts = true;
}

void ctUtil::init_flipperControl( buttonList& flipControl, lineSegFlip& LSflip, float numReps )
{
    sf::Text label("FL", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    float buttDim = 25.0f;

    lineSegFlip* pLsFlip = &LSflip;
    flipControl.init( pLsFlip->pos.x - buttDim, pLsFlip->pos.y - buttDim, buttDim, buttDim, label );
    flipControl.mode = 'I';
    flipControl.pHitFunc = [pLsFlip](void) { pLsFlip->init_up(); };
    flipControl.pHitFunc_up = [pLsFlip](void) { pLsFlip->init_dn(); };
    flipControl.persist = 3;

    flipControl.ownButts = true;
    flipControl.openOnRight = true;

    label.setString("speed");
    pLsFlip->rotVelFlip /= (float)numReps;
    std::function<void(float)> p_SetFunc = [ pLsFlip ](float x)
    {
        if( pLsFlip->rotVelFlip < 0.0f ) pLsFlip->rotVelFlip = -x;
        else pLsFlip->rotVelFlip = x;
    };
    float flipVelMag = pLsFlip->rotVelFlip < 0.0f ? -pLsFlip->rotVelFlip : pLsFlip->rotVelFlip;// positive value only
    flipControl.pButtVec.push_back( new buttonValOnHit( flipControl.pos.x + buttDim + 10.0f, flipControl.pos.y, 150, 20, label, 0.01f/(float)numReps, 0.2f/(float)numReps, flipVelMag, p_SetFunc ) );

    label.setString("position up");
 //   float dnAngle = -acosf( pLsFlip->Ldn.dot(pLsFlip->Lup)/(pLsFlip->len*pLsFlip->len) );
    float dnAngle = asinf( pLsFlip->Ldn.cross(pLsFlip->Lup)/(pLsFlip->len*pLsFlip->len) );
    p_SetFunc = [ pLsFlip ](float x)
    {
      pLsFlip->Lup = pLsFlip->Ldn.Rotate(x);
      if( pLsFlip->Ldn.cross(pLsFlip->Lup) < 0.0f ) { if( pLsFlip->rotVelFlip < 0.0f ) pLsFlip->rotVelFlip *= -1.0f; }
      else if( pLsFlip->rotVelFlip > 0.0f ) pLsFlip->rotVelFlip *= -1.0f;
      pLsFlip->set_up();
    };//, std::placeholders::_1 );
    buttonValOnHit* pCSup = new buttonValOnHit( flipControl.pos.x + buttDim + 10.0f, flipControl.pos.y+35.0f, 150, 20, label, -3.1416f, 3.1416f, dnAngle, p_SetFunc );
    flipControl.pButtVec.push_back( pCSup );

    label.setString("position down");
    dnAngle = atan2f( pLsFlip->Ldn.y, pLsFlip->Ldn.x );
    p_SetFunc = std::bind( [ pLsFlip ](float x)
    {
        float dAng = asinf( pLsFlip->Ldn.cross(pLsFlip->Lup)/(pLsFlip->len*pLsFlip->len) );
        pLsFlip->Ldn.x = pLsFlip->len*cosf(x);
        pLsFlip->Ldn.y = pLsFlip->len*sinf(x);
        pLsFlip->Lup = pLsFlip->Ldn.Rotate( dAng );
        pLsFlip->set_dn();
    }, std::placeholders::_1 );
    flipControl.pButtVec.push_back( new buttonValOnHit( flipControl.pos.x + buttDim + 10.0f, flipControl.pos.y+70.0f, 150, 20, label, 0.0f, 3.1416f, dnAngle, p_SetFunc ) );

    label.setString("Length");
    p_SetFunc = std::bind( [ pLsFlip ](float x)
    {
        pLsFlip->Ldn /= pLsFlip->len; pLsFlip->Ldn *= x;
        pLsFlip->Lup /= pLsFlip->len; pLsFlip->Lup *= x;
        pLsFlip->L   /= pLsFlip->len; pLsFlip->L *= x;
        pLsFlip->len = pLsFlip->L.mag();
        pLsFlip->vtx[1].position = Equate( pLsFlip->pos + pLsFlip->L );// update visual

    }, std::placeholders::_1 );
    flipControl.pButtVec.push_back( new buttonValOnHit( flipControl.pos.x + buttDim + 10.0f, flipControl.pos.y+105.0f, 150, 20, label, 50.0f, 300.0f, pLsFlip->len, p_SetFunc ) );

}

void ctUtil::init_elevatorControl( buttonList& elevControl, lineSegElevator& LSelev, float posx, float posy, float numReps )
{
    sf::Text label("elev", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    float W = 50.0f, H = 25.0f;
    elevControl.init( posx, posy, W, H, label );
    elevControl.persist = 3;
    elevControl.ownButts = true;// and per 1st line in init() above
    label.setString("UP");
    lineSegElevator* pLsElev = &LSelev;
    elevControl.pButtVec.push_back( new buttonRect( posx, posy+H, W, H, std::bind( lineSegElevator::init_up, pLsElev ), label ) );
    elevControl.pButtVec.back()->mode = 'I';
    label.setString("STOP");
    elevControl.pButtVec.push_back( new buttonRect( posx, posy+2.0f*H, W, H, std::bind( lineSegElevator::stop, pLsElev ), label ) );
    elevControl.pButtVec.back()->mode = 'I';
    label.setString("DOWN");
    elevControl.pButtVec.push_back( new buttonRect( posx, posy+3.0f*H, W, H, std::bind( lineSegElevator::init_dn, pLsElev ), label ) );
    elevControl.pButtVec.back()->mode = 'I';
    label.setString("speed"); pLsElev->magVel /= (float)numReps;
    elevControl.pButtVec.push_back( new buttonValOnHit( posx-25.0f, posy+4.5f*H, 100, 20, label, 1.0f/(float)numReps, 10.0f/(float)numReps, pLsElev->magVel, &(pLsElev->magVel) ) );
}

joyButton* ctUtil::init_gunControl( buttonList& gunControl, gun& Gun, float posx, float posy, float iVel )//, bool drive, float driveVel )
{
    sf::Text label("GUN", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    gunControl.init( posx, posy, 70.0f, 25.0f, label );
    gunControl.persist = 3;
    gunControl.ownButts = true;
    joyButton* pJoyStk = new joyButton;
    pJoyStk->init( 55.0f, 6.0f, posx + 170.0f, posy + 50.0f );
    pJoyStk->pSel = &(Gun.is_firing);
    pJoyStk->pBoolRt = &(Gun.is_free);
    pJoyStk->pFunc_r = std::bind( gun::setRotation, &Gun, std::placeholders::_1);
    gun* pGun = &Gun;
  //  if( drive ) pJoyStk->pFunc_v2d = std::bind( [pGun,driveVel](vec2d r) { if( pGun->is_free ) pGun->v = driveVel*r; }, std::placeholders::_1 );
    gunControl.pButtVec.push_back( pJoyStk );

    buttonList* pList = new buttonList;
    label.setString("other");
    pList->init( pJoyStk->pos.x - 20.0f, pJoyStk->pos.y + pJoyStk->r_ring + 5.0f, 40.0f, 20.0f, label );
    pList->persist = 3;
    pList->ownButts = true;
    gunControl.pButtVec.push_back( pList );

 //   radioButton* pRB = new radioButton( pList->pos.x, pList->pos.y - 60.0f, 2.0f*pJoyStk->r_ring, 20.0f, 4, label, 2 );
    label.setString("Fire Mode");
    label.setColor( sf::Color(255,0,0) );
    radioButton* pRB = new radioButton( pJoyStk->pos.x - pJoyStk->r_ring, pList->pos.y + 30.0f, 2.0f*pJoyStk->r_ring, 17.0f, 4, label, 2 );
 //   label.setString("Fire Mode");
 //   gun* pGun = &Gun;
    pRB->pFuncIdx = std::bind( [pGun](size_t i) { const int x[] = {0,10,6,3}; pGun->fireDelay = x[i%4]; }, std::placeholders::_1 );
    pRB->pFuncIdx(2);// set initial fire mode
    pList->pButtVec.push_back( pRB );

    label.setString("Bullet Speed");
    label.setColor( sf::Color(0,0,0) );
    pList->pButtVec.push_back( new buttonValOnHit( pJoyStk->pos.x - pJoyStk->r_ring, pList->pos.y + 60.0f, 2.0f*pJoyStk->r_ring, 17.0f, label, 0.0f, 100.0f, iVel, &(Gun.bulletSpeed) ) );

    return pJoyStk;
}

void ctUtil::init_waveControl( buttonList& waveControl, waveSeg& wave, float posx, float posy, char type )
{
    sf::Text label("Wave", *button::pFont, 12);
    label.setColor( sf::Color(0,0,0) );
    waveControl.init( posx, posy, 70.0f, 25.0f, label );
    waveControl.persist = 3;
    waveControl.ownButts = true;
    posy += 30.0f;

    float wvLen = wave.getWavelengthLt(), wvAmp = wave.Amp_wvLt;
    label.setString("Amplitude left");    // old below
    if( type == 'c' ) waveControl.pButtVec.push_back( new buttonValOnHit( posx-125, posy, 150, 22, label, 0.0, 50.0f, wvAmp, std::bind(waveSeg::setAmplitudeLt, &wave, std::placeholders::_1) ) );
    else waveControl.pButtVec.push_back( new slideBar( posx-125, posy+10, 150, 20, label, 0.0f, 50.0f, wvAmp, std::bind(waveSeg::setAmplitudeLt, &wave, std::placeholders::_1) ) );
    label.setString("Wavelength left");
    if( type == 'c' ) waveControl.pButtVec.push_back( new buttonValOnHit( posx-125, posy+40, 150, 22, label, 128.0f, 1280.0f, wvLen, std::bind(waveSeg::setWavelengthLt, &wave, std::placeholders::_1) ) );
    else waveControl.pButtVec.push_back( new slideBar( posx-125, posy+50, 150, 20, label, 128.0f, 1280.0f, wvLen, std::bind(waveSeg::setWavelengthLt, &wave, std::placeholders::_1) ) );
    wvLen = wave.getWavelengthRt(), wvAmp = wave.Amp_wvRt;
    label.setString("Amplitude right");
    if( type == 'c' ) waveControl.pButtVec.push_back( new buttonValOnHit( posx+105, posy, 150, 22, label, 0.0, 50.0f, wvAmp, std::bind(waveSeg::setAmplitudeRt, &wave, std::placeholders::_1) ) );
    else waveControl.pButtVec.push_back( new slideBar( posx+85, posy+10, 150, 20, label, 0.0, 50.0f, wvAmp, std::bind(waveSeg::setAmplitudeRt, &wave, std::placeholders::_1) ) );
    label.setString("Wavelength right");
    if( type == 'c' ) waveControl.pButtVec.push_back( new buttonValOnHit( posx+105, posy+40, 150, 22, label, 128.0f, 1280.0f, wvLen, std::bind(waveSeg::setWavelengthRt, &wave, std::placeholders::_1) ) );
    else waveControl.pButtVec.push_back( new slideBar( posx+85, posy+50, 150, 20, label, 128.0f, 1280.0f, wvLen, std::bind(waveSeg::setWavelengthRt, &wave, std::placeholders::_1) ) );
}

void ctUtil::init_recorderControl( buttonList& recorderControl, float posx, float posy )
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
     //   recorderControl.mode = 'I';
     //   recorderControl.pHitFunc_up = nullptr;//recorderControl.pHitFunc;
    }
}

void ctUtil::init_recorderControl( buttonList& recorderControl, float posx, float posy, float msgOfstX, float msgOfstY )// call above after assign msg positions, etc.
{
    if( button::pFont )
    {
        recMsgTxt[0].setColor( sf::Color::Cyan ); recMsgTxt[0].setFont( *button::pFont ); recMsgTxt[0].setCharacterSize( 12 );
        recMsgTxt[1].setColor( sf::Color::Cyan ); recMsgTxt[1].setFont( *button::pFont ); recMsgTxt[1].setCharacterSize( 12 );
        recMsgNum[0].setColor( sf::Color::White ); recMsgNum[0].setFont( *button::pFont ); recMsgNum[0].setCharacterSize( 12 );
        recMsgNum[1].setColor( sf::Color::White ); recMsgNum[1].setFont( *button::pFont ); recMsgNum[1].setCharacterSize( 12 );

        recMsgTxt[0].setString( "base" ); recMsgTxt[0].setPosition( posx + msgOfstX, posy + msgOfstY );
        recMsgTxt[1].setString( "frame" ); recMsgTxt[1].setPosition( posx + msgOfstX, posy + msgOfstY + 40.0f );
        recMsgNum[0].setString( "0" ); recMsgNum[0].setPosition( posx + msgOfstX, posy + msgOfstY + 20.0f );
        recMsgNum[1].setString( "0" ); recMsgNum[1].setPosition( posx + msgOfstX, posy + msgOfstY + 60.0f );
    }

    init_recorderControl( recorderControl, posx, posy );
}

void ctUtil::updateRecorderDisplay()
{
    static size_t oldIndex = 0, oldCnt = 0;
    if( rec::is_running() )
    if( rec::get_baseIndex() != oldIndex )
    {
        std::stringstream ss;
        ss << rec::get_baseIndex();
        recMsgNum[0].setString( ss.str().c_str() );
        oldIndex = rec::get_baseIndex();
    }

    if( rec::get_frameCnt() != oldCnt )
    {
        std::stringstream ss;
        ss << rec::get_frameCnt();
        recMsgNum[1].setString( ss.str().c_str() );
        oldCnt = rec::get_frameCnt();
    }
}

void ctUtil::drawRecorderDisplay( sf::RenderTarget& RT )
{
     RT.draw( recMsgTxt[0] );
    RT.draw( recMsgTxt[1] );
    RT.draw( recMsgNum[0] );
    RT.draw( recMsgNum[1] );
}

int ctUtil::addButtonState( button& rButt, std::vector<state_ab*>& p_stateVec )// returns # of states added
{
    slideBar* pSB = dynamic_cast<slideBar*>( &rButt );
    if( pSB )
    {
        p_stateVec.push_back( new state_f( &(pSB->fVal), std::bind( slideBar::set_fVal, pSB, std::placeholders::_1) ) );
    //    std::cout << "added slideBar\n";
        return 1;
    }

    buttonValOnHit* pVoH = dynamic_cast<buttonValOnHit*>( &rButt );
    if( pVoH )
    {
        p_stateVec.push_back( new state_f( &(pVoH->fVal), std::bind( buttonValOnHit::set_fVal, pVoH, std::placeholders::_1) ) );
    //    std::cout << "added click strip\n";
        return 1;
    }

    floatSpot* pFS = dynamic_cast<floatSpot*>( &rButt );
    if( pFS )
    {
        p_stateVec.push_back( new state_f( &(pFS->voh.fVal), std::bind( buttonValOnHit::set_fVal, &(pFS->voh), std::placeholders::_1) ) );
     //   std::cout << "added click strip in floatSpot\n";
        return 1;
    }

    radioButton* pRB = dynamic_cast<radioButton*>( &rButt );
    if( pRB )
    {
    //    p_stateVec.push_back( pRB->newState() );
        p_stateVec.push_back( new state_fn( std::bind( &radioButton::getState, pRB, std::placeholders::_1 ), std::bind( &radioButton::setState, pRB, std::placeholders::_1 ), 1 ) );
        std::cout << "added radioButton\n";
        return 1;
    }

    buttonList* pBL = dynamic_cast<buttonList*>( &rButt );
    if( pBL )
    {
        int retVal = 0;
        for( auto& pButt : pBL->pButtVec )
            retVal += addButtonState( *pButt, p_stateVec );
        return retVal;
    }

    return 0;
}

/*
void ctUtil::init_superList()
{

    if( !button::pFont ) { std::cerr << "\n ctUtil says no pFont no good!"; return; }
    const sf::Font& font = *button::pFont;

    float posx = 1000.0f, posy = 100.0f, W = 60.0f, H = 25.0f;
    std::vector< std::function<void(void)> > pFuncVec;
    std::vector<sf::Text> TitleVec;
    rectButtVec.reserve(20);

    // make list_2 all rectButts
    sf::Text txt( "list 2", font, 10);
    txt.setColor( sf::Color(0,0,0) );
    TitleVec.push_back( txt ); pFuncVec.push_back( std::bind( [](void) { std::cout << "\n list 2"; } ) );
    txt.setString("2A"); TitleVec.push_back(txt); pFuncVec.push_back( std::bind( [](void) { std::cout << "\n 2A"; } ) );
    txt.setString("2B"); TitleVec.push_back(txt); pFuncVec.push_back( std::bind( [](void) { std::cout << "\n 2B"; } ) );
    txt.setString("2C"); TitleVec.push_back(txt); pFuncVec.push_back( std::bind( [](void) { std::cout << "\n 2C"; } ) );
    txt.setString("2D"); TitleVec.push_back(txt); pFuncVec.push_back( std::bind( [](void) { std::cout << "\n 2D"; } ) );
    list_2.init( posx+W, posy+4.0f*H, W, H, H, TitleVec, pFuncVec, rectButtVec, 'R' );
    list_2.persist = 3;

    // make list_1 = 2rb + list_2 + rb
 //   TitleVec.clear(); pFuncVec.clear();
    txt.setString("list 1");
    list_1.init( posx, posy+3.0f*H, W, H, txt );// homeButt
    list_1.pHitFunc = std::bind( [](void) { std::cout << "\n list 1"; } );
    buttonRect tmpButt;
    txt.setString("1A"); tmpButt.init( posx+W, posy+3.0f*H, W, H, txt ); tmpButt.pHitFunc = std::bind( [](void) { std::cout << "\n 1A"; } );
    rectButtVec.push_back( tmpButt );
    list_1.pButtVec.push_back( &rectButtVec.back() );
    list_1.pButtVec.push_back( &list_2 );
    txt.setString("1C"); tmpButt.init( posx+W, posy+5.0f*H, W, H, txt ); tmpButt.pHitFunc = std::bind( [](void) { std::cout << "\n 1C"; } );
    rectButtVec.push_back( tmpButt );
    list_1.pButtVec.push_back( &rectButtVec.back() );
    txt.setString("1D"); tmpButt.init( posx+W, posy+6.0f*H, W, H, txt ); tmpButt.pHitFunc = std::bind( [](void) { std::cout << "\n 1D"; } );
    rectButtVec.push_back( tmpButt );
    list_1.pButtVec.push_back( &rectButtVec.back() );
    list_1.persist = 3;

    // superList
    txt.setString("SUPER");
    superList.init( posx, posy, W, H, txt );// homeButt
    superList.pHitFunc = std::bind( [](void) { std::cout << "\n SUPER"; } );
    txt.setString("SA"); tmpButt.init( posx, posy+H, W, H, txt ); tmpButt.pHitFunc = std::bind( [](void) { std::cout << "\n SA"; } );
    rectButtVec.push_back( tmpButt );
    superList.pButtVec.push_back( &rectButtVec.back() );
    txt.setString("SB"); tmpButt.init( posx, posy+2.0f*H, W, H, txt ); tmpButt.pHitFunc = std::bind( [](void) { std::cout << "\n SB"; } );
    rectButtVec.push_back( tmpButt );
    superList.pButtVec.push_back( &rectButtVec.back() );
    superList.pButtVec.push_back( &list_1 );// +3.0f*H
    txt.setString("SD"); tmpButt.init( posx, posy+4.0f*H, W, H, txt ); tmpButt.pHitFunc = std::bind( [](void) { std::cout << "\n SD"; } );
    rectButtVec.push_back( tmpButt );
    superList.pButtVec.push_back( &rectButtVec.back() );
    superList.persist = 3;
}
*/
