#ifndef __TERRAIN__
#define __TERRAIN__

#include "PreHeader.h"
#include "Obj.h"
#include <d3dx10math.h>
using VERTICE::PosNormalTex;

class Texture;

class Terrain
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR3 normal;
        D3DXVECTOR2 texture;
	};

	struct HeightMapType 
	{ 
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	Terrain();
	Terrain(const Terrain&);
	~Terrain();

	bool Init(const string &hightMapFile, const string &path, const string &fileName);
	void Shutdown();

    ID3D11Buffer*const& GetVerticeBuffer() const;
    ID3D11Buffer*const& GetIndexBuffer() const;
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
    const XMMATRIX GetWorldMatrix() const;
private:
    //加载高度图并计算各顶点高度
	bool LoadHeightMap(const string &);

    //归一化高度
	void NormalizeHeightMap();

    //计算各点点法线向量
	bool CalculateNormals();

    //销毁高度数组
	void ShutdownHeightMap();

    //计算纹理坐标
	void CalculateTextureCoordinates();

    //加载地形纹理
	bool LoadTexture(const string &path, const string &filename);
	
    //释放地形纹理
    void ReleaseTexture();
    
    //创建顶点缓冲和索引缓冲
	bool InitializeBuffers(ID3D11Device*);

    //销毁缓冲
	void ShutdownBuffers();
	
private:
	int terrainWidth, terrainHeight;
	int vertexCount, indexCount;
	ID3D11Buffer *vertexBuffer, *indexBuffer;
	HeightMapType* heightMap;
	Texture *texture;
};

#endif