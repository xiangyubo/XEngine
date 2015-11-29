#include "RenderSystem.h"
#include "Utility.h"
#include "Light.h"
#include "Effects.h"
#include "Obj.h"
#include "Model.h"
#include "Vertice.h"
#include "SkyBox.h"
#include "Skeleton.h"
#include "Particle.h"
#include "RenderTexture.h"
#include "TerrainShader.h"
#include "Terrain.h"

DXGI_FORMAT GetDepthResourceFormat(DXGI_FORMAT depthformat)
{
    DXGI_FORMAT resformat;
    switch (depthformat)
    {
    case DXGI_FORMAT::DXGI_FORMAT_D16_UNORM:
        resformat = DXGI_FORMAT::DXGI_FORMAT_R16_TYPELESS;
        break;
    case DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT:
        resformat = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
        break;
    case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT:
        resformat = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
        break;
    case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        resformat = DXGI_FORMAT::DXGI_FORMAT_R32G8X24_TYPELESS;
        break;
    }

    return resformat;
}

DXGI_FORMAT GetDepthSRVFormat(DXGI_FORMAT depthformat)
{
    DXGI_FORMAT srvformat;
    switch (depthformat)
    {
    case DXGI_FORMAT::DXGI_FORMAT_D16_UNORM:
        srvformat = DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT;
        break;
    case DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT:
        srvformat = DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        break;
    case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT:
        srvformat = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
        break;
    case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        srvformat = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        break;
    }
    return srvformat;
}

XMMATRIX InverseTransposeX(CXMMATRIX M)
{
    XMMATRIX A = M;
    A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

    XMVECTOR det = XMMatrixDeterminant(A);
    return XMMatrixTranspose(XMMatrixInverse(&det, A));
}

RenderSystem::RenderSystem()
{
    swapChain = NULL;
    device = NULL;
    deviceContext = NULL;
    renderTargetView = NULL;

    effectMgr = NULL;

    lightEffect = NULL;
    directionLightTech = NULL;
    pointLightTech = NULL;
    spotLightTech = NULL;

    testEffect = NULL;
    testTech = NULL;

    currentEffect = NULL;
    currentTech = NULL;

    depthStencilView = NULL;
    shadowFlag = FALSE;
    renderTexture = NULL;

    terrainShader = NULL;

    width = 0;
    height = 0;
    msaaQuality = 0;
    shadersName.clear();
}

RenderSystem* RenderSystem::GetInstance()
{
    static RenderSystem instance;
    return &instance;
}

BOOL RenderSystem::Init(HWND hwnd)
{   
    RECT rc;
    GetClientRect(hwnd, &rc);
    width = rc.right - rc.left;
    height = rc.bottom - rc.top;

    if(ReadShadersEtc() == FALSE)
    {
        return FALSE;
    }

    if(!InitD3D(hwnd))
    {    
        return FALSE;
    }

    effectMgr = new EffectMgr;
    if(effectMgr->Init(device, "shaders/", shadersName) == FALSE)
    {
        return FALSE;
    }
    lightEffect = effectMgr->GetEffectByName("LightShaders.fx");
    directionLightTech = effectMgr->GetTechByName("T_DirLight");
    shadowMapEffect = effectMgr->GetEffectByName("ShadowMap.fx");
    shadowMapTech = effectMgr->GetTechByName("ShadowMapRender");

    if( !InputLayouts::Init(device) )
    {
        return FALSE;
    }

    SetCurrentETL(lightEffect, directionLightTech, InputLayouts::PosNormalTexTan);

    terrainShader = new TerrainShader;
    ID3DX11Effect *tempEffect = effectMgr->GetEffectByName("Terrain.fx");
    ID3DX11EffectTechnique *tempTech = effectMgr->GetTechByName("TerrainTech");
    terrainShader->Init(tempEffect, tempTech);

    return TRUE;
}

BOOL RenderSystem::InitD3D(HWND hwnd)
{
    HRESULT	hr;
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    D3D_FEATURE_LEVEL	curLevel;
    hr = D3D11CreateDevice(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        NULL,
        featureLevels,
        ARRAYSIZE( featureLevels ),
        D3D11_SDK_VERSION,
        &device,
        &curLevel,
        &deviceContext);
    if(FAILED(hr))
    {
        MessageBox(NULL,TEXT("创建D3D设备失败!"),TEXT("ERROR"),MB_OK);
        return FALSE;
    }

    device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaaQuality);
    msaaQuality = 0;
    DXGI_SWAP_CHAIN_DESC scDesc = {0};
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferDesc.Width = width;
    scDesc.BufferDesc.Height = height;
    scDesc.BufferDesc.RefreshRate.Numerator = 60;
    scDesc.BufferDesc.RefreshRate.Denominator = 1;
    scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scDesc.BufferCount = 1;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.Flags = 0;
    scDesc.OutputWindow = hwnd;
    scDesc.SampleDesc.Count = msaaQuality<1?1:4;
    scDesc.SampleDesc.Quality = msaaQuality<1?0:msaaQuality-1;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scDesc.Windowed = TRUE;

    IDXGIDevice *pDxgiDevice(NULL);
    hr = device->QueryInterface(__uuidof(IDXGIDevice),reinterpret_cast<void**>(&pDxgiDevice));
    if(FAILED(hr))
    {
        MessageBox(NULL,TEXT("获取 DXGIDevice 失败！"),TEXT("ERROR"),MB_OK);
        return FALSE;
    }
    IDXGIAdapter *pDxgiAdapter(NULL);
    hr = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter),reinterpret_cast<void**>(&pDxgiAdapter));
    if(FAILED(hr))
    {
        MessageBox(NULL,TEXT("获取 DXGIAdapter 失败！"),TEXT("ERROR"),MB_OK);
        return FALSE;
    }
    IDXGIFactory *pDxgiFactory(NULL);
    hr = pDxgiAdapter->GetParent(__uuidof(IDXGIFactory),reinterpret_cast<void**>(&pDxgiFactory));
    if(FAILED(hr))
    {
        MessageBox(NULL,TEXT("获取 DXGIFactory 失败"),TEXT("ERROR"),MB_OK);
        return FALSE;
    }
    hr = pDxgiFactory->CreateSwapChain(device,&scDesc,&swapChain);
    if(FAILED(hr))
    {
        MessageBox(NULL,TEXT("创建交换链失败"),TEXT("ERROR"),MB_OK);
        return FALSE;
    }
    SafeRelease(pDxgiFactory);
    SafeRelease(pDxgiAdapter);
    SafeRelease(pDxgiDevice);

    ID3D11Texture2D *backBuffer(NULL);
    swapChain->GetBuffer(0,__uuidof(ID3D11Texture2D),reinterpret_cast<void**>(&backBuffer));
    hr = device->CreateRenderTargetView(backBuffer,0,&renderTargetView);

    if(FAILED(hr))
    {
        MessageBox(NULL,TEXT("创建渲染目标视图失败！"),TEXT("ERROR"),MB_OK);
        return FALSE;
    }
    SafeRelease(backBuffer);

    renderTexture = new RenderTexture;
    renderTexture->Init(device, width, height);
    //初始化深度模板视图
    InitDepthStencilView(DXGI_FORMAT_D32_FLOAT);
    //初始化阴影贴图纹理视图和深度模板测试视图
    InitShadowMapView(DXGI_FORMAT_D32_FLOAT);

    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    deviceContext->RSSetViewports( 1, &vp );

    return TRUE;
}

void RenderSystem::SetDepthTest()
{
    D3D11_DEPTH_STENCIL_DESC depthDesc;
    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthDesc.StencilEnable = TRUE;
    depthDesc.StencilReadMask = 0xFF;
    depthDesc.StencilWriteMask = 0xFF;

    depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    if( FAILED( device->CreateDepthStencilState(&depthDesc, &depthStencilState)))
    {
        MessageBox(NULL,TEXT("创建深度\模板状态失败"),TEXT("Error"),MB_OK);
    }
    deviceContext->OMSetDepthStencilState(depthStencilState, 1);
}

ID3D11Device* RenderSystem::getDevice()
{
    return device;
}

ID3D11DeviceContext* RenderSystem::GetDeviceContext()
{
    return deviceContext;
}

void RenderSystem::Shutdown()
{
    ShutdownD3D();
    ShutdownShaders();
    ShutdownDepthStencil();
}

void RenderSystem::ShutdownD3D()
{
    if(swapChain)
    {
        swapChain->SetFullscreenState(FALSE, NULL);
    }
    SafeRelease(deviceContext);
    SafeRelease(device);
    SafeRelease(renderTargetView);
    SafeRelease(swapChain);
    SafeDelete(renderTexture);
}

void RenderSystem::ShutdownShaders()
{
    SafeRelease(lightEffect);
    SafeRelease(testEffect);
}

void RenderSystem::ShutdownDepthStencil()
{
    SafeRelease(depthStencilView);
    SafeRelease(depthStencilBuffer);
    SafeRelease(depthStencilState);
}

void RenderSystem::ClearBackground()
{
    float ClearColor[4] =  {0.0f, 0.125f, 0.3f, 1.0f};
    deviceContext->ClearRenderTargetView(renderTargetView, ClearColor);
}

void RenderSystem::ClearDepthBuffer(ID3D11DepthStencilView *dsv)
{
    deviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderSystem::InitLightVP(const Light* light)
{
    XMVECTOR    eye;    //眼睛所在位置
    XMVECTOR    up = XMVectorSet(0, 1, 0, 0);   //向上的向量
    XMVECTOR    at = XMVectorSet(0, 0, 0, 1);   //观察对象的位置

    if(light->type == 0) //平行光
    {
        eye = -1 * 40 * XMLoadFloat4(&light->direction);
        //eye = XMLoadFloat4(&light.position);
        eye.m128_f32[3] = 1.0f;
        lightView = XMMatrixLookAtLH(eye, at, up);
        lightProj = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1.2f, 20.0f, 100.0f);
        //lightProj = XMMatrixOrthographicOffCenterLH(-20, 20, -20, 20, 10.0f, 60.0f);       
    }
    else
    {
        eye = XMLoadFloat4(&light->position);
        eye.m128_f32[3] = 1.0f;
        lightView = XMMatrixLookAtLH(eye, at, up);
        lightProj = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1.2f, 0.01f, 100.0f);
    }
}

void RenderSystem::SetLightVP()
{
    currentEffect->GetVariableByName("ShadowMapView")->AsMatrix()->SetMatrix((float*)&lightView);
    currentEffect->GetVariableByName("ShadowMapProjection")->AsMatrix()->SetMatrix((float*)&lightProj);
}

BOOL RenderSystem::CreateBuffer(const D3D11_BUFFER_DESC &bd, const D3D11_SUBRESOURCE_DATA &initData, ID3D11Buffer* &buffer)
{
    HRESULT res = device->CreateBuffer(&bd, &initData, &buffer);
    if( FAILED(res))
    {
        return FALSE;
    }
    return TRUE;
}

void RenderSystem::Present()
{
    swapChain->Present(0, 0);   
}

void RenderSystem::SetMaterial(const Material *mate)
{
    ID3DX11EffectVectorVariable *pAmbient = currentEffect->GetVariableByName("MatAmbient")->AsVector();
    ID3DX11EffectVectorVariable *pDiffuse = currentEffect->GetVariableByName("MatDiffuse")->AsVector();
    ID3DX11EffectVectorVariable *pSpecular = currentEffect->GetVariableByName("MatSpecular")->AsVector();
    ID3DX11EffectScalarVariable *pPower = currentEffect->GetVariableByName("MatPower")->AsScalar();
    ID3DX11EffectScalarVariable *pTextureOn = currentEffect->GetVariableByName("textureOn")->AsScalar();
    ID3DX11EffectShaderResourceVariable *pTexMap = currentEffect->GetVariableByName("Texture")->AsShaderResource();

    pAmbient->SetFloatVector((float*)&mate->ambient);
    pDiffuse->SetFloatVector((float*)&mate->diffuse);
    pSpecular->SetFloatVector((float*)&mate->specular);
    pPower->SetFloat(mate->power);
    if(mate->texture == nullptr)
    {
        pTextureOn->SetBool(FALSE);
        pTexMap->SetResource(NULL);
    }
    else
    {
        auto res = pTextureOn->SetBool(TRUE);
        res = pTexMap->SetResource(mate->texture);
    }
}

void RenderSystem::SetLight(const Light* light)
{
    _light = *light;
    ID3DX11EffectScalarVariable *pType = currentEffect->GetVariableByName("type")->AsScalar();
    ID3DX11EffectVectorVariable *pLightPos = currentEffect->GetVariableByName("LightPos")->AsVector();
    ID3DX11EffectVectorVariable *pLightDir = currentEffect->GetVariableByName("LightDir")->AsVector();
    ID3DX11EffectVectorVariable *pLightAmb = currentEffect->GetVariableByName("LightAmb")->AsVector();
    ID3DX11EffectVectorVariable *pLightDif = currentEffect->GetVariableByName("LightDiff")->AsVector();
    ID3DX11EffectVectorVariable *pLightSpe = currentEffect->GetVariableByName("LightSpe")->AsVector();
    ID3DX11EffectScalarVariable *pLightAtt0 = currentEffect->GetVariableByName("LightAtt0")->AsScalar();
    ID3DX11EffectScalarVariable *pLightAtt1 = currentEffect->GetVariableByName("LightAtt1")->AsScalar();
    ID3DX11EffectScalarVariable *pLightAtt2 = currentEffect->GetVariableByName("LightAtt2")->AsScalar();
    ID3DX11EffectScalarVariable *pLightAlpha = currentEffect->GetVariableByName("LightAlpha")->AsScalar();
    ID3DX11EffectScalarVariable *pLightBeta = currentEffect->GetVariableByName("LightBeta")->AsScalar();
    ID3DX11EffectScalarVariable *pLightFallOff = currentEffect->GetVariableByName("LightFallOff")->AsScalar();

    pType->SetInt(light->type);
    pLightPos->SetFloatVector((float*)&light->position);
    pLightDir->SetFloatVector((float*)&light->direction);

    pLightAmb->SetFloatVector((float*)&light->ambient);
    pLightDif->SetFloatVector((float*)&light->diffuse);
    pLightSpe->SetFloatVector((float*)&light->specular);

    pLightAtt0->SetFloat(light->attenuation0);
    pLightAtt1->SetFloat(light->attenuation1);
    pLightAtt2->SetFloat(light->attenuation2);

    pLightAlpha->SetFloat(light->alpha);
    pLightBeta->SetFloat(light->beta);
    pLightFallOff->SetFloat(light->falloff);

    InitLightVP(light);
}

void RenderSystem::SetProjection(const XMMATRIX& proj)
{
    _proj = proj;
    currentEffect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float*)&proj);
}

void RenderSystem::SetView(const XMMATRIX& view)
{
    _view = view;
    currentEffect->GetVariableByName("View")->AsMatrix()->SetMatrix((float*)&view);
}

void RenderSystem::SetEye(const XMVECTOR& eye)
{
    XMStoreFloat4(&eyePos, eye);
    currentEffect->GetVariableByName("EyePosition")->AsVector()->SetFloatVector((float*)&eye);
}

void RenderSystem::SetCurrentETL(ID3DX11Effect *effect,
                                 ID3DX11EffectTechnique *tech,
                                 ID3D11InputLayout *layout)
{
    currentEffect = effect;
    currentTech = tech;
    currentLayout = layout;
    deviceContext->IASetInputLayout(currentLayout);
}

void RenderSystem::SetShadowOpen(BOOL flag)
{
    shadowFlag = flag;
}

BOOL RenderSystem::InitShadowMapView(DXGI_FORMAT format)
{
    DXGI_FORMAT resformat = GetDepthResourceFormat(format);
    DXGI_FORMAT srvformat = GetDepthSRVFormat(format);

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texDesc.SampleDesc.Count = msaaQuality<1?1:4;
    texDesc.SampleDesc.Quality = msaaQuality<1?0:msaaQuality-1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    if( FAILED( device->CreateTexture2D( &texDesc, NULL, &shadowMapBuffer ) ) ) 
    {
        MessageBox(NULL,TEXT("创建阴影贴图缓冲失败！"),TEXT("ERROR"),MB_OK);
        return FALSE;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDSV.ViewDimension = msaaQuality<1 ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DMS;
    descDSV.Texture2DMS.UnusedField_NothingToDefine = 0;
    descDSV.Texture2D.MipSlice = 0;

    if( FAILED( device->CreateDepthStencilView( shadowMapBuffer, &descDSV, &DS_SMView ) ) )
    {
        MessageBox(NULL,TEXT("创建阴影贴图深度模板测试视图失败！"),TEXT("ERROR"),MB_OK);
        return FALSE;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R32G32_TYPELESS;
    srvDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = msaaQuality<1 ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DMS;
    srvDesc.Texture2D.MipLevels = texDesc.MipLevels;    

    if( FAILED( device->CreateShaderResourceView( shadowMapBuffer, &srvDesc, &shadowMapView) ) ) 
    {
        MessageBox(NULL,TEXT("创建阴影贴图纹理资源视图失败！"),TEXT("ERROR"),MB_OK);
        return FALSE;
    }
    SafeRelease(shadowMapBuffer);
    return TRUE;
}

BOOL RenderSystem::InitDepthStencilView(DXGI_FORMAT format)
{
    HRESULT	hr;
    DXGI_FORMAT resformat = GetDepthResourceFormat(format);

    D3D11_TEXTURE2D_DESC dsDesc;
    dsDesc.Format = resformat;
    //dsDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    dsDesc.Width = width;
    dsDesc.Height = height;
    dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    dsDesc.MipLevels = 1;
    dsDesc.ArraySize = 1;
    dsDesc.CPUAccessFlags = 0;
    dsDesc.SampleDesc.Count = msaaQuality<1?1:4;
    dsDesc.SampleDesc.Quality = msaaQuality<1?0:msaaQuality-1;
    dsDesc.MiscFlags = 0;
    dsDesc.Usage = D3D11_USAGE_DEFAULT;
    hr = device->CreateTexture2D(&dsDesc,0,&depthStencilBuffer);
    if(FAILED(hr))
    {
        MessageBox(NULL,TEXT("创建深度模板缓冲失败！"),TEXT("ERROR"),MB_OK);
        return FALSE;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = format;
    //descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2DMS.UnusedField_NothingToDefine = 0;

    hr = device->CreateDepthStencilView(depthStencilBuffer, &descDSV, &depthStencilView);
    if(FAILED(hr))
    {
        MessageBox(NULL,TEXT("创建深度模板视图失败"),TEXT("ERROR"),MB_OK);
        return FALSE;
    }
    SafeRelease(depthStencilBuffer);
    return TRUE;
}

void RenderSystem::DrawTerrain(Terrain *terrain)
{
    terrainShader->SetShaderParameters(terrain->GetWorldMatrix(), _view, _proj, &_light, terrain->GetTexture());
    //terrainShader->SetSadowParameters(shadowFlag, renderTexture->GetShaderResourceView(), lightView, lightProj);
    terrainShader->Render(terrain);
}

void RenderSystem::DrawSky(const SkyBox *sky, XMMATRIX &wvp)
{
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
    ClearDepthBuffer(depthStencilView);
    ClearBackground();
    ID3DX11Effect *skyEffect = effectMgr->GetEffectByName("Sky.fx");
    ID3DX11EffectTechnique *skyTech = effectMgr->GetTechByName("SkyTech");
    SetCurrentETL(skyEffect, skyTech, InputLayouts::Pos);
    currentEffect->GetVariableByName("WorldViewProj")->AsMatrix()->SetMatrix((float*)&wvp);
    currentEffect->GetVariableByName("skyMap")->AsShaderResource()->SetResource(sky->GetSkyResource());

    UINT stride = InputLayouts::pStride;
    UINT offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &sky->GetShadowMapVerticeBuffer(), &stride, &offset);
    deviceContext->IASetIndexBuffer(sky->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

    _DrawObj(sky);
}

void RenderSystem::DrawRenderables(const vector<Val>& renderables)
{
    objs.clear();
    models.clear();
    skeletons.clear();
    particles.clear();
    for(auto it = renderables.begin(); it != renderables.end(); ++it)
    {
        auto obj = it->get();
        if(obj->GetRenderType() == RenderableThing::RenderType::PARTICLE)
        {
            particles.push_back(dynamic_cast<Particle*>(obj));
        }
        else if(obj->GetRenderType() == RenderableThing::RenderType::OBJ)
        {
            objs.push_back(dynamic_cast<const Obj*>(obj));
        }
        else if(obj->GetRenderType() == RenderableThing::RenderType::MODEL)
        {
            models.push_back(dynamic_cast<const Model*>(obj));
        }
        else if(obj->GetRenderType() == RenderableThing::RenderType::SKELETON)
        {
            skeletons.push_back(dynamic_cast<const Skeleton*>(obj));
        }
    }
    if(shadowFlag == TRUE)
    {
        _DrawShadowMap();
    }
    //_DrawParticles();
    _DrawRenderables();
}

void RenderSystem::_DrawShadowMap()
{
    //设置渲染目标视图为空，不需要执行像素着色器
    renderTexture->SetRenderTarget(deviceContext, DS_SMView);
    ClearDepthBuffer(DS_SMView);

    //deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
    //ClearDepthBuffer(depthStencilView);

    SetCurrentETL(shadowMapEffect, shadowMapTech, InputLayouts::Pos);
    SetLightVP();  

    //设置渲染图元模式
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = InputLayouts::pStride;
    UINT offset = 0;

    for(auto it = objs.begin(); it != objs.end(); ++it)
    {
        auto obj = *it;
        currentEffect->GetVariableByName("World")->AsMatrix()->SetMatrix((float*)&obj->GetWorldMatrix());
        deviceContext->IASetVertexBuffers(0, 1, &obj->GetShadowMapVerticeBuffer(), &stride, &offset);
        deviceContext->IASetIndexBuffer(obj->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
        _DrawObj(obj);
    }

    for(auto it = models.begin(); it != models.end(); ++it)
    {
        auto obj = *it;
        currentEffect->GetVariableByName("World")->AsMatrix()->SetMatrix((float*)&obj->GetWorldMatrix());
        deviceContext->IASetVertexBuffers(0, 1, &obj->GetShadowMapVerticeBuffer(), &stride, &offset);
        deviceContext->IASetIndexBuffer(obj->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
        _DrawModel(obj, true);
    }

    for(auto it = skeletons.begin(); it != skeletons.end(); ++it)
    {
        auto obj = *it;
        currentEffect->GetVariableByName("World")->AsMatrix()->SetMatrix((float*)&obj->GetWorldMatrix());
        _DrawSkeleton(obj, true);
    }
}

void RenderSystem::_DrawRenderables()
{
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
    ClearDepthBuffer(depthStencilView);
    //ClearBackground();
    SetCurrentETL(lightEffect, directionLightTech, InputLayouts::PosNormalTexTan);
    currentEffect->GetVariableByName("shadowFlag")->AsScalar()->SetBool(shadowFlag);

    if(shadowFlag == TRUE)
    {
        //设置阴影贴图
        SetLightVP();
        currentEffect->GetVariableByName("shadowMap")->AsShaderResource()->SetResource(renderTexture->GetShaderResourceView());
        //currentEffect->GetVariableByName("shadowMap")->AsShaderResource()->SetResource(renderTargetView);
    }

    //设置渲染图元模式
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = InputLayouts::pNTTStride;
    UINT offset = 0;

    for(auto it = objs.begin(); it != objs.end(); ++it)
    {
        auto obj = *it;
        currentEffect->GetVariableByName("World")->AsMatrix()->SetMatrix((float*)&obj->GetWorldMatrix());
        deviceContext->IASetVertexBuffers(0, 1, &obj->GetVerticeBuffer(), &stride, &offset);
        deviceContext->IASetIndexBuffer(obj->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
        SetMaterial(&obj->GetMaterial());
        _DrawObj(obj);
    }

    for(auto it = models.begin(); it != models.end(); ++it)
    {
        auto obj = *it;
        currentEffect->GetVariableByName("World")->AsMatrix()->SetMatrix((float*)&obj->GetWorldMatrix());
        deviceContext->IASetVertexBuffers(0, 1, &obj->GetVerticeBuffer(), &stride, &offset);
        deviceContext->IASetIndexBuffer(obj->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
        _DrawModel(obj, false);
    }

    for(auto it = skeletons.begin(); it != skeletons.end(); ++it)
    {
        auto obj = *it;
        currentEffect->GetVariableByName("World")->AsMatrix()->SetMatrix((float*)&obj->GetWorldMatrix());
        _DrawSkeleton(obj, false);
    }
    currentEffect->GetVariableByName("shadowMap")->AsShaderResource()->SetResource(NULL);
}

void RenderSystem::SetParticleArgv(Particle *par)
{
    currentEffect->GetVariableByName("gGameTime")->AsScalar()->SetFloat(par->age);
    currentEffect->GetVariableByName("gTimeStep")->AsScalar()->SetFloat(par->timeStep);
    currentEffect->GetVariableByName("gEyePosW")->AsVector()->SetFloatVector((float*)&eyePos);
    currentEffect->GetVariableByName("gEmitPosW")->AsVector()->SetFloatVector((float*)&par->GetWorldVertex());
    currentEffect->GetVariableByName("gEmitDirW")->AsVector()->SetFloatVector((float*)&par->emitDirW);
    currentEffect->GetVariableByName("gTexArray")->AsShaderResource()->SetResource(par->GetParticleSRV());
    currentEffect->GetVariableByName("gRandomTex")->AsShaderResource()->SetResource(par->GetRandomSRV());

}

void RenderSystem::_DrawParticles()
{
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
    ClearDepthBuffer(depthStencilView);
    ID3DX11Effect *tempEffect = effectMgr->GetEffectByName("fire.fx");
    ID3DX11EffectTechnique *tempTech = effectMgr->GetTechByName("StreamOutTech");
    SetCurrentETL(tempEffect, tempTech, InputLayouts::Partical);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    D3DX11_TECHNIQUE_DESC techDesc;
    UINT stride = InputLayouts::pPStride;
    UINT offset = 0;

    for(auto it = particles.begin(); it != particles.end(); ++it)
    {
        auto obj = *it;
        SetParticleArgv(obj);

        if(obj->m_bFirstRun == TRUE)
        {
            deviceContext->IASetVertexBuffers(0, 1, &obj->GetVerticeBuffer(), &stride, &offset);
        }
        else
        {
            deviceContext->IASetVertexBuffers(0, 1, &obj->GetDrawVertexBuffer(), &stride, &offset);
        }
        deviceContext->SOSetTargets(1, &obj->GetStreamOutVertexBuffer(), &offset);
        currentTech->GetDesc(&techDesc);
        for(UINT i = 0; i < techDesc.Passes; ++i)
        {
            currentTech->GetPassByIndex(i)->Apply(0, deviceContext);
            if(obj->m_bFirstRun == TRUE)
            {
                deviceContext->Draw(1, 0);
                obj->m_bFirstRun = FALSE;
            }
            else
            {
                deviceContext->DrawAuto();
            }
        }

        ID3D11Buffer* bufferArray[1] = {0};
        deviceContext->SOSetTargets(1, bufferArray, &offset);
        std::swap(obj->GetDrawVertexBuffer(), obj->GetStreamOutVertexBuffer());
        deviceContext->IASetVertexBuffers(0, 1, &obj->GetDrawVertexBuffer(), &stride, &offset);

        ID3DX11EffectTechnique *tempTech = effectMgr->GetTechByName("DrawTech");
        SetCurrentETL(tempEffect, tempTech, InputLayouts::Partical);
        currentTech->GetDesc(&techDesc);
        for(UINT i = 0; i < techDesc.Passes; ++i)
        {
            currentTech->GetPassByIndex(i)->Apply(0, deviceContext);
            deviceContext->DrawAuto();
        }
    }
}

void RenderSystem::_DrawObj(const Obj *obj)
{
    D3DX11_TECHNIQUE_DESC techDesc;
    currentTech->GetDesc(&techDesc);
    for(UINT i = 0; i < techDesc.Passes; ++i)
    {
        currentTech->GetPassByIndex(i)->Apply(0, deviceContext);
        deviceContext->DrawIndexed(obj->GetVerticeCount(), 0, 0);
    }
}

void RenderSystem::_DrawModel(const Model* mod, bool isShadow)
{
    auto meshes = mod->GetMeshes();
    for(auto it = meshes.begin(); it != meshes.end(); ++it)
    {
        if(isShadow == false)
        {
            Material currentMate = *mod->GetMateTable().find(it->mate)->second;
            SetMaterial(&currentMate);
        }

        D3DX11_TECHNIQUE_DESC techDesc;
        currentTech->GetDesc(&techDesc);
        for(UINT i = 0; i < techDesc.Passes; ++i)
        {
            currentTech->GetPassByIndex(i)->Apply(0, deviceContext);
            deviceContext->DrawIndexed(it->indexCount, it->startIndex, 0);
        }
    }
}

void RenderSystem::_DrawSkeleton(const Skeleton *ske, bool isShadow)
{
    auto meshes = ske->GetMeshes();

    for(auto it = meshes.begin(); it != meshes.end(); ++it)
    {
        if(isShadow == true)
        {
            UINT stride = InputLayouts::pStride;
            UINT offset = 0;
            deviceContext->IASetVertexBuffers(0, 1, &it->shadowMapVerticesBuffer, &stride, &offset);
        }
        else
        {
            UINT stride = InputLayouts::pNTTStride;
            UINT offset = 0;
            deviceContext->IASetVertexBuffers(0, 1, &it->verticesBuffer, &stride, &offset);

            if(it->texture == NULL)
            {
                currentEffect->GetVariableByName("textureOn")->AsScalar()->SetBool(FALSE);       
            }
            else
            {
                auto res = currentEffect->GetVariableByName("textureOn")->AsScalar()->SetBool(TRUE);
                res = currentEffect->GetVariableByName("Texture")->AsShaderResource()->SetResource(it->texture);
            }
        }
        deviceContext->IASetIndexBuffer(it->indicesBuffer, DXGI_FORMAT_R32_UINT, 0);
        D3DX11_TECHNIQUE_DESC techDesc;
        currentTech->GetDesc(&techDesc);
        for(UINT i = 0; i < techDesc.Passes; ++i)
        {
            currentTech->GetPassByIndex(i)->Apply(0, deviceContext);
            deviceContext->DrawIndexed(it->indices.size(), 0, 0);
        }
    }
}

BOOL RenderSystem::ReadShadersEtc()
{
    ifstream filestr("etc/shaders.txt");
    if(filestr.fail())
    {
        MessageBox(NULL,TEXT("读取着色器配置文件失败"),TEXT("ERROR"),MB_OK);
        return FALSE;
    }
    stringstream  buffer;
    buffer<<filestr.rdbuf();
    string line;
    while(buffer>>line)
    {
        shadersName.push_back(line);
    }
    filestr.close();
    return TRUE;
}