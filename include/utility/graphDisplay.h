#ifndef GRAPHDISPLAY_H
#define GRAPHDISPLAY_H

#include <SFML/Graphics.hpp>
#include<vector>

class graphDisplay
{
    public:
        std::vector<sf::Vertex> vtxVec;
        sf::Vertex axes[8];// 2 sf::Quads 0-3=x, 4-7=y
        sf::RectangleShape bkRect;
        sf::Vector2f pos, sz;//
        float u_min, u_max, v_min, v_max;// domain and range. Scale determined

        // helpers
        float u_scale, v_scale;
        float x_ogn, y_ogn;// axes[0].position.y = y_orgn, axes[4].position.x = x_orgn
        float (*F)(float);

    graphDisplay();
    void init( sf::Vector2f pos, sf::Vector2f sz, float(*func)(float), float xmin, float xmax, float ymin, float ymax, sf::Color bkClr, sf::Color graphClr );
    void draw( sf::RenderWindow& rRW );
    void update( float t, float (*w)(float,float) );// eg: w = k*x - f*t
    virtual ~graphDisplay();

    protected:

    private:
};

#endif // GRAPHDISPLAY_H
