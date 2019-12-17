#include "sfmlUtils.h"

// text effects functions
bool textExpand( sf::Text& r_text, float scale_i, float scale_f, int Nframes, bool reset )
{
    sf::FloatRect fr = r_text.getLocalBounds();

    static int frCount = 0;
    static float oldRt = fr.left + fr.width;
    static float oldBot = fr.top + fr.height;
    if(reset)
    {
        frCount = 0;
        oldRt = fr.left + fr.width;
        oldBot = fr.top + fr.height;
    }

    if( frCount < Nframes )
        ++frCount;
    else
        return false;// not expanding anymore

    float scl = scale_i + (scale_f - scale_i)*(float)frCount/(float)Nframes;
    r_text.setScale( scl, scl );
    fr = r_text.getLocalBounds();
    float newRt = fr.left + fr.width;
    float newBot = fr.top + fr.height;
    r_text.move( (oldRt-newRt)/2.0f, (oldBot-newBot)/2.0f );
    oldRt -= (oldRt-newRt)/2.0f;
    oldBot -= (oldBot-newBot)/2.0f;

    return true;
}

bool textFade( sf::Text& r_text, int alpha_i, int alpha_f, int Nframes, bool reset )
{
    static int frCount = 0;
    static unsigned int oldRed = r_text.getColor().r;
    static unsigned int oldGrn = r_text.getColor().g;
    static unsigned int oldBlu = r_text.getColor().b;
    if(reset)
    {
        frCount = 0;
        oldRed = r_text.getColor().r;
        oldGrn = r_text.getColor().g;
        oldBlu = r_text.getColor().b;
    }


    if( frCount < Nframes )
        ++frCount;
    else
        return false;// not fading anymore

    int alf = ( unsigned int )( (float)alpha_i + (float)(alpha_f - alpha_i)*(float)frCount/(float)Nframes );
    r_text.setColor( sf::Color(oldRed, oldGrn, oldBlu, alf) );

    return true;
}

sf::Vertex* laserDisplay()
{
    const int skySz = 6;
    const sf::Vertex sky[skySz] = { sf::Vertex( sf::Vector2f(400.0f, 50.0f), sf::Color::Red ),
                            sf::Vertex( sf::Vector2f(200.0f,100.0f), sf::Color::Blue ),
                            sf::Vertex( sf::Vector2f(600.0f,150.0f), sf::Color::Green ),
                            sf::Vertex( sf::Vector2f(470.0f,200.0f), sf::Color::Red ),
                            sf::Vertex( sf::Vector2f(450.0f,100.0f), sf::Color::Blue ),
                            sf::Vertex( sf::Vector2f(300.0f,80.0f), sf::Color::Green ) };

    const int delay = 10;
    static int delayIdx = 0, i = 0;
    static sf::Vertex pr[2] = { sf::Vertex( sf::Vector2f(400.0f,550.0f), sf::Color::Red ), sky[0] };

    if( ++delayIdx == delay )
    {
        delayIdx = 0;
        if( ++i == skySz ) i = 0;
        pr[1] = sky[i];
    }

    return pr;
}

void makeLED( std::vector<sf::Vertex>& qVec, sf::Vector2f pos, size_t nCells, size_t w, size_t h, size_t sp, sf::Color clr )
{
    sf::Vertex v( pos, clr );
    qVec.clear();

    for( size_t n = 0; n < nCells; ++n )
    {
        v.position.x = pos.x + (w+sp)*n;// UpLt
        v.position.y = pos.y;
        qVec.push_back(v);
        v.position.x += w;// UpRt
        qVec.push_back(v);

        v.position.y += h;// BtRt
        qVec.push_back(v);
        v.position.x -= w;// BtLt
        qVec.push_back(v);
    }
}

void drawLED( sf::RenderWindow& rRW, std::vector<sf::Vertex>& qVec, size_t nCells )
{
    if( qVec.size() > 4*nCells ) rRW.draw( &qVec[0], 4*nCells, sf::Quads );
    else rRW.draw( &qVec[0], qVec.size(), sf::Quads );
}

void sparkleInit( sf::Vertex* pVtx9, sf::Vector2f pos, sf::Color clr )// pVtx9 must point to array of >= 9 sf::Vertex
{
    for( size_t i=0; i<9; ++i)
    {
        pVtx9[i].position = pos;
        pVtx9[i].color = clr;
    }
}

void sparkleUpdate( sf::Vertex* pVtx9, float v, float Rmax )
{
    const float sqrt2 = sqrtf(2.0f);
//    if(  pVtx9[0].position.y - pVtx9[1].position.y >= Rmax )
    if(  pVtx9[3].position.x - pVtx9[7].position.x >= 2.0f*Rmax )
    {
        pVtx9[0].color = sf::Color::Cyan;
        return;
    }

    pVtx9[1].position.y -= v;// up
    pVtx9[2].position.x += v/sqrt2;// up rt
    pVtx9[2].position.y -= v/sqrt2;// up rt
    pVtx9[3].position.x += v;// rt
    pVtx9[4].position.x += v/sqrt2;// dn rt
    pVtx9[4].position.y += v/sqrt2;// dn rt
    pVtx9[5].position.y += v;// dn
    pVtx9[6].position.x -= v/sqrt2;// dn lt
    pVtx9[6].position.y += v/sqrt2;// dn lt
    pVtx9[7].position.x -= v;// lt
    pVtx9[8].position.x -= v/sqrt2;// up lt
    pVtx9[8].position.y -= v/sqrt2;// up lt

    pVtx9[0].position = pVtx9[8].position;// tie
}

void sparkleDraw( sf::Vertex* pVtx9,  sf::RenderWindow& rRW )
{
    if( pVtx9[0].color != sf::Color::Cyan )
//        rRW.draw( pVtx9, 0, sf::Points );
        rRW.draw( pVtx9, 9, sf::LinesStrip );
}

bool sparkleInUse( sf::Vertex* pVtx9 ) { return pVtx9[0].color != sf::Color::Cyan; }

void makeAxes( sf::Vertex* axes, sf::FloatRect wndw, sf::Color vclr )
{
    // x-axis
    axes[0].position.x = wndw.left + 5.0f;
    axes[0].position.y = wndw.top + wndw.height/2.0f;     axes[0].color = vclr;
    axes[1].position.x = wndw.left + wndw.width - 5.0f;
    axes[1].position.y = wndw.top + wndw.height/2.0f;      axes[1].color = vclr;
    // y-axis
    axes[2].position.x = wndw.left + wndw.width/2.0f;
    axes[2].position.y = wndw.top + 5.0f;     axes[2].color = vclr;
    axes[3].position.x = wndw.left + wndw.width/2.0f;
    axes[3].position.y = wndw.top + wndw.height - 5.0f;  axes[3].color = vclr;
}

void makeBoundBox(  sf::Vertex* bndBox, const sf::Sprite& spr, sf::Color vclr )
{
    // bounding box
    sf::Vector2f pos = spr.getPosition();
    float w = spr.getLocalBounds().width;  w *= spr.getScale().x;
    float h = spr.getLocalBounds().height; h *= spr.getScale().y;
    bndBox[0].position.x = pos.x - w/2.0f; bndBox[0].position.y = pos.y - h/2.0f; bndBox[0].color = vclr;// UpLt
    bndBox[1].position.x = pos.x + w/2.0f; bndBox[1].position.y = pos.y - h/2.0f; bndBox[1].color = vclr;// UpRt
    bndBox[2].position.x = pos.x + w/2.0f; bndBox[2].position.y = pos.y + h/2.0f; bndBox[2].color = vclr;// DnRt
    bndBox[3].position.x = pos.x - w/2.0f; bndBox[3].position.y = pos.y + h/2.0f; bndBox[3].color = vclr;// DnLt
    bndBox[4] = bndBox[0];
}

void starFieldInit( std::vector<std::vector<sf::Vertex>>& layerVec, int numLayers, int* pNL, sf::FloatRect bndBox )
{
    std::default_random_engine rgen;// seed this?
    std::uniform_real_distribution<float> fdx( bndBox.left, bndBox.left+bndBox.width );
    std::uniform_real_distribution<float> fdy( bndBox.top, bndBox.top+bndBox.height );
    layerVec.resize( numLayers );

    // base layer is single vertex stars
    layerVec[0].resize( pNL[0] );
     for( int j=0; j< pNL[0]; ++j )
     {
        layerVec[0][j].position.x = fdx(rgen);
        layerVec[0][j].position.y = fdy(rgen);
        layerVec[0][j].color = sf::Color::White;
     }

    // other layers are quads with increasing size
    int starSz = 2;
    for( int i=1; i< numLayers; ++i )
    {
        starSz = i+1;
        layerVec[i].resize( 4*pNL[i] );

         for( int j=0; j< 4*pNL[i]; j += 4 )
         {
            float posX = fdx(rgen), posY = fdy(rgen);
            layerVec[i][j].position.x = posX;// UpLt
            layerVec[i][j].position.y = posY;
            layerVec[i][j+1].position.x = posX + starSz;// UpRt
            layerVec[i][j+1].position.y = posY;
            layerVec[i][j+2].position.x = posX + starSz;// DnRt
            layerVec[i][j+2].position.y = posY + starSz;
            layerVec[i][j+3].position.x = posX;// DnLt
            layerVec[i][j+3].position.y = posY + starSz;
            layerVec[i][j].color = layerVec[i][j+1].color = layerVec[i][j+2].color = layerVec[i][j+3].color = sf::Color::White;
         }
    }
}

void starFieldUpdate( std::vector<std::vector<sf::Vertex>>& layerVec, float* pVely, sf::FloatRect bndBox )
{
    static std::default_random_engine rgen;// seed this?
    static std::uniform_real_distribution<float> posx( bndBox.left, bndBox.left+bndBox.width );

    // single vertex stars
    for( size_t j=0; j< layerVec[0].size(); ++j )
    {
        layerVec[0][j].position.y += pVely[0];
        if( pVely[0] > 0.0f && layerVec[0][j].position.y > bndBox.top + bndBox.height )
        {
            layerVec[0][j].position.y = bndBox.top;
            layerVec[0][j].position.x = posx(rgen);
        }
        else if( pVely[0] < 0.0f && layerVec[0][j].position.y < bndBox.top )
        {
            layerVec[0][j].position.y = bndBox.top + bndBox.height;
            layerVec[0][j].position.x = posx(rgen);
        }
    }

    // quad layers
    for( size_t i=1; i< layerVec.size(); ++i )
        for( size_t j=0; j< layerVec[i].size(); j+=4 )
        {
            layerVec[i][j].position.y += pVely[i];// UpLt
            layerVec[i][j+1].position.y += pVely[i];// UpRt
            layerVec[i][j+2].position.y += pVely[i];// DnRt
            layerVec[i][j+3].position.y += pVely[i];// DnLt
            if( pVely[i] > 0.0f && layerVec[i][j].position.y > bndBox.top + bndBox.height )
            {
                float posX = posx(rgen);
                layerVec[i][j].position.y = bndBox.top;
                layerVec[i][j].position.x = posX;
                layerVec[i][j+1].position.y = bndBox.top;
                layerVec[i][j+1].position.x = posX + i+1;
                layerVec[i][j+2].position.y = bndBox.top + i+1;
                layerVec[i][j+2].position.x = posX + i+1;
                layerVec[i][j+3].position.y = bndBox.top + i+1;
                layerVec[i][j+3].position.x = posX;
            }
            else if( pVely[i] < 0.0f && layerVec[i][j].position.y < bndBox.top )
            {
                float posX = posx(rgen);
                layerVec[i][j].position.y = bndBox.top + bndBox.height;
                layerVec[i][j].position.x = posX;
                layerVec[i][j+1].position.y = bndBox.top + bndBox.height;
                layerVec[i][j+1].position.x = posX + i+1;
                layerVec[i][j+2].position.y = bndBox.top + bndBox.height + i+1;
                layerVec[i][j+2].position.x = posX + i+1;
                layerVec[i][j+3].position.y = bndBox.top + bndBox.height + i+1;
                layerVec[i][j+3].position.x = posX;
            }
        }
}

void starFieldDraw( const std::vector<std::vector<sf::Vertex>>& layerVec, sf::RenderTarget& RT )
{
    RT.draw( &(layerVec[0][0]), layerVec[0].size(), sf::Points );
    for( size_t i=1; i< layerVec.size(); ++i )
        RT.draw( &(layerVec[i][0]), layerVec[i].size(), sf::Quads );
}

void assignRHbox_all( sf::Vertex v[], int stIdx, sf::FloatRect rR, sf::FloatRect tR )
{
    assignRHbox_pos( v, stIdx, rR );
    assignRHbox_tex( v, stIdx, tR );
}

void assignRHbox_pos( sf::Vertex v[], int stIdx, sf::FloatRect rR )
{
    v[stIdx].position.x = rR.left;// UpLt
    v[stIdx].position.y = rR.top;
    stIdx += 1; stIdx %= 4;
    v[stIdx].position.x = rR.left + rR.width;// UpRt
    v[stIdx].position.y = rR.top;
    stIdx += 1; stIdx %= 4;
    v[stIdx].position.x = rR.left + rR.width;// DnRt
    v[stIdx].position.y = rR.top + rR.height;
    stIdx += 1; stIdx %= 4;
    v[stIdx].position.x = rR.left;// DnLt
    v[stIdx].position.y = rR.top + rR.height;
}

void assignRHbox_tex( sf::Vertex v[], int stIdx, sf::FloatRect tR )
{
    v[stIdx].texCoords.x = tR.left;// UpLt
    v[stIdx].texCoords.y = tR.top;
    stIdx += 1; stIdx %= 4;
    v[stIdx].texCoords.x = tR.left + tR.width;// UpRt
    v[stIdx].texCoords.y = tR.top;
    stIdx += 1; stIdx %= 4;
    v[stIdx].texCoords.x = tR.left + tR.width;// DnRt
    v[stIdx].texCoords.y = tR.top + tR.height;
    stIdx += 1; stIdx %= 4;
    v[stIdx].texCoords.x = tR.left;// DnLt
    v[stIdx].texCoords.y = tR.top + tR.height;
}
