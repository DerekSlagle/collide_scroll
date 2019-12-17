#ifndef MAINHELPER_H_INCLUDED
#define MAINHELPER_H_INCLUDED

#include <vector>

#include "rec.h"
// mvHit types
#include "collider/ball_go.h"
//#include "ball.h"
#include "collider/regPolygon.h"
#include "collider/block.h"
#include "collider/gun.h"// derives from block
#include "collider/decayPolygon.h"
#include "collider/expandPolygon.h"
//#include "mvHit.h"

// segHit types
#include "collider/lineSeg.h"
#include "collider/waveSeg.h"
//#include "lineSegElevator.h"
#include "collider/lineSegConveyor.h"
//#include "lineSegFlip.h"
//#include "lineBumper.h"
//#include "lineSegRotate.h"
#include "collider/lineSegSwing.h"
#include "collider/arcSeg.h"

// control classes
//#include "dropList.h"
#include "spriteSheet.h"
//#include "slideBar.h"

//#include "buttonValOnHit.h"
//#include "joyButton.h"
//#include "buttonList.h"

#include "utility/controlUtils.h"

// other
#include "collider/waveTank.h"
#include "collider/waterPipe.h"


extern gun* pGun_1;// gun
extern expandPolygon *pXpgA;
extern waveSeg* p_wvSeg;
extern lineSegConveyor* pLSconvey;

extern buttonList waveControl;
extern buttonList gunControl;
extern buttonList recorderControl;

//extern waveTank wvTank_1;
extern std::vector<waveTank> wvTankVec;

bool loadSegs( std::istream&, std::vector<segHit*>& );
bool load_mvHits( std::istream&, std::vector<mvHit*>& );
bool loadChannel( std::istream&, std::vector<segHit*>& );
void loadMessages( sf::Text& helpMsg, sf::Text& pauseMsg, std::vector<sf::Text>& msgVec, const sf::Font& font );

void do_spawn( const mvHit& mh, std::vector<mvHit*>& pMhv );

void init_waveControl( waveSeg& wave, float posx, float posy );
void init_gunControl( gun& Gun, float posx, float posy );
void init_recorderControl( float posx, float posy );
//void do_fire( size_t skipIdx, float velGrad, std::vector<mvHit*>& pMhv );

#endif // MAINHELPER_H_INCLUDED
