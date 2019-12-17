#ifndef LEG_H
#define LEG_H

#include<cmath>

class Leg
{
    public:
        float x0, y0;
        float len;
        Leg *prev, *next;
        Leg( float X0, float Y0, float Len );
        virtual ~Leg();

        virtual float x( float s )const = 0;
        virtual float y( float s )const = 0;
        virtual void set_xy( float s, float& rx, float& ry ) const { rx = x(s); ry = y(s); }// base version
        virtual void T( float s, float& Tx, float& Ty )const;// provide unit tangent to curve at s
        const Leg* update( float& rx, float& ry, float& s, float v, float dt )const;
   //     virtual const Leg* update( float& rx, float& ry, float& s, float v, float dt );
        const Leg* updateGrav( float& rx, float& ry, float& s, float& v, float dt, float g )const;

    protected:

    private:
};

#endif // LEG_H
