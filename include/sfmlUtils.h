#ifndef SFMLUTILS_H_INCLUDED
#define SFMLUTILS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include<string>
#include<sstream>
#include<random>

template<class T>
float to_sf_string(sf::Text& strArg, T x)// returns position of end of strArg
{
    std::stringstream ss;
    ss << x;
    std::string tempStr;
    ss >> tempStr;
    strArg.setString( tempStr.c_str() );
    sf::FloatRect fr = strArg.getLocalBounds();
    return fr.left + fr.width;
}

// text effects functions
bool textExpand( sf::Text& r_text, float scale_i, float scale_f, int Nframes, bool reset );
bool textFade( sf::Text& r_text, int alpha_i, int alpha_f, int Nframes, bool reset );

sf::Vertex* laserDisplay();
void makeLED( std::vector<sf::Vertex>& qVec, sf::Vector2f pos, size_t nCells, size_t w, size_t h, size_t sp, sf::Color clr );
void drawLED( sf::RenderWindow& rRW, std::vector<sf::Vertex>& qVec, size_t nCells );

void sparkleInit( sf::Vertex* pVtx9, sf::Vector2f pos, sf::Color clr );// pVtx9 must point to array of >= 9 sf::Vertex
void sparkleUpdate( sf::Vertex* pVtx9, float v, float Rmax );
void sparkleDraw( sf::Vertex* pVtx9,  sf::RenderWindow& rRW );
bool sparkleInUse( sf::Vertex* pVtx9 );

void starFieldInit( std::vector<std::vector<sf::Vertex>>& layerVec, int numLayers, int* pNL, sf::FloatRect bndBox );
void starFieldUpdate( std::vector<std::vector<sf::Vertex>>& layerVec, float* pVely, sf::FloatRect bndBox );
void starFieldDraw(  const std::vector<std::vector<sf::Vertex>>& layerVec, sf::RenderTarget& RT );

// make axes through and bounding box for Ani
// sf::Vertex axes[4]; sf::Vertex bndBox[5];
// RW.draw( bndBox, 5, sf::LinesStrip ); RW.draw( axes, 4, sf::Lines );
 //else if( rEvent.key.code == sf::Keyboard::A ){ --rIR.left; std::cout << "\n--Lt: " << rIR.left; }
 // else if( rEvent.key.code == sf::Keyboard::D ){ ++rIR.left;  std::cout << "\n++Lt: " << rIR.left; }
 // else if( rEvent.key.code == sf::Keyboard::W ){ --rIR.top;   std::cout << "\n--Tp: " << rIR.top;  }
 // else if( rEvent.key.code == sf::Keyboard::S ){ ++rIR.top;   std::cout << "\n++Tp: " << rIR.top;  }
void makeAxes( sf::Vertex* axes, sf::FloatRect wndw, sf::Color vclr );// axes through center of wndw
void makeBoundBox(  sf::Vertex* bndBox, const sf::Sprite& spr, sf::Color vclr );// obtain & to texture rect from SS and fine tune it

void assignRHbox_all( sf::Vertex v[], int stIdx, sf::FloatRect rR, sf::FloatRect tR );
void assignRHbox_pos( sf::Vertex v[], int stIdx, sf::FloatRect rR );
void assignRHbox_tex( sf::Vertex v[], int stIdx, sf::FloatRect tR );


#endif // SFMLUTILS_H_INCLUDED
