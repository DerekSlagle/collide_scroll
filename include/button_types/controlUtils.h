#ifndef CONTROLUTILS_H_INCLUDED
#define CONTROLUTILS_H_INCLUDED

#include "../button_types/buttonValOnHit.h"
#include "../button_types/joyButton.h"
#include "../button_types/buttonList.h"
#include "../button_types/slideBar.h"
#include "../button_types/hoverSpot.h"
#include "../button_types/floatSpot.h"
#include "../button_types/textButton.h"
#include "../button_types/radioButton.h"
#include "../button_types/okBox.h"
#include "../button_types/colorPicker.h"

#include "../collider/waveSeg.h"
#include "../collider/gun.h"
#include "../collider/lineSegFlip.h"
#include "../collider/lineSegElevator.h"

#include "../rec.h"

/*
#include "button_types/buttonValOnHit.h"
#include "button_types/joyButton.h"
#include "button_types/buttonList.h"
#include "button_types/slideBar.h"
#include "button_types/hoverSpot.h"
#include "button_types/floatSpot.h"
#include "button_types/textButton.h"
#include "button_types/radioButton.h"
#include "waveSeg.h"
*/

namespace ctUtil
{
    void init_superList( buttonList& superList, float posx, float posy );
    void init_textList( buttonList& textList, float posx, float posy );
    void init_flipperControl( buttonList& flipControl, lineSegFlip& LSflip, float numReps = 1.0f );
    void init_elevatorControl( buttonList& elevControl, lineSegElevator& LSelev, float posx, float posy, float numReps = 1.0f );
    joyButton* init_gunControl( buttonList& gunControl, gun& Gun, float posx, float posy, float iVel = 30.0f );//, bool drive = false, float driveVel = 0.0f );
    void init_waveControl( buttonList& waveControl, waveSeg& wave, float posx, float posy, char type = 'c' );// c = click strip, s = slideBar
    void init_recorderControl( buttonList& recorderControl, float posx, float posy );
    void init_recorderControl( buttonList& recorderControl, float posx, float posy, float msgOfstX, float msgOfstY );// call above after assign msg positions, etc.
    void updateRecorderDisplay();
    void drawRecorderDisplay( sf::RenderTarget& RT );
    int addButtonState( button& rButt, std::vector<state_ab*>& p_stateVec );// returns # of states added
}

#endif // CONTROLUTILS_H_INCLUDED
