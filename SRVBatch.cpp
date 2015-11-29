#include "SRVBatch.h"
#include "Utility.h"
#include "Random.h"
#include "RenderSystem.h"

ID3D11ShaderResourceView* SRVBatch::m_pRandomTexture=NULL;

SRVBatch::~SRVBatch()
{
    for(auto it = shaderResourceViewTable.begin(); it != shaderResourceViewTable.end(); ++it)
    {
        SafeRelease(*it);
    }
    SafeRelease(m_pRandomTexture);
    shaderResourceViewTable.clear();
    shaderResourceViewTable.resize(0);
    name2IndexTable.clear();
}

SRVBatch* SRVBatch::GetInstance()
{
    static SRVBatch instance;
    return &instance;
}

ID3D11ShaderResourceView* SRVBatch::LoadSRV(const string& path, const string& fileName)
{
    if(name2IndexTable.find(fileName) != name2IndexTable.end())
    {
        int index = name2IndexTable.find(fileName)->second;
        return shaderResourceViewTable[index];
    }
    ID3D11ShaderResourceView* temp;
    string tempFile = path + fileName;
    wstring file(tempFile.begin(), tempFile.end());
    auto res = D3DX11CreateShaderResourceViewFromFile( RenderSystem::GetInstance()->getDevice(), file.c_str(),
								NULL, NULL, &temp, NULL );
    if( FAILED(res))
    {
        file += L"创建渲染资源视图失败 " + res;
        MessageBox(NULL, file.c_str(), L"Error", MB_OK);
        return NULL;
    }
    name2IndexTable[fileName] = shaderResourceViewTable.size();
    shaderResourceViewTable.push_back(temp);
    return temp;
}

ID3D11ShaderResourceView* SRVBatch::GetSRVByName(const string &fileName)
{
    if(name2IndexTable.find(fileName) != name2IndexTable.end())
    {
        int index = name2IndexTable.find(fileName)->second;
        return shaderResourceViewTable[index];
    }
    return NULL;
}

ID3D11ShaderResourceView* SRVBatch::GetSRVByIndex(int index)
{
    if(0 <= index && index < shaderResourceViewTable.size())
    {
        return shaderResourceViewTable[index];
    }
    return NULL;
}

ID3D11ShaderResourceView* SRVBatch::GetRandomSRV1D()
{
    if(m_pRandomTexture != NULL)
    {
        return m_pRandomTexture;
    }

    XMFLOAT4 randomValues[1024];

	for(int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = Random::NextFloat(-1.0f, 1.0f);
		randomValues[i].y = Random::NextFloat(-1.0f, 1.0f);
		randomValues[i].z = Random::NextFloat(-1.0f, 1.0f);
		randomValues[i].w = Random::NextFloat(-1.0f, 1.0f);
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = randomValues;
	initData.SysMemPitch = 1024*sizeof(XMFLOAT4);
	initData.SysMemSlicePitch = 1024*sizeof(XMFLOAT4);
	
	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = 1024;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	ID3D11Texture1D* randomTex = 0;
	RenderSystem::GetInstance()->getDevice()->CreateTexture1D(&texDesc, &initData, &randomTex);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	RenderSystem::GetInstance()->getDevice()->CreateShaderResourceView(randomTex, &viewDesc, &m_pRandomTexture);

	return m_pRandomTexture;
}

int SRVBatch::AddSRV(const string& fileName, ID3D11ShaderResourceView* temp)
{
    if(name2IndexTable.find(fileName) != name2IndexTable.end())
    {
        return name2IndexTable[fileName];
    }
    name2IndexTable[fileName] = shaderResourceViewTable.size();
    shaderResourceViewTable.push_back(temp);
    return name2IndexTable[fileName];
}

void SRVBatch::Clear()
{
    for(auto it = shaderResourceViewTable.begin(); it != shaderResourceViewTable.end(); ++it)
    {
        SafeRelease(*it);
    }
    shaderResourceViewTable.clear();
    shaderResourceViewTable.resize(0);
    name2IndexTable.clear();
}