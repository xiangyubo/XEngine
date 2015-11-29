#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "PreHeader.h"
#include "RenderableThing.h"

class Particle : public RenderableThing
{
private:
    ID3D11Buffer* drawVB;   //渲染缓冲
	ID3D11Buffer* streamOutVB;  //输出流目标缓冲
    ID3D11ShaderResourceView *texRV;    //粒子纹理
    ID3D11ShaderResourceView* randomTexRV;  //随机纹理

public:
	float timeStep; //每两次调用之间的时间
	float age;      //存活时间

    bool m_bFirstRun;
	UINT m_nMaxParticles;

	XMFLOAT4 emitDirW;  //发射方向

public:
    Particle(); 

    ~Particle();

    BOOL Init(const string &filePath, const string &file);

    virtual void Update(float time);

    XMFLOAT4 GetWorldVertex();

    ID3D11Buffer*& GetDrawVertexBuffer()
    {
        return drawVB;
    }

    ID3D11Buffer* & GetStreamOutVertexBuffer()
    {
        return streamOutVB;
    }

    //设置发射方向
    void SetEmitDirect(XMFLOAT4 &);

    ID3D11ShaderResourceView* GetParticleSRV()
    {
        return texRV;
    }

    ID3D11ShaderResourceView* GetRandomSRV()
    {
        return randomTexRV;
    }

protected:
    virtual void Release();

    virtual BOOL InitBuffers();

    virtual void AssembleShadowMapVertices();

};

#endif