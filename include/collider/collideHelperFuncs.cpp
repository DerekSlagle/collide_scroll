#include "collideHelperFuncs.h"

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

            case 'R' :// lineSegRotate
            pCBvec.push_back( new lineSegRotate(fin) );
            break;

            case 'S' :// lineSegSwing
            pCBvec.push_back( new lineSegSwing(fin) );
            break;

            case 'F' :// lineSegFlip
            pCBvec.push_back( new lineSegFlip(fin) );
            break;

            case 'E' :// lineSegElevator
            pCBvec.push_back( new lineSegElevator(fin) );
            break;

            case 'B' :// lineBumper
            pCBvec.push_back( new lineBumper(fin) );
            break;

            case 'C' : {// lineSegConveyor
            lineSegConveyor* pLSconvey = new lineSegConveyor(fin);
            pCBvec.push_back( pLSconvey );
            if( !is_Hard ) pLSconvey->vtx[1].color = pLSconvey->vtx[0].color = sf::Color::Blue;
            break; }

            case 'W' :// waveSeg
            pCBvec.push_back( new waveSeg(fin) );
            break;

            default:
            std::cout << "FUCKED!!\n";
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
        //    std::cout << "ball\n";
            break;

            case 'R' :// regPolygon
            pMhVec.push_back( new regPolygon(is) );
         //   std::cout << "regPoly\n";
            break;

            case 'B' :// block
            pMhVec.push_back( new block(is) );
         //   std::cout << "block\n";
            break;

            case 'G' :// gun
            pMhVec.push_back( new gun(is) );
         //   std::cout << "gun\n";
            break;

            case 'D' :// decayPolygon
            pMhVec.push_back( new decayPolygon(is) );
         //   std::cout << "decayPoly\n";
            break;

            case 'E' :// expandPolygon
                pMhVec.push_back(  new expandPolygon(is) );
         //       std::cout << "expandPoly\n";
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
