#ifndef __RANDOM__
#define __RANDOM__

#include "PreHeader.h"

class Random
{
private:
	static UINT m_seed;
    static int32_t MTArray[624];
    static int32_t idx;
    static int32_t isInitialized;
public:
    static void InitMT(int seed);
	static float NextFloat(float min, float max);
    static XMFLOAT2 NextFloat(XMFLOAT2 min, XMFLOAT2 max);
	static XMFLOAT3 NextFloat(XMFLOAT3 min, XMFLOAT3 max);
	static XMFLOAT4 NextFloat(XMFLOAT4 min, XMFLOAT4 max);
	static float NextFloat();
	static int NextInt(int min, int max);
	static int NextInt(int max);
    static int NextInt();
    static void Generate();

private:
    Random() 
    {}
    Random(const Random& )
    {}
    Random & operator = (const Random& )
    {}
    ~Random()
    {}

};

#endif