#ifndef __TERRAIN_SHADER__
#define __TERRAIN_SHADER__

#include "PreHeader.h"
class Light;
class Terrain;

class TerrainShader
{
public:
	TerrainShader();
	TerrainShader(const TerrainShader&);
	~TerrainShader();

	bool Init(ID3DX11Effect*, ID3DX11EffectTechnique* );
	void Shutdown();
    bool SetShaderParameters(const XMMATRIX &world, XMMATRIX &view, XMMATRIX &proj, 
									    const Light *light, 
                                        ID3D11ShaderResourceView *texture);
	bool Render(Terrain *);
    void SetSadowParameters(BOOL , ID3D11ShaderResourceView *, XMMATRIX &, XMMATRIX &);
    ID3DX11Effect* GetCurrentEffect();
private:
	bool InitializeShader();
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	

private:

    ID3DX11Effect           *currentEffect;
    ID3DX11EffectTechnique  *currentTech;
	ID3D11SamplerState      *m_sampleState;
};

#endif