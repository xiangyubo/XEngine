#include "Particle.h"
#include "SRVBatch.h"
#include "Utility.h"
#include "Vertice.h"
#include "RenderSystem.h"

Particle::Particle()
{
    type = PARTICLE;
    m_bFirstRun=TRUE;
	timeStep=0;
	age=0;
	m_nMaxParticles=500;

    drawVB = NULL;
    streamOutVB = NULL;
    texRV = NULL;
    randomTexRV = NULL;
 
	emitDirW = XMFLOAT4(0.0f, 1.0f, 1.0f, 0.0f);
}

Particle::~Particle()
{
    SafeRelease(drawVB);
    SafeRelease(streamOutVB);
    texRV = NULL;
    randomTexRV = NULL;
}

void Particle::Update(float time)
{
    time /= 1000;
    timeStep = time;
    age += time;
}

void Particle::Release()
{
    SafeRelease(drawVB);
    SafeRelease(streamOutVB);
    texRV = NULL;
    randomTexRV = NULL;
}

BOOL Particle::Init(const string &filePath, const string &file)
{
    texRV = SRVBatch::GetInstance()->LoadSRV(filePath, file);
    randomTexRV = SRVBatch::GetInstance()->GetRandomSRV1D();
    
    InitBuffers();
    return TRUE;
}

XMFLOAT4 Particle::GetWorldVertex()
{
    return XMFLOAT4(x, y, z, 1.0f);
}

void Particle::SetEmitDirect(XMFLOAT4 &emitDir)
{
    emitDirW = emitDir;
}

BOOL Particle::InitBuffers()
{
    D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = InputLayouts::pPStride * 1;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	VERTICE::ParticleVertex p;
	ZeroMemory(&p, sizeof(VERTICE::ParticleVertex));
	p.age  = 0.0f;
	p.type = 0; 

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &p;

    RenderSystem::GetInstance()->CreateBuffer(vbd, vinitData, verticesBuffer);

	vbd.ByteWidth = InputLayouts::pPStride * m_nMaxParticles;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

    RenderSystem::GetInstance()->getDevice()->CreateBuffer(&vbd, NULL, &drawVB);
	RenderSystem::GetInstance()->getDevice()->CreateBuffer(&vbd, NULL, &streamOutVB);

    return TRUE;
}

void Particle::AssembleShadowMapVertices()
{
    
}