#ifndef __RENDERABLE__
#define __RENDERABLE__

#include "PreHeader.h"
#include "Vertice.h"
#include "Light.h"
#include "Utility.h"
#include "Geometry.h"
#include "Input.h"

class RenderableThing
{
public:
    enum RenderType
    {
        PARTICLE = 0,
        OBJ = 1,
        MODEL,
        SKELETON
    };

    typedef shared_ptr<RenderableThing> RenVal;
protected:
    RenderType      type;
    ID3D11Buffer    *verticesBuffer;    //顶点缓冲
    ID3D11Buffer    *indicesBuffer;     //索引缓冲
    ID3D11Buffer    *shadowMapVerticesBuffer;   //阴影贴图顶点缓冲
    XMFLOAT3        *shadowMapVertices; //阴影贴图的顶点

    float           x, y, z;    //世界坐标
    XMFLOAT4X4      world;  //世界矩阵
    XMFLOAT4X4      move;   //平移矩阵
    XMFLOAT4X4      rotate; //旋转矩阵
    float           scale;  //放缩比例

    XMFLOAT4X4      pworld;  //父节点世界矩阵
    XMFLOAT4X4      protate; //父节点旋转矩阵
    float           pscale;  //父节点放缩比例

    RenderableThing*parent;
    vector<RenVal>  children;

    BOOL            isHandleInput;
public:
    RenderableThing()
    {
        verticesBuffer = NULL;
        indicesBuffer = NULL;
        shadowMapVerticesBuffer = NULL;
        shadowMapVertices = NULL;
        parent = NULL;

        x = 1;
        y = 1;
        z = 1;
        XMStoreFloat4x4(&world, XMMatrixIdentity());
        XMStoreFloat4x4(&rotate, XMMatrixIdentity());

        XMStoreFloat4x4(&pworld, XMMatrixIdentity());
        XMStoreFloat4x4(&protate, XMMatrixIdentity());
        XMStoreFloat4x4(&move, XMMatrixIdentity());

        scale = 1.0f;
        pscale = 1.0f;
        isHandleInput = FALSE;
    }

    virtual ~RenderableThing()
    {
        SafeRelease(verticesBuffer);
        SafeRelease(indicesBuffer);
        SafeRelease(shadowMapVerticesBuffer);
        ReleasNewArray(shadowMapVertices);
        parent = NULL;
    }

    //添加子对象
    void AddChild(RenderableThing *child)
    {
        child->parent = this;
        children.push_back(RenVal(child));
    }

    //添加子对象
    void AddChild(RenVal child)
    {
        child->parent = this;
        children.push_back(child);
    }

    //获取对象类型
    RenderType GetRenderType()
    {
        return type;
    }

    //设置世界中的位置
    void SetPosition(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
        XMMATRIX temp = XMMATRIX(1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            x,y,z,1);
        XMStoreFloat4x4(&world, temp);
    }

    //获取世界矩阵
    const XMMATRIX GetWorldMatrix() const
    {
        XMMATRIX temp = XMLoadFloat4x4(&move)*XMLoadFloat4x4(&rotate);
        temp = XMMatrixMultiply(temp, XMLoadFloat4x4(&world));
        temp = XMLoadFloat4x4(&pworld) * temp;
        float   ts = pscale * scale;
        XMStoreFloat4x4(const_cast<XMFLOAT4X4*>(&move), XMMatrixIdentity());
        XMStoreFloat4x4(const_cast<XMFLOAT4X4*>(&rotate), XMMatrixIdentity());
        XMStoreFloat4x4(const_cast<XMFLOAT4X4*>(&world), temp);
        return XMMatrixScaling(ts, ts, ts) * temp;
    }

    //获得世界坐标
    Position GetWorldPosition()
    {
        return Position(x, z);
    }

    //获取顶点缓冲
    ID3D11Buffer*const& GetVerticeBuffer() const
    {
        return verticesBuffer;
    }

    //获取索引缓冲
    ID3D11Buffer*const& GetIndexBuffer() const
    {
        return indicesBuffer;
    }

    //获取阴影贴图顶点缓冲
    ID3D11Buffer*const& GetShadowMapVerticeBuffer() const
    {
        return shadowMapVerticesBuffer;
    }

    //绕x轴转动，以Pi为单位
    void Pitch(float x)
    {
        XMMATRIX temp = XMMatrixMultiply(XMLoadFloat4x4(&rotate), XMMatrixRotationX(x));
        XMStoreFloat4x4(&rotate, temp);
    }

    //绕y轴转动，以Pi为单位
    void Yaw(float y)
    {
        XMMATRIX temp = XMMatrixMultiply(XMLoadFloat4x4(&rotate), XMMatrixRotationY(y));
        XMStoreFloat4x4(&rotate, temp);
    }

    //绕z轴转动，以Pi为单位
    void Roll(float z)
    {
        XMMATRIX temp = XMMatrixMultiply(XMLoadFloat4x4(&rotate), XMMatrixRotationZ(z));
        XMStoreFloat4x4(&rotate, temp);
    }

    //设置放缩比例
    void SetScale(float s)
    {
        scale = s;
    }

    //更新逻辑
    virtual void Update(float time) = 0;

    //更新自己相对于父亲的世界矩阵，旋转矩阵，缩放比例
    void _UpdateByFather(float time)
    {
        for(auto it = children.begin(); it != children.end(); ++it)
        {
            XMStoreFloat4x4(&(*it)->pworld, GetWorldMatrix());
            (*it)->pscale = pscale * scale;
            (*it)->Update(time);
            (*it)->_UpdateByFather(time);
        }
    }

    //得到自己所有的孩子列表
    vector<RenVal> GetChildren()
    {
        return children;
    }

    //添加被观察的输入
    void AddObserveTarget(Input *input)
    {
        input->AddObserver(this);
    }

    void DeleteObserveTarget(Input *input)
    {
        input->DeleteObserver(this);
    }

    //设置是否处理输入
    void SetHandleInput(BOOL flag)
    {
        isHandleInput = flag;
    }

    //检查是否处理输入
    BOOL IsHandleInput()
    {
        return isHandleInput;
    }

    //处理输入信息
    virtual void HandleInput(Input *input)
    {
        
    }

protected:
    //释放函数
    virtual void Release() = 0;

    //创建缓冲
    virtual BOOL InitBuffers() = 0;

    //组合阴影贴图顶点缓冲
    virtual void AssembleShadowMapVertices() = 0;
};

#endif