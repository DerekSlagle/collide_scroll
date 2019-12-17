#ifndef COLLIDEHELPERFUNCS_H_INCLUDED
#define COLLIDEHELPERFUNCS_H_INCLUDED

// mvHit types
#include "ball_go.h"
//#include "ball.h"
#include "regPolygon.h"
#include "block.h"
#include "gun.h"// derives from block
#include "decayPolygon.h"
#include "expandPolygon.h"
//#include "mvHit.h"

// segHit types
#include "lineSeg.h"
#include "waveSeg.h"
#include "lineSegElevator.h"
#include "lineSegConveyor.h"
#include "lineSegFlip.h"
#include "lineBumper.h"
//#include "lineSegRotate.h"
#include "lineSegSwing.h"
#include "arcSeg.h"

#include "waveTank.h"
#include "waterPipe.h"
#include "pipeCap.h"
#include "pipeStraight.h"
#include "pipeElbow.h"

bool loadSegs( std::istream&, std::vector<segHit*>& );
bool load_mvHits( std::istream&, std::vector<mvHit*>& );


#endif // COLLIDEHELPERFUNCS_H_INCLUDED
