#ifndef __TIMER__
#define __TIMER__

#include "PreHeader.h"

class Timer
{
private:
    BOOL    isPause;        //是否暂停
    LARGE_INTEGER time;     //系统绝对时间
    uint64_t lastTime;      //上一次记录时候的时间
    float scale;            //时间缩放比例
    static double Freq;     //统一系统时钟频率

public:
    Timer():isPause(false), scale(1.0f)
    {
        QueryPerformanceCounter(&time);
        lastTime = time.QuadPart;
    }

    ~Timer(){}

    void SetScale(float _scale)
    {
        scale = _scale;
    }

    float GetScale()
    {
        return scale;
    }

    void SetPause(BOOL pau)
    {
        isPause = pau;
    }

    BOOL IsPause()
    {
        return isPause;
    }

    //初始化定时器统一时钟频率
    static bool InitTimer()
    {
        LARGE_INTEGER _time;
        if(!QueryPerformanceFrequency(&_time))
        {
            return false;
        }

        Freq = double(_time.QuadPart)/1000.0;

        return true;
    }

    //获得同一个定时器距离上一次定时的时差
    double GetDelta()
    {
        QueryPerformanceCounter(&time);
        double delta = double(time.QuadPart - lastTime)/Freq;
        lastTime = time.QuadPart;
        if(isPause == TRUE)
        {
            return 0.0;
        }
        return delta * scale;
    }

    //获得和不同定时器之间的时差
    double GetDelta(const Timer &t)
    {
        QueryPerformanceCounter(&time);
        double delta = double(time.QuadPart - t.lastTime )/Freq;
        lastTime = time.QuadPart;
        if(isPause == TRUE)
        {
            return 0.0;
        }
        return delta * scale;
    }
};

#endif