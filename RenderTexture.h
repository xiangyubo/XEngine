#ifndef __RENDERTEXTURE__
#define __RENDERTEXTURE__

#include "PreHeader.h"

class RenderTexture
{
public:
    RenderTexture();
    RenderTexture(const RenderTexture&);
    ~RenderTexture();

    bool Init(ID3D11Device*, int, int);
    void Shutdown();

    void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
    void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
    ID3D11ShaderResourceView* GetShaderResourceView();

private:
    ID3D11Texture2D *renderTargetTexture;
    ID3D11RenderTargetView *renderTargetView;
    ID3D11ShaderResourceView *shaderResourceView;
};

#endif