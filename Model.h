#ifndef __MODEL__
#define __MODEL__

#include "RenderableThing.h"

class RenderSystem;

using VERTICE::PosNormalTexTan;

class Model :public RenderableThing
{
public:
    typedef unordered_map<string, shared_ptr<Material>>     MateTable;
    struct SubMesh
    {
        string  mate;       //材质名称
        int     startIndex; //子集对应的索引开始序号
        int     indexCount; //子集对应的索引数目
    };
protected:
    vector<PosNormalTexTan> vertices;   //顶点
    vector<DWORD>    indices;    //索引
    UINT32          vertCount;

    vector<XMFLOAT3>		poses;          //载入模型时位置
    vector<XMFLOAT3>		normals;        //载入模型时法线
    vector<XMFLOAT2>		texturePoses;   //载入模型时纹理坐标

    string      filePath;   //模型数据路径
    vector<SubMesh> meshs;   //网格信息
    MateTable   mateTable;  //材质表

public:
    Model();
    virtual ~Model();

    //载入模型文件
    BOOL LoadModel(const string &path, const string &fileName);

    //释放资源
    void Release();

    //获取网格信息
    const vector<SubMesh>& GetMeshes() const;

    //获取材质表
    const MateTable& GetMateTable() const;

    virtual void Update(float time);

private:
    // 提取顶点信息（位置、纹理、法线）
	BOOL ParseVertices(const string &fileName);

	// 解析材质文件
	BOOL ParseMaterialFile(const string &fileName);

	// 解析子网格（submesh）结构
	BOOL ParseSubset(const string &fileName);
	
    //初始化缓冲区
    BOOL InitBuffers();

    //组合阴影贴图顶点缓冲
    void Model::AssembleShadowMapVertices();
};

#endif