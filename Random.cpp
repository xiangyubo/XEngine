#include "Random.h"
#include <ctime>

UINT Random::m_seed = time(0);
int32_t Random::MTArray[624] = {0};
int32_t Random::idx = 0;
int32_t Random::isInitialized = 0;

void Random::InitMT(int seed)
{
    int32_t i;
    int32_t p;
    idx = 0;
    MTArray[0] = seed;
    for (i = 1; i < 624; ++i) 
    {
        p = 1812433253 * (MTArray[i-1]^(MTArray[i-1] >> 30)) + i;
        MTArray[i] = p & 0xffffffff;
    }
    isInitialized = 1;
}

float Random::NextFloat( float min, float max )
{
    m_seed = 214013 * m_seed + 2531011;
    return min + (m_seed >> 16) * (1.0f / 65535.0f) * (max - min);
}

XMFLOAT2 Random::NextFloat(XMFLOAT2 min, XMFLOAT2 max)
{
    XMFLOAT2 rand;
    rand.x = Random::NextFloat(min.x,max.x);
    rand.y = Random::NextFloat(min.y,max.y);
    return rand;
}

XMFLOAT3 Random::NextFloat(XMFLOAT3 min, XMFLOAT3 max)
{
    XMFLOAT3 rand;
    rand.x = Random::NextFloat(min.x,max.x);
    rand.y = Random::NextFloat(min.y,max.y);
    rand.z = Random::NextFloat(min.z,max.z);

    return rand;
}

XMFLOAT4 Random::NextFloat(XMFLOAT4 min, XMFLOAT4 max)
{
    XMFLOAT4 rand;
    rand.x = Random::NextFloat(min.x,max.x);
    rand.y = Random::NextFloat(min.y,max.y);
    rand.z = Random::NextFloat(min.z,max.z);
    rand.w = Random::NextFloat(min.w,max.w);

    return rand;
}

float Random::NextFloat()
{
    return Random::NextFloat(0, 1.0f);
}

int Random::NextInt( int min, int max )
{
    return min + NextInt() % (max - min);
}

int Random::NextInt( int max )
{
    return NextInt() % (max+1);
}

int Random::NextInt()
{
    if (!isInitialized)
        InitMT((int32_t)time(NULL)); 

    if (idx == 0)
        Generate();

    int32_t y = MTArray[idx];
    y = y ^ (y >> 11);
    y = y ^ ((y << 7) & 2636928640);
    y = y ^ ((y << 15) & 4022730752);
    y = y ^ (y >> 18);

    idx = (idx + 1) % 624;
    return y;
}

void Random::Generate()
{
    int32_t i, y;
    for (i = 0; i < 624; ++i) 
    {
        y = (MTArray[i] & 0x80000000) + (MTArray[(i+1) % 624] & 0x7fffffff);
        MTArray[i] = MTArray[(i + 397) % 624] ^ (y >> 1);
        if (y & 0x0001)
        {
            MTArray[i] = MTArray[i] ^ (2567483615);
        }
    }
}
