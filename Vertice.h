#ifndef __VERTICE__
#define __VERTICE__

#include "PreHeader.h"
#include "Utility.h"

namespace VERTICE
{
    struct Pos
    {
        XMFLOAT3    pos;        //局部坐标
    };

    struct PosNormalTex
    {
        XMFLOAT3    pos;        //局部坐标
        XMFLOAT3    normal;     //法线向量
        XMFLOAT2    tex;        //纹理坐标
    };

    struct PosNormalTexTan
    {
        XMFLOAT3    pos;        //局部坐标
        XMFLOAT3    normal;     //法线向量
        XMFLOAT3	tangent;    //切线向量
        XMFLOAT2    tex;        //纹理坐标
    };

    struct ParticleVertex 
    {
        XMFLOAT3 initialPos;
        XMFLOAT3 initialVel;
        XMFLOAT2 size;
        float age;
        unsigned int type;
    };
}

struct InputLayoutDesc
{
    static const D3D11_INPUT_ELEMENT_DESC Pos[1];
    static const D3D11_INPUT_ELEMENT_DESC PosNormalTex[3];
    static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTan[4];
    static const D3D11_INPUT_ELEMENT_DESC Partical[5];
};

struct InputLayouts
{
    static BOOL isInited;
    static ID3D11InputLayout    *Pos;
    static UINT pStride;
    static ID3D11InputLayout    *PosNormalTex;
    static UINT pNTStride;
    static ID3D11InputLayout    *PosNormalTexTan;
    static UINT pNTTStride;
    static ID3D11InputLayout    *Partical;
    static UINT pPStride;

private:
    static ID3DX11Effect            *verticeEffect;
    static ID3DX11EffectTechnique   *posTech;
    static ID3DX11EffectTechnique   *posNormTexTech;
    static ID3DX11EffectTechnique   *posNormalTexTanTech;
    static ID3DX11EffectTechnique   *particalTech;

    static ID3DBlob *pTechBlob;
    static ID3DBlob *pErrorBlob;

public:

    static BOOL Init(ID3D11Device* device);
    static void Destroy();
};

#endif