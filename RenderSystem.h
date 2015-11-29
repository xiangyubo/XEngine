#ifndef __RENDER__
#define __RENDER__

#include "PreHeader.h"
#include "RenderableThing.h"
class Obj;
class Model;
class SkyBox;
class Skeleton;
class Particle;
class EffectMgr;
class Terrain;
class RenderTexture;
class TerrainShader;

struct Material;
struct Light;

class RenderSystem
{
private:
    typedef uint64_t                    Key;
    typedef shared_ptr<RenderableThing> Val;
    typedef unordered_map<Key, Val>     RenderTable;

private:
    //必要的d3d设备
    ID3D11Device            *device;
    IDXGISwapChain          *swapChain;
	ID3D11DeviceContext     *deviceContext;
	ID3D11RenderTargetView  *renderTargetView;

    EffectMgr               *effectMgr;
    vector<string>          shadersName;

    ID3DX11Effect           *lightEffect;    //光照渲染效果
    ID3DX11EffectTechnique  *directionLightTech;  //平行光渲染器
    ID3DX11EffectTechnique  *pointLightTech;    //点光源渲染器
    ID3DX11EffectTechnique  *spotLightTech; //聚光光源渲染器

    ID3DX11Effect           *shadowMapEffect;   //阴影贴图渲染效果
    ID3DX11EffectTechnique  *shadowMapTech; //阴影贴图渲染器

    ID3DX11Effect           *testEffect;    //测试渲染效果
    ID3DX11EffectTechnique  *testTech;      //测试渲染器

    ID3DX11Effect           *currentEffect; //当前使用的渲染效果
    ID3DX11EffectTechnique  *currentTech;   //当前使用的渲染器
    ID3D11InputLayout       *currentLayout; //当前使用的顶点布局

    ID3D11Texture2D         *depthStencilBuffer;
    ID3D11DepthStencilView  *depthStencilView;  //深度模板视图
    ID3D11DepthStencilState *depthStencilState; //深度测试状态

    ID3D11Texture2D         *shadowMapBuffer;   //阴影贴图缓冲
    ID3D11DepthStencilView  *DS_SMView;  //深度模板-阴影贴图视图
    ID3D11ShaderResourceView *shadowMapView;     //阴影贴图视图

    RenderTexture           *renderTexture;

    XMFLOAT4    eyePos;
    BOOL        shadowFlag;

    XMMATRIX    lightView;
    XMMATRIX    lightProj;

    Light                   _light;
    XMMATRIX                _view;
    XMMATRIX                _proj;
    TerrainShader           *terrainShader;

    int         width;   //窗口初始化宽
    int         height;  //窗口初始化高
    UINT        msaaQuality;    //最高渲染质量级别

    vector<const Obj*>   objs;
    vector<const Model*> models;
    vector<const Skeleton*> skeletons;
    vector<Particle*> particles;
public:
    ~RenderSystem(){};
    
    //单例   
    static RenderSystem* GetInstance();
    
    BOOL Init(HWND hwnd);   

    void Shutdown();
    
    //对外渲染物体序列
    void DrawRenderables(const vector<Val>& );

    //设置当前的Effect、Tech、Layout
    void SetCurrentETL(ID3DX11Effect *, ID3DX11EffectTechnique *, ID3D11InputLayout *);

    //是否打开阴影效果
    void SetShadowOpen(BOOL );

    //渲染天空
    void DrawSky(const SkyBox * , XMMATRIX &);

    //渲染地形
    void DrawTerrain(Terrain *);

    void SetMaterial(const Material* material);

    void SetDepthTest();

    void SetLight(const Light* );

    void SetProjection(const XMMATRIX& );

    void SetView(const XMMATRIX& );

    void SetEye(const XMVECTOR& );

    BOOL CreateBuffer(const D3D11_BUFFER_DESC &, const D3D11_SUBRESOURCE_DATA &, ID3D11Buffer* &);

    void ClearBackground();

    void ClearDepthBuffer(ID3D11DepthStencilView *);

    void Present();

    ID3D11Device* getDevice();

    ID3D11DeviceContext* GetDeviceContext();

private:
    //读取配置文件
    BOOL ReadShadersEtc();

    //初始化D3D设备
    BOOL InitD3D(HWND hwnd);

    //初始化深度模板缓存
    BOOL InitDepthStencilView(DXGI_FORMAT format);

    //初始化阴影贴图视图
    BOOL InitShadowMapView(DXGI_FORMAT format);

    void ShutdownD3D();
    void ShutdownShaders();
    void ShutdownDepthStencil();

    //渲染阴影贴图
    void _DrawShadowMap();

    //正常渲染物品
    void _DrawRenderables();

    //初始化灯光的视觉矩阵和投影矩阵
    void InitLightVP(const Light* light);

    //设置灯光的视觉矩阵和投影矩阵
    void SetLightVP();

    //设置粒子相关参数
    void SetParticleArgv(Particle*);

    //底层渲染粒子效果
    void _DrawParticles();

    //底层渲染一个物体
    void _DrawObj(const Obj* );

    //底层渲染一个模型
    void _DrawModel(const Model* , bool );

    //底层渲染一个骨骼
    void _DrawSkeleton(const Skeleton*, bool );

    RenderSystem();
};

#endif