#ifndef FLIPDIGIT_H_INCLUDED
#define FLIPDIGIT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include<fstream>
#include "../spriteSheet.h"

class flipDigit
{
    public:
    const spriteSheet* pSS;
    flipDigit* next;
    sf::Vertex vtxA[8];// one digit = Bt + Top
    sf::Vertex vtxB[8];// other loaded digit
    sf::Vertex* pVtx[2] = { vtxA, vtxB };
//    sf::Vertex** ppVtx = pVtx;

    bool flipping = false;
    float dx, dy;// per frame expand/ contract: -/+ then +/-
    size_t currDigit = 0;
    float tileSzX = 10, tileSzY = 10;// actual = 105 x 72
    float flipSpeed;// 0.04
    float flareWidth;
    float scale = 1.0f;
    bool callRecursive = false;

    flipDigit();
    bool init( const spriteSheet& SS, size_t iValue, sf::Vector2f pos, flipDigit* Next, float speed = 0.1f, float flare = 0.005f );
    void draw( sf::RenderTarget& rRW )const;
    void loadDigit_toVtx( sf::Vertex* pQuad0, size_t frIdx, sf::Vector2f pos );
    void swapPtrs( sf::Vertex*& pVa, sf::Vertex*& pVb );
    void initFlip();
    void updateFlipDisplay();
    void setPosition( sf::Vector2f pos );

    private:


};

void initFlipArray( flipDigit* pFd, int numDgts,  const spriteSheet& SS, size_t iValue, sf::Vector2f Pos0, float dgtSpacing, float speed=0.1f, float flare=0.005f );
void updateFlipArray( flipDigit* pFd, int numDgts );
void drawFlipArray( const flipDigit* pFd, int numDgts, sf::RenderTarget& rRW );
void setFlipArrayPosition( flipDigit* pFd, int numDgts, sf::Vector2f pos );

#endif // FLIPDIGIT_H_INCLUDED
