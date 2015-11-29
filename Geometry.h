#ifndef __GEOMETRY__
#define __GEOMETRY__

#include "Utility.h"

//基本速度
struct Velocity
{
    float vx;
    float vy;
    Velocity(float _x = 0.0, float _y = 0.0):vx(_x), vy(_y) 
    {
        
    }
};

//基本坐标
struct Position
{
    float x;
    float y;
    Position(float _x = 0.0, float _y = 0.0):x(_x), y(_y)
    {
        
    }

    bool operator == (const Position &pos) const
    {
        return x == pos.x && y == pos.y;
    }

    void move(const Velocity &v)
    {
        x += v.vx;
        y += v.vy;
    }
};

//矩形
struct Rect
{
    Position lu;
    Position rd;
    Rect()
    {

    }

    Rect(const Position _lu, const Position _rd):lu(_lu), rd(_rd) 
    {

    }

    bool is_contain(const Position &pos) const
    {
        return lu.x <= pos.x && 
            lu.y >= pos.y &&
            rd.x >= pos.x && 
            rd.y <= pos.y;
    }

    bool is_intersect(const Rect &rt) const
    {
        if(lu.x > rt.rd.x)  return false;
        if(rd.x < rt.lu.x)  return false;
        if(lu.y < rt.rd.y)  return false;
        if(rd.y > rt.lu.y)  return false;
        return true;
        //float x1 = MyMax(lu.x, rt.lu.x);
        //float y1 = MyMin(lu.y, rt.lu.y);
        //float x2 = MyMin(rd.x, rt.rd.x);
        //float y2 = MyMax(rd.y, rt.rd.y);
        //return !(x1 > x2 || y1 < y2);
    }
};


#endif