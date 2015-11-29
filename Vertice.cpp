#include "Vertice.h"

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Pos[1] = 
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosNormalTex[3] = 
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosNormalTexTan[4] = 
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Partical[5] = 
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

BOOL InputLayouts::isInited = FALSE;
ID3D11InputLayout*  InputLayouts::Pos = NULL;
ID3D11InputLayout*  InputLayouts::PosNormalTex = NULL;
ID3D11InputLayout*  InputLayouts::PosNormalTexTan = NULL;
ID3D11InputLayout*  InputLayouts::Partical = NULL;

ID3DX11Effect*      InputLayouts::verticeEffect = NULL;
ID3DX11EffectTechnique* InputLayouts::posTech = NULL;
ID3DX11EffectTechnique* InputLayouts::posNormTexTech = NULL;
ID3DX11EffectTechnique* InputLayouts::posNormalTexTanTech = NULL;
ID3DX11EffectTechnique* InputLayouts::particalTech = NULL;

ID3DBlob*   InputLayouts::pTechBlob = NULL;
ID3DBlob*   InputLayouts::pErrorBlob = NULL;

UINT InputLayouts::pStride = 0;
UINT InputLayouts::pNTStride = 0;
UINT InputLayouts::pNTTStride = 0;
UINT InputLayouts::pPStride = 0;

BOOL InputLayouts::Init(ID3D11Device* device)
{
    if(isInited == TRUE)
    {
        return TRUE;
    }
	D3DX11_PASS_DESC    passDesc;

    if(FAILED(D3DX11CompileFromFile(TEXT("shaders/Vertices.fx"),
        NULL, NULL, NULL, "fx_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS,
        0, NULL, &pTechBlob, &pErrorBlob,
        NULL)))
    {
        MessageBox(NULL, TEXT("编译节点着色器失败"), TEXT("ERROR"), MB_OK);
        Destroy();
        return FALSE;
    }

    if( FAILED( D3DX11CreateEffectFromMemory(pTechBlob->GetBufferPointer(),
        pTechBlob->GetBufferSize(), 0, device,
        &verticeEffect)))
    {
        MessageBox(NULL, TEXT("创建节点渲染效果失败"), TEXT("ERROR"), MB_OK);
        Destroy();
        return FALSE;
    }

    posTech = verticeEffect->GetTechniqueByName("PosTech");
    posNormTexTech = verticeEffect->GetTechniqueByName("PosNormalTexTech");
    posNormalTexTanTech = verticeEffect->GetTechniqueByName("PosNormalTexTanTech");
    particalTech = verticeEffect->GetTechniqueByName("ParticalTech");

	posTech->GetPassByIndex(0)->GetDesc(&passDesc);
	if( FAILED(device->CreateInputLayout(InputLayoutDesc::Pos, 1, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &Pos)))
    {        
        MessageBox(NULL, TEXT("创建位置节点布局失败"), TEXT("ERROR"), MB_OK);
        Destroy();
        return FALSE;
    }
    pStride = sizeof(VERTICE::Pos);

	posNormTexTech->GetPassByIndex(0)->GetDesc(&passDesc);
	if( FAILED(device->CreateInputLayout(InputLayoutDesc::PosNormalTex, 3, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &PosNormalTex)))
    {
        MessageBox(NULL, TEXT("创建位置-法线-纹理坐标节点布局失败"), TEXT("ERROR"), MB_OK);
        Destroy();
        return FALSE;
    }
    pNTStride = sizeof(VERTICE::PosNormalTex);

	posNormalTexTanTech->GetPassByIndex(0)->GetDesc(&passDesc);
	if( FAILED(device->CreateInputLayout(InputLayoutDesc::PosNormalTexTan, 4, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &PosNormalTexTan)))
    {
        MessageBox(NULL, TEXT("创建位置-法线-纹理坐标-切线坐标节点布局失败"), TEXT("ERROR"), MB_OK);
        Destroy();
        return FALSE;
    }
    pNTTStride = sizeof(VERTICE::PosNormalTexTan);

    particalTech->GetPassByIndex(0)->GetDesc(&passDesc);
	if( FAILED(device->CreateInputLayout(InputLayoutDesc::Partical, 5, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &Partical)))
    {
        MessageBox(NULL, TEXT("粒子节点布局失败"), TEXT("ERROR"), MB_OK);
        Destroy();
        return FALSE;
    }
    pPStride = sizeof(VERTICE::ParticleVertex);
    isInited = TRUE;

    return TRUE;
}

void InputLayouts::Destroy()
{
    SafeRelease(Pos);
	SafeRelease(PosNormalTex);
	SafeRelease(PosNormalTexTan);
    SafeRelease(Partical);
    SafeRelease(verticeEffect);
    SafeRelease(pTechBlob);
    SafeRelease(pErrorBlob);
}
