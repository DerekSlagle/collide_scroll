#include "rec.h"

namespace rec
{
    bool running = false;
    size_t index_last = 0;
    size_t cacheLimit = 20;
    size_t cacheSize = 20;
    size_t dFrames = 3;// save period
    size_t frameCnt = 0;// kkeping track of relative position to initial state with this.
    bool* pOn = nullptr;

    // storage
    std::vector<state_ab*> pStateVec;// all saved states
     // init - binding
     std::function<void( std::vector<state_ab*>& )> pInitFunc = nullptr;

}

void rec::init( size_t cacheSz, size_t saveFreq, std::function<void( std::vector<state_ab*>& )> p_InitFunc, bool* p_onOff )
{
    cacheLimit = cacheSz;
    cacheSize = 0;
    dFrames = saveFreq;

    pInitFunc = p_InitFunc;
    pInitFunc( pStateVec );
    for( auto& pSt : pStateVec ) pSt->Reserve( cacheSz );
    index_last = 0;
    frameCnt = 0;
    running = false;
    pOn = p_onOff;
}

void rec::re_init()
{
    cleanup();
    pInitFunc( pStateVec );
    for( auto& pSt : pStateVec ) pSt->Reserve( cacheLimit );
    std::cerr << "\n re_init: Clearing history";
}

void rec::cleanup()
{
    for( auto& pSt : pStateVec ) delete pSt;
    pStateVec.clear();
    frameCnt = 0;
    index_last = 0;
    cacheSize = 0;
    running = false;
}

void rec::saveState()// every dFrames in circular manner
{

    if( cacheSize < cacheLimit )// push_back next set of datum
    {
        index_last = cacheSize++;
        for( auto& pSt : pStateVec ) pSt->pushState();
   //     std::cerr << "\n cache building: Nstates = " << cacheSize;
   //     std::cerr << " index_last = " << index_last;
    }
    else// over write last+1 (back of queue) then increment index_last
    {
        index_last = (1 + index_last)%cacheLimit;// 1st time rolls index_last = 0
        for( auto& pSt : pStateVec ) pSt->update(index_last);// update existing instances
    //    std::cerr << "\n cache cycling: index_last = " << index_last;
    }
}


void rec::restoreState( size_t ofst )// F9 to step back dFrames
{
    if( index_last >= cacheSize ){ std::cerr << "\nsvbh"; return; }// something very bad happened
    if( cacheSize == 0 ) return;// no states

    // ofst is from initial state, which is just ahead of index_last = next to be overwritten (oldest).
    size_t idx = 0;// index to 'initial state' if cache still building
    if( cacheSize == cacheLimit ) idx = index_last + 1;// 'initial' state follows 'current' state in the "queue" being emulated by cycling index_last
    idx += ofst; idx %= cacheSize;// add ofst and wrap
    for( const auto& pSt : pStateVec ) pSt->setState(idx);

    return;
}

bool rec::update()// will use function pointer to store pFunc(z) in cacheVec, or z itself otherwise.
{
    if( !running ) return false;
    if( !pInitFunc ) return false;

    if( cacheSize == 0 )
    {
        saveState();// should cause index_last = 0 assign
        frameCnt = 0;// reset this value too
    }

//    size_t cacheSz = pHistoryVec.size();

    ++frameCnt;

    if( frameCnt < dFrames*(cacheSize-1)  )// in playback
    {
        if( frameCnt%dFrames == 0 ) restoreState( frameCnt/dFrames );//{ std::cout << "\n playback: frameCnt= " << frameCnt << " index_last = " << index_last; restoreState( frameCnt/dFrames ); }
    }
    else if( frameCnt == dFrames*(cacheSize-1) )// end of playback reached
    {
        std::cout << "\n end playback. final state restored. frameCnt = " << frameCnt << " index_last = " << index_last;
        for( const auto& st : pStateVec ) st->setState(index_last);
        running = false;
        return false;// pause for user choice: continue forward and overwrite cached values, or replay recording
    }
    else// recording!
    {
        if( frameCnt%dFrames == 0 )
        {
      //      std::cout << " saving: frameCnt= " << frameCnt;
            saveState();
            if( cacheSize == cacheLimit ) frameCnt -= dFrames;// index_last got bumped above as well in this case
        }

        if( frameCnt > dFrames*cacheLimit )// illegal values have been seen
        { std::cerr << "\n frCnt too high! frCnt= " << frameCnt << " dF*cLmt= " << dFrames*cacheLimit;
          std::cerr << "\ncacheSz= " << cacheSize << " cacheLimit= " << cacheLimit; return false; }
    }

    return true;// keep going
}

void rec::initControl( std::vector<std::string>*& pLabelVec, std::vector< std::function<void(void)> >*& p_pFuncVec, size_t stepSz )
{
    static std::vector<std::string> LabelVec;
    static std::vector< std::function<void(void)> > pFuncVec;
    LabelVec.clear();
    LabelVec.reserve(8);
    pFuncVec.clear();
    pFuncVec.reserve(8);

    LabelVec.push_back( "recorder" );  pFuncVec.push_back( std::bind( rec::onOff ) );
    LabelVec.push_back( "stop" );      pFuncVec.push_back( std::bind( rec::stop ) );
    LabelVec.push_back( "record" );    pFuncVec.push_back( std::bind( rec::record ) );
    LabelVec.push_back( "play" );      pFuncVec.push_back( std::bind( rec::play ) );
    std::string rewStr("rew ");
    std::stringstream ss; ss << stepSz;
    rewStr += ss.str();
    LabelVec.push_back( rewStr );    pFuncVec.push_back( std::bind( [stepSz](void) { rec::rewind(stepSz); } ) );
    LabelVec.push_back( "goto 0" );    pFuncVec.push_back( std::bind( rec::goTo_initial ) );
    LabelVec.push_back( "jog rev" );   pFuncVec.push_back( std::bind( rec::jog_rev ) );
    LabelVec.push_back( "jog fwd" );   pFuncVec.push_back( std::bind( rec::jog_fwd ) );

    pLabelVec = &LabelVec;
    p_pFuncVec = &pFuncVec;
}

void rec::jog_rev()
{
    if( frameCnt == 0 ) { std::cout << "still at initial state\n"; std::cerr << "\n too far"; return; }

    if( frameCnt%dFrames != 0 )// backup to state just behind
        frameCnt = dFrames*(frameCnt/dFrames);
//    else// frCnt is a multiple of dFrames already
        frameCnt -= dFrames;// backup to previous state

 //   frameCnt = dFrames*(frameCnt/dFrames);// even up any fraction
    std::cout << "jog_rev: frameCnt= " << frameCnt;
    restoreState( frameCnt/dFrames );
    return;
}

void rec::jog_fwd()
{
    frameCnt = dFrames*(frameCnt/dFrames);// even up any fraction
    if( frameCnt == dFrames*(cacheSize - 1 ) ) { std::cout << "already at final state\n"; std::cerr << "\n at end already"; return; }
    frameCnt += dFrames;// forward to next state
    std::cout << "jog_fwd: frameCnt= " << frameCnt;
    restoreState( frameCnt/dFrames );
    return;
}// false if already at final state

bool rec::fwd( size_t n )
{
    if( frameCnt + n*dFrames > dFrames*cacheSize ) return false;
    frameCnt += n*dFrames;
    restoreState( frameCnt/dFrames );
    return true;
}

bool rec::rewind( size_t n )
{
    if( frameCnt < n*dFrames ) return false;
    frameCnt -= n*dFrames;
    frameCnt = dFrames*(frameCnt/dFrames);// even up any fraction
    std::cout << "rewind: frameCnt= " << frameCnt;
    restoreState( frameCnt/dFrames );
    return true;
}

void rec::rewind_50()
{
    if( frameCnt < 50*dFrames ) { std::cerr << "\n too far";  return; }
    frameCnt -= 50*dFrames;
    frameCnt = dFrames*(frameCnt/dFrames);// even up any fraction
    std::cout << "rewind: frameCnt= " << frameCnt;
    restoreState( frameCnt/dFrames );
    return;
}


bool rec::goTo( size_t n )
{
    if( n >= cacheSize ) return false;
    frameCnt = n*dFrames;
    restoreState(n);
    return true;
}

void rec::goTo_initial()
{
    frameCnt = 0;
    restoreState(0);
}

void rec::goTo_final()
{
    if( cacheSize == 0 ) return;
    frameCnt = dFrames*cacheSize;
    restoreState( cacheSize-1 );
}

void rec::onOff()
{
    if( pOn )
    {
        if( *pOn )// closing?
        {
            stop();
        }
        *pOn = !*pOn;
    }
}

void rec::record()
{
    if( cacheSize == 0 ){ frameCnt = 0; saveState();  }
    else frameCnt = dFrames*( cacheSize - 1 );// begin at current final_state

    std::cout << " rec begin curr value: frameCnt= " << frameCnt << " index_last: " << index_last;
    running = true;
}

void rec::play()// false if in final state
{
    if( cacheSize == 0 ) { std::cout << "play: empty vector\n"; return; }
 //   if( frameCnt == dFrames*mh_posVec.size() ) { std::cout << "play: at final state\n"; return false; }
    if( frameCnt == dFrames*(cacheSize - 1 ) ) { std::cout << "play: at final state\n"; return; }
    running = true;
    return;
}

void rec::stop()
{
 //   residualCnt = frameCnt%dFrames;
    std::cout << "\n stop:";
    running = false;
}

bool rec::is_recording()
{
    return running && frameCnt >= dFrames*(cacheSize -1 );
}

bool rec::is_running() { return running; }

size_t rec::get_frameCnt() { return frameCnt; }

size_t rec::get_baseIndex() { return index_last; }
