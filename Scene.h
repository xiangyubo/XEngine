#ifndef __SCENE__
#define __SCENE__

#include "PreHeader.h"
#include "SkyBox.h"
#include "Input.h"
#include "Light.h"
#include "Geometry.h"
#include "Terrain.h"
class Camera;
class QTree;

class Scene
{
public:
    Camera              *camera; //通过照相机获得视图矩阵
    XMFLOAT4X4          proj;   //投影矩阵
protected:
    typedef uint64_t            Key;
    typedef shared_ptr<RenderableThing> Val;
    typedef unordered_map<Key, Val>     RenderTable;

    Input               input;
    RenderTable         renderTable;    //场景中所有可渲染的物件
    vector<Val>         visiableTable;  //场景中可见物体
    shared_ptr<SkyBox>  skyBox;         //天空盒
    shared_ptr<Terrain> terrain;        //地形
    shared_ptr<Light>   light;
    QTree               *qTree;     //四叉树，用于可见性测试
public:
    Scene();
    virtual ~Scene();

    void Init(const Rect &);

    //添加一个场景中的物体
    void AddRenderableThing(RenderableThing& );
 
    //删除一个场景中的物体
    void DeleteRenderableThing(RenderableThing& );

    //添加天空盒子
    void AddSky(SkyBox* );

    //添加地形
    void AddTerrain(Terrain *);

    //跟新场景中的物体
    virtual void Update(double time);

    //处理输入信息
    virtual void HandleInput(Input *);

    //添加灯光
    void AddLight(Light *);

    //渲染场景中的物体
    void Draw();

    void Release();

private:
    //能否被看见
    void VisibleTest();

    //展开物体和它的子节点
    void UnfoldChildren(Val& );
};

#endif