#include "lvl_CGOL.h"

bool lvl_CGOL::init( )
{

   std::ifstream fin("include/level/lvl_CGOL_data/seed_catagories.txt");
    if( !fin ) { std::cout << "no init_data\n"; return false; }

    fin >> wCell >> hCell;
    fin >> genPeriod; genTime = 0;
    std::cout << "\ngenPeriod = " << genPeriod;
//    cellRect.setSize( sf::Vector2f(wCell,hCell) );
 //   cellRect.setFillColor( sf::Color::Blue );

    fin >> rows >> cols;
    left = top = 0;// bounds of area containing live cells
    right = cols-1, bottom = rows-1;// initially entire field
    stA.reserve( rows );
    for( size_t i=0; i<rows; ++i )
        stA.push_back( std::vector<bool>(cols,false) );

    stB = stA;

    std::string seedCatag;

    sf::Text txt( "SEEDS", *button::pFont, 10);
    txt.setColor( sf::Color(0,0,0) );
    gameList.init( 300.0f, 20.0f, 70.0f, 30.0f, txt );

  //      float posY = gameList.pos.y;
    float dy = gameList.sz.y + 4.0f;
    float i = 0.0f;
    while( fin >> seedCatag )// add a catagory list list
    {
        ++i;
        txt.setString( seedCatag.c_str() );
        buttonList* pBL = new buttonList;//( gameList.pos.x, gameList.pos.y+(float)(i+1)*dy, gameList.sz.x, gameList.sz.y, txt );
        pBL->init( gameList.pos.x, gameList.pos.y+ i*dy, gameList.sz.x, gameList.sz.y, txt );
        pBL->pHitFunc = nullptr;
        pBL->ownButts = true;
        pBL->persist = 2;
   //     pBL->openOnHover = true;
        gameList.pButtVec.push_back( pBL );

        std::string listName( "include/level/lvl_CGOL_data/" + seedCatag + '/' + seedCatag + "_list.txt" );
        std::ifstream finList( listName.c_str() );
        std::cout << "\nlist: " << listName;
        float j = 0.0f;
        while( finList >> listName )// list items
        {
            std::cout << '\n' << listName;
            txt.setString( listName.c_str() );
            buttonRect* pBR = new buttonRect;//( gameList.pos.x, gameList.pos.y+(float)(i+1)*dy, gameList.sz.x, gameList.sz.y, txt );
            pBR->init( gameList.pos.x + gameList.sz.x + 5.0f, pBL->pos.y + j*dy, gameList.sz.x, gameList.sz.y, txt );
            std::string itemFileName( seedCatag + '/' + listName );
         //   pBR->pHitFunc = [ this, itemFileName ](void) { loadGame( itemFileName ); gameList.closeList(); };
//            pBR->pHitFunc = [ this, itemFileName ](void) { loadGame( itemFileName ); gameList.closeList(); left=0; right=cols-1; top=0; bottom=rows-1; };
            pBR->pHitFunc = [ this, itemFileName ](void) { loadGame( itemFileName ); gameList.closeList(); };
            pBL->pButtVec.push_back( pBR );
            ++j;
        }
    }

    gameList.persist = 3;
    gameList.ownButts = true;
    button::RegisteredButtVec.push_back( &gameList );

    // period
    txt.setString("gen period");
    std::function<void(float)> p_SetFunc = std::bind( [this](float x)
    { genPeriod = x; genTime = 0.0f; }, std::placeholders::_1 );
    genPeriodStrip.init( 50, 20, 150, 22, txt, 2.0, 40.0, genPeriod, p_SetFunc );
    button::RegisteredButtVec.push_back( &genPeriodStrip );

    // text messages
    if( button::pFont )
    {
        genCntMsg.setFont( *button::pFont );
        genCntMsg.setCharacterSize( 16 );
        genCntMsg.setColor( sf::Color::White );
        genCntMsg.setPosition( sf::Vector2f( 400.0f, 20.0f ) );
        genCntMsg.setString( "gen #:" );

        genCntMsgNum.setFont( *button::pFont );
        genCntMsgNum.setCharacterSize( 16 );
        genCntMsgNum.setColor( sf::Color::White );
        genCntMsgNum.setPosition( sf::Vector2f( 470.0f, 20.0f ) );
        genCntMsgNum.setString( "0" );
    }

    Level::goto_MMButt.setPosition( {1100.0f,30.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );

    for( auto& vtx : liveBox ) vtx.color = sf::Color::Red;

    return true;
}

/*
bool lvl_CGOL::init( )
{

   std::ifstream fin("include/level/lvl_CGOL_data/init_data.txt");
    if( !fin ) { std::cout << "no init_data\n"; return false; }

    fin >> wCell >> hCell;
    fin >> genPeriod; genTime = 0;
    std::cout << "\ngenPeriod = " << genPeriod;
//    cellRect.setSize( sf::Vector2f(wCell,hCell) );
 //   cellRect.setFillColor( sf::Color::Blue );

    fin >> rows >> cols;
    stA.reserve( rows );
    for( size_t i=0; i<rows; ++i )
        stA.push_back( std::vector<bool>(cols,false) );

    stB = stA;

    std::string gameName;
    fin >> gameName;

    if( loadGame( gameName ) )
    {
        sf::Text txt( gameName.c_str(), *button::pFont, 10);
        txt.setColor( sf::Color(0,0,0) );
        gameList.init( 300.0f, 20.0f, 70.0f, 30.0f, txt );

        while( fin >> gameName ) gameFileNameVec.push_back( gameName );
        std::cout << "\n game names = " << gameFileNameVec.size();

  //      float posY = gameList.pos.y;
        float dy = gameList.sz.y + 4.0f;
        for( size_t i = 0; i < gameFileNameVec.size(); ++i )// add a button to the list
        {
            txt.setString( gameFileNameVec[i].c_str() );
            buttonRect* pBR = new buttonRect;//( gameList.pos.x, gameList.pos.y+(float)(i+1)*dy, gameList.sz.x, gameList.sz.y, txt );
            pBR->init( gameList.pos.x, gameList.pos.y+(float)(i+1)*dy, gameList.sz.x, gameList.sz.y, txt );
            const std::string* pCstr = &( gameFileNameVec[i] );
            pBR->pHitFunc = std::bind( [ this, pCstr ](void) { loadGame( *pCstr ); gameList.title.setString( pCstr->c_str() ); } );
            gameList.pButtVec.push_back( pBR );
        }

        gameList.persist = 2;
        gameList.ownButts = true;
        button::RegisteredButtVec.push_back( &gameList );

        // period
        txt.setString("gen period");
        std::function<void(float)> p_SetFunc = std::bind( [this](float x)
        { genPeriod = x; genTime = 0.0f; }, std::placeholders::_1 );
        genPeriodStrip.init( 50, 20, 150, 22, txt, 2.0, 40.0, genPeriod, p_SetFunc );
        button::RegisteredButtVec.push_back( &genPeriodStrip );

        // text messages
        if( button::pFont )
        {
            genCntMsg.setFont( *button::pFont );
            genCntMsg.setCharacterSize( 16 );
            genCntMsg.setColor( sf::Color::White );
            genCntMsg.setPosition( sf::Vector2f( 400.0f, 20.0f ) );
            genCntMsg.setString( "gen #:" );

            genCntMsgNum.setFont( *button::pFont );
            genCntMsgNum.setCharacterSize( 16 );
            genCntMsgNum.setColor( sf::Color::White );
            genCntMsgNum.setPosition( sf::Vector2f( 470.0f, 20.0f ) );
            genCntMsgNum.setString( "0" );
        }


        return true;
    }

    return false;
}
*/

bool lvl_CGOL::handleEvent( sf::Event& rEvent )
{
    if (rEvent.type == sf::Event::MouseButtonPressed)
    {
        if (rEvent.mouseButton.button == sf::Mouse::Left)
        {
            // find cell clicked on and toggle state
            if( button::pButtMse ) return true;
            int r = (int)(button::mseY/hCell), c = (int)(button::mseX/wCell);
            if( ( r >= 0 && r < (int)stA.size() ) && ( c >= 0 && r < (int)stA[r].size() ) )
            {
                stA[r][c] = !stA[r][c];// toggle value
                if( stA[r][c] ) anyAlive = true;
                if( r < top+1 ) top = r-1;
                if( r > bottom-1 ) bottom = r+1;
                if( c < left+1 ) left = c-1;
                if( c > right-1 ) right = c+1;
            }
        }
    }

    return true;
}

void lvl_CGOL::update( float dt )
{
    if( !anyAlive ) return;
    genTime+=dt;
    if( genTime >= genPeriod || Level::step_update ){ genTime = 0.0f; }
    else return;

    ++genCount;
    to_SF_string( genCntMsgNum, genCount );
    anyAlive = false;

    const int dr[8] = { -1, -1, -1,  0, 0,  1, 1, 1 };
    const int dc[8] = { -1,  0,  1, -1, 1, -1, 0, 1 };

    int newLeft = cols, newRight = 0, newTop = rows, newBottom = 0;// max initial vals in search for new min values
    for( int r = top; r < (int)stA.size() && r <= bottom; ++r )
    for( int c = left; c < (int)stA[r].size() && c <= right; ++c )
    {
        int liveNborCnt = 0;
            for( int i=0; i<8; ++i )
                if( r+dr[i] >= 0 && r+dr[i] < (int)rows && c+dc[i] >= 0 && c+dc[i] < (int)cols && stA[r+dr[i]][c+dc[i]] ) ++liveNborCnt;

        if( stA[r][c] )
        {
            if( liveNborCnt < 2 ) stB[r][c] = false;// cell dies
            else if( liveNborCnt > 3 ) stB[r][c] = false;// cell dies
            else// cell lives on
            {
                stB[r][c] = true; anyAlive = true;
                if( r < newTop ) newTop = r;
                if( r > newBottom ) newBottom = r;
                if( c < newLeft ) newLeft = c;
                if( c > newRight ) newRight = c;
            }
        }
        else// dead cell
        {
            if( liveNborCnt == 3 )// comes alive
            {
                stB[r][c] = true; anyAlive = true;
                if( r < newTop ) newTop = r;
                if( r > newBottom ) newBottom = r;
                if( c < newLeft ) newLeft = c;
                if( c > newRight ) newRight = c;
            }// cell is reborn
            else stB[r][c] = false;
        }
    }
    /*    if( stA[r][c] )
        {
            int liveNborCnt = 0;
            for( int i=0; i<8; ++i )
                if( r+dr[i] >= 0 && r+dr[i] < (int)rows && c+dc[i] >= 0 && c+dc[i] < (int)cols && stA[r+dr[i]][c+dc[i]] ) ++liveNborCnt;

            if( liveNborCnt < 2 ) stB[r][c] = false;// cell dies
            else if( liveNborCnt > 3 ) stB[r][c] = false;// cell dies
            else
            {
                stB[r][c] = true; anyAlive = true;
                if( r < newTop ) newTop = r;
                if( r > newBottom ) newBottom = r;
                if( c < newLeft ) newLeft = c;
                if( c > newRight ) newRight = c;
            }
        }
        else// dead cell
        {
            int liveNborCnt = 0;
            for( int i=0; i<8; ++i )
                if( r+dr[i] >= 0 && r+dr[i] < (int)rows && c+dc[i] >= 0 && c+dc[i] < (int)cols && stA[r+dr[i]][c+dc[i]] ) ++liveNborCnt;

            if( liveNborCnt == 3 )
            {
                stB[r][c] = true; anyAlive = true;
                if( r < newTop ) newTop = r;
                if( r > newBottom ) newBottom = r;
                if( c < newLeft ) newLeft = c;
                if( c > newRight ) newRight = c;
            }// cell is reborn
            else stB[r][c] = false;
        }   */

//    std::cout << "\n # cells updated = " << (right-left+1)*(bottom-top+1);

    stA = stB;
    // expand area by 1 each way to include perimeter of dead cells around live area
    top = newTop < 1 ? 0 : newTop - 1;
    bottom = newBottom+1 < rows ? newBottom + 1 : rows-1;
    left = newLeft < 1 ? 0 : newLeft - 1;
    right = newRight+1 < cols ? newRight + 1 : cols-1;

    liveBox[0].position.x = left*wCell;  liveBox[0].position.y = top*hCell;// up lt
    liveBox[1].position.x = (right+1)*wCell; liveBox[1].position.y = top*hCell;// up rt
    liveBox[2].position.x = (right+1)*wCell; liveBox[2].position.y = (bottom+1)*hCell;// dn rt
    liveBox[3].position.x = left*wCell;  liveBox[3].position.y = (bottom+1)*hCell;// dn lt
    liveBox[4].position.x = left*wCell;  liveBox[4].position.y = top*hCell;// up lt

    std::cout << "\n # cells updated = " << (right-left+1)*(bottom-top+1);

    return;
}

/*
void lvl_CGOL::update( float dt )
{
    if( !anyAlive ) return;
    genTime+=dt;
    if( genTime >= genPeriod || Level::step_update ){ genTime = 0.0f; }
    else return;

    ++genCount;
    to_SF_string( genCntMsgNum, genCount );
    anyAlive = false;

    const int dr[8] = { -1, -1, -1,  0, 0,  1, 1, 1 };
    const int dc[8] = { -1,  0,  1, -1, 1, -1, 0, 1 };

    for( int r = 0; r < (int)stA.size(); ++r )
    for( int c = 0; c < (int)stA[r].size(); ++c )
        if( stA[r][c] )
        {
            int liveNborCnt = 0;
            for( int i=0; i<8; ++i )
                if( r+dr[i] >= 0 && r+dr[i] < (int)rows && c+dc[i] >= 0 && c+dc[i] < (int)cols && stA[r+dr[i]][c+dc[i]] ) ++liveNborCnt;

            if( liveNborCnt < 2 ) stB[r][c] = false;// cell dies
            else if( liveNborCnt > 3 ) stB[r][c] = false;// cell dies
            else { stB[r][c] = true; anyAlive = true; }
        }
        else// dead cell
        {
            int liveNborCnt = 0;
            for( int i=0; i<8; ++i )
                if( r+dr[i] >= 0 && r+dr[i] < (int)rows && c+dc[i] >= 0 && c+dc[i] < (int)cols && stA[r+dr[i]][c+dc[i]] ) ++liveNborCnt;

            if( liveNborCnt == 3 ) { stB[r][c] = true; anyAlive = true; }// cell is reborn
            else stB[r][c] = false;
        }

    stA = stB;

    return;
}
*/

void lvl_CGOL::draw( sf::RenderTarget& RT ) const
{
//    std::vector< std::vector<bool> >& rSt = A_isCurrent ? stA : stB;
    sf::RectangleShape cellRect;
    cellRect.setSize( sf::Vector2f(wCell,hCell) );
    cellRect.setFillColor( sf::Color::Blue );

    if( anyAlive )
    {
        for( size_t r = 0; r < stA.size(); ++r )
        for( size_t c = 0; c < stA[r].size(); ++c )
            if( stA[r][c] )
            {
                cellRect.setPosition( sf::Vector2f(c*wCell,r*hCell) );
                RT.draw( cellRect );
            }

        RT.draw( liveBox, 5, sf::LinesStrip );
    }


    RT.draw( genCntMsg );
    RT.draw( genCntMsgNum );

    return;
}

void lvl_CGOL::cleanup()
{
    return;
}

bool lvl_CGOL::loadGame( const std::string& fName )
{
    std::string fullFname = "include/level/lvl_CGOL_data/" + fName + ".txt";
    std::ifstream fin( fullFname.c_str() );
    if( !fin ) { std::cout << "no game data\n"; return false; }

    genCount = 0;

    for( size_t r = 0; r < stA.size(); ++r )
    for( size_t c = 0; c < stA[r].size(); ++c )
        stA[r][c] = false;// all dead initially

    stB = stA;

    left = cols; right = 0; top = rows; bottom = 0;// max initial vals in search for new min values
    size_t offRow = 0, offCol = 0;
    fin >> offRow >> offCol;
    size_t numLive = 0;
    fin >> numLive;
    if( numLive > 0 )
    {
        for( size_t i=0; i<numLive; ++i )
        {
            size_t r, c;
            fin >> r >> c;// index to live cell
            r += offRow; c += offCol;
            stA[r][c] = true;
            if( r < top ) top = r;
            if( r > bottom ) bottom = r;
            if( c < left ) left = c;
            if( c > right ) right = c;
        }

        anyAlive = true;

        top = top < 1 ? 0 : top - 1;
        bottom = bottom < rows-1 ? bottom + 1 : rows-1;
        left = left < 1 ? 0 : left - 1;
        right = right < cols-1 ? right + 1 : cols-1;
    }

 //   gameList.closeList();// causes crash

    return true;
}
