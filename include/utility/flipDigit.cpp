#include "flipDigit.h"

flipDigit::flipDigit(): pSS(nullptr), next(nullptr), flipping(false), currDigit(0), tileSzX(10), tileSzY(10)
{
    pVtx[0] = vtxA;
    pVtx[1] = vtxB;
    dx = dy = 1.0f;
    flipSpeed = 0.04f;
    flareWidth = 0.005;
}

bool flipDigit::init( const spriteSheet& SS, size_t iValue, sf::Vector2f pos, flipDigit* Next, float speed, float flare  )
{
    pSS = &SS;
    currDigit = iValue;
    next = Next;
    flipSpeed = speed < 0.0f ? -speed : speed;
    flareWidth = flare < 0.0f ? -flare : flare;
    tileSzX = SS.getFrRect(0,0).width*scale;
    tileSzY = SS.getFrRect(0,0).height*scale;

    loadDigit_toVtx( vtxA, currDigit, pos );
    currDigit += 1;
    currDigit %= 10;
    loadDigit_toVtx( vtxB, currDigit, pos );
    return true;
}

void flipDigit::draw( sf::RenderTarget& rRW )const
{
    rRW.draw( pVtx[0], 8, sf::Quads, &(pSS->txt) );
    if( flipping ) rRW.draw( pVtx[1], 8, sf::Quads, &(pSS->txt) );

    if( callRecursive && next ) next->draw(rRW);
}

void flipDigit::setPosition( sf::Vector2f pos )
{
 //   if( flipping ) return;
    // reassign positions for
    sf::Vector2f dPos;
//    dPos.x = pos.x - vtxA[0].position.x;// save offset to any point not moved during a flip (the lowers)
//    dPos.y = pos.y - vtxA[0].position.y;
    dPos.x = pos.x - vtxA[4].position.x;// save offset to any point not moved during a flip (the lowers)
    dPos.y = pos.y + vtxA[7].position.y - 2*vtxA[4].position.y;
    // move all 16 vertices by dPos
    for( size_t i=0; i<8; ++i )
    {
        vtxA[i].position += dPos;
        vtxB[i].position += dPos;
    }
}

void flipDigit::loadDigit_toVtx( sf::Vertex* pQuad0, size_t frIdx, sf::Vector2f pos )
{
//    const size_t vPqd = 4;
    sf::IntRect IR[2] = { pSS->getFrRect(frIdx,0), pSS->getFrRect(frIdx,1) };// up - dn
    sf::Vertex* pQuad1 = pQuad0 + 4;
    size_t i=0;// looper

    for(i=0; i<4; ++i)// both sets
    {
        pQuad0[i].position = pos;
        pQuad1[i].position = pos;
        pQuad1[i].position.y += IR[0].height*scale;

        pQuad0[i].texCoords = sf::Vector2f( IR[0].left, IR[0].top );
        pQuad1[i].texCoords = sf::Vector2f( IR[1].left, IR[1].top );
    }

    // offsets from UpLt
    pQuad0[1].texCoords.x += IR[0].width;  pQuad0[1].position.x += IR[0].width*scale;// to UpRt
    pQuad0[2].texCoords.x += IR[0].width;  pQuad0[2].position.x += IR[0].width*scale;
    pQuad0[2].texCoords.y += IR[0].height; pQuad0[2].position.y += IR[0].height*scale;// to BtmRt
    pQuad0[3].texCoords.y += IR[0].height; pQuad0[3].position.y += IR[0].height*scale;// to BtmLt

    pQuad1[1].texCoords.x += IR[1].width;  pQuad1[1].position.x += IR[1].width*scale;
    pQuad1[2].texCoords.x += IR[1].width;  pQuad1[2].position.x += IR[1].width*scale;
    pQuad1[2].texCoords.y += IR[1].height; pQuad1[2].position.y += IR[1].height*scale;
    pQuad1[3].texCoords.y += IR[1].height; pQuad1[3].position.y += IR[1].height*scale;
}

void flipDigit::swapPtrs( sf::Vertex*& pVa, sf::Vertex*& pVb )
{
    sf::Vertex* temp = pVa;
    pVa = pVb;
    pVb = temp;
}

void flipDigit::initFlip()
{
    if( flipping ) return;
    flipping = true;
    dy = tileSzY*flipSpeed;
    dx = tileSzX*flareWidth;
    swapPtrs( pVtx[0], pVtx[1] );
    if( currDigit == 0 && next ) next->initFlip();
}

void flipDigit::updateFlipDisplay()
{
    if( callRecursive && next ) next->updateFlipDisplay();

    if( !flipping ) return;

    if( dx > 0.0f )// 1st 1/2 of flip
    {
        pVtx[1][0].position.y += dy;// UpLt
        pVtx[1][0].position.x -= dx;// UpLt
        pVtx[1][1].position.y += dy;// UpRt
        pVtx[1][1].position.x += dx;// UpRt

        if( pVtx[1][0].position.y >= pVtx[1][2].position.y )// mid flip point: top below bottom
        {
            pVtx[1][0].position.y = pVtx[1][3].position.y - tileSzY;
            pVtx[1][1].position.y = pVtx[1][2].position.y - tileSzY;
            pVtx[0][6].position.x = pVtx[1][1].position.x;// UpRt
            pVtx[0][7].position.x = pVtx[1][0].position.x;// UpLt
            pVtx[0][6].position.y = pVtx[0][5].position.y;
            pVtx[0][7].position.y = pVtx[0][4].position.y;

            pVtx[1][0].position.x = pVtx[1][3].position.x;
            pVtx[1][1].position.x = pVtx[1][2].position.x;

            dx *= -1.0f;// now shrink
            swapPtrs( pVtx[0], pVtx[1] );
        }
    }
    else// 2nd 1/2 of flip dx>0
    {
        pVtx[1][7].position.y += dy;// BtLt
        pVtx[1][7].position.x -= dx;// BtLt
        pVtx[1][6].position.y += dy;// BtRt
        pVtx[1][6].position.x += dx;// BtRt

        if( pVtx[1][6].position.y >= pVtx[1][5].position.y + tileSzY )// flip over
        {
            flipping = false;
            pVtx[1][6].position.y = pVtx[1][5].position.y + tileSzY;
            pVtx[1][7].position.y = pVtx[1][4].position.y + tileSzY;
            pVtx[1][6].position.x = pVtx[1][5].position.x;
            pVtx[1][7].position.x = pVtx[1][4].position.x;
            swapPtrs( pVtx[0], pVtx[1] );

            currDigit += 1;
            currDigit %= 10;

            loadDigit_toVtx( pVtx[1], currDigit,  pVtx[0][0].position );
        }
    }
}

void initFlipArray( flipDigit* pFd, int numDgts,  const spriteSheet& SS, size_t iValue, sf::Vector2f Pos0, float dgtSpacing, float speed, float flare )
{
    dgtSpacing += (float)SS.getFrRect(0,0).width*pFd->scale;
    sf::Vector2f Posf( Pos0.x + (numDgts-1)*dgtSpacing, Pos0.y );

    if( speed < 0.0f ) speed *= -1.0f;
    if( flare < 0.0f ) flare *= -1.0f;

    for( int i=0; i<numDgts-1; ++i )
    {
        pFd[i].init( SS, iValue%10, sf::Vector2f( Posf.x-i*dgtSpacing, Pos0.y ), pFd+i+1, speed, flare );
        iValue /= 10;
    }

    pFd[numDgts-1].init( SS, 0, Pos0, nullptr, speed, flare ); // last digit in set
}

void setFlipArrayPosition( flipDigit* pFd, int numDgts, sf::Vector2f pos )
{
    // left most digit at pos is pFd[numDgts-1]
    sf::Vector2f spc = pFd[1].vtxA[0].position - pFd[0].vtxA[0].position;
    for( int i=0; i<numDgts; ++i )
    {
        sf::Vector2f spc_x_i;
        spc_x_i.x = i*spc.x;// retarded
        spc_x_i.y = i*spc.y;// neccesity
 //       pFd[i].setPosition( pos + spc_x_i );
        pFd[(numDgts-1)-i].setPosition( pos - spc_x_i );
    }

}

void updateFlipArray( flipDigit* pFd, int numDgts )
{
    for( int i=0; i<numDgts; ++i ) pFd[i].updateFlipDisplay();
}

void drawFlipArray( const flipDigit* pFd, int numDgts, sf::RenderTarget& rRW )
{
    for( int i=0; i<numDgts; ++i ) pFd[i].draw(rRW);
}

// sample usage
/*
// declare global
const int numDgts = 4;
flipDigit flipSet[numDgts];


// in main()
     // load a spriteSheet
    std::ifstream inFile("images/flipDigitsSS_data.txt");
    if( !inFile ) return 5;
    spriteSheet flipDgtSS( inFile );
    inFile.close();
    initFlipArray( flipSet, 4,  flipDgtSS, 6, sf::Vector2f(100.0f,100.0f), 12.0f );
// game loop
updateFlipArray ( flipSet, numDgts );
// to render
drawFlipArray ( flipSet, numDgts, RW );

// call initFlip() to trigger a flip
*/

