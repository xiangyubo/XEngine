#include "TerrainShader.h"
#include "Vertice.h"
#include "RenderSystem.h"
#include "Utility.h"
#include "Light.h"
#include "Terrain.h"
using namespace std;

TerrainShader::TerrainShader()
{
	currentEffect = NULL;
	currentTech = NULL;
	m_sampleState = NULL;
}

TerrainShader::TerrainShader(const TerrainShader& other)
{
}

TerrainShader::~TerrainShader()
{
    Shutdown();
}

bool TerrainShader::Init(ID3DX11Effect *effect, ID3DX11EffectTechnique *tech)
{
	bool result;
    currentEffect = effect;
    currentTech = tech;
	
	result = InitializeShader();
	if(!result)
	{
		return false;
	}

	return true;
}

void TerrainShader::Shutdown()
{
	ShutdownShader();
}

ID3DX11Effect* TerrainShader::GetCurrentEffect()
{
    return currentEffect;
}

void TerrainShader::SetSadowParameters(BOOL shadowFlag, ID3D11ShaderResourceView *shadowMap, XMMATRIX &lightView, XMMATRIX &lightProj)
{
    currentEffect->GetVariableByName("shadowFlag")->AsScalar()->SetBool(shadowFlag);
    if(shadowFlag == TRUE)
    {
        //ÉèÖÃÒõÓ°ÌùÍ¼
        currentEffect->GetVariableByName("ShadowMapView")->AsMatrix()->SetMatrix((float*)&lightView);
        currentEffect->GetVariableByName("ShadowMapProjection")->AsMatrix()->SetMatrix((float*)&lightProj);
        currentEffect->GetVariableByName("shadowMap")->AsShaderResource()->SetResource(shadowMap);
    }
}

bool TerrainShader::Render(Terrain *terrain)
{
	bool result;
    UINT stride = InputLayouts::pNTStride;
    UINT offset = 0;

    RenderSystem::GetInstance()->GetDeviceContext()->IASetInputLayout(InputLayouts::PosNormalTex);
	RenderSystem::GetInstance()->GetDeviceContext()->PSSetSamplers(0, 1, &m_sampleState);
    RenderSystem::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, &terrain->GetVerticeBuffer(), &stride, &offset);
    RenderSystem::GetInstance()->GetDeviceContext()->IASetIndexBuffer(terrain->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
    RenderSystem::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    D3DX11_TECHNIQUE_DESC techDesc;
    currentTech->GetDesc(&techDesc);
    for(UINT i = 0; i < techDesc.Passes; ++i)
    {
        currentTech->GetPassByIndex(i)->Apply(0, RenderSystem::GetInstance()->GetDeviceContext());
        RenderSystem::GetInstance()->GetDeviceContext()->DrawIndexed(terrain->GetIndexCount(), 0, 0);
    }

	return true;
}

bool TerrainShader::InitializeShader()
{
	HRESULT result;
    D3D11_SAMPLER_DESC samplerDesc;

	// Create a texture sampler state description.
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    result = RenderSystem::GetInstance()->getDevice()->CreateSamplerState(&samplerDesc, &m_sampleState);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void TerrainShader::ShutdownShader()
{
    SafeRelease(m_sampleState);
	return;
}

void TerrainShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool TerrainShader::SetShaderParameters(const XMMATRIX &world, XMMATRIX &view, XMMATRIX &proj, 
									    const Light *light, 
                                        ID3D11ShaderResourceView *texture)
{
    currentEffect->GetVariableByName("World")->AsMatrix()->SetMatrix((float*)&world);
    currentEffect->GetVariableByName("View")->AsMatrix()->SetMatrix((float*)&view);
    currentEffect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float*)&proj);

    ID3DX11EffectVectorVariable *pLightDir = currentEffect->GetVariableByName("lightDirection")->AsVector();
    ID3DX11EffectVectorVariable *pLightAmb = currentEffect->GetVariableByName("ambientColor")->AsVector();
    ID3DX11EffectVectorVariable *pLightDif = currentEffect->GetVariableByName("diffuseColor")->AsVector();
	
    pLightDir->SetFloatVector((float*)&light->direction);
    pLightAmb->SetFloatVector((float*)&light->ambient);
    pLightDif->SetFloatVector((float*)&light->diffuse);
    
    currentEffect->GetVariableByName("Texture")->AsShaderResource()->SetResource(texture);
	return true;
}