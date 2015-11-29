#ifndef __SRVBATCH__
#define __SRVBATCH__

#include "PreHeader.h"

class SRVBatch
{
private:
    vector<ID3D11ShaderResourceView*>   shaderResourceViewTable;
    map<string, int>    name2IndexTable;
    static ID3D11ShaderResourceView *m_pRandomTexture;
public:
    ~SRVBatch();
    
    //单例
    static SRVBatch* GetInstance();

    //添加一个渲染资源视图
    int AddSRV(const string& , ID3D11ShaderResourceView* );

    //通过名字获取一个渲染资源视图
    ID3D11ShaderResourceView* GetSRVByName(const string& );

    //通过索引获取一个渲染资源视图
    ID3D11ShaderResourceView* GetSRVByIndex(int );

    //加载一个渲染资源视图
    ID3D11ShaderResourceView* LoadSRV(const string& , const string& );

    //生成一个随机的一维纹理
    ID3D11ShaderResourceView* GetRandomSRV1D();

    //清除
    void Clear();

private:
    SRVBatch(){};
};

#endif