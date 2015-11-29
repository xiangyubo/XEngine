#include "Obj.h"
#include "Utility.h"
#include "RenderSystem.h"

Obj::Obj()
{
    type = OBJ;
    vertices = NULL;
    indices = NULL;
    
    vcount = 0;
    icount = 0;
    
    ZeroMemory(&mate, sizeof(mate));
    vertCount = 0;
}

Obj::~Obj()
{
    vertCount = 0;
    ReleasNewArray(vertices);
    ReleasNewArray(indices);

    SafeRelease(verticesBuffer);
    SafeRelease(indicesBuffer);
    SafeRelease(shadowMapVerticesBuffer);
}

void Obj::CreateTestTriangle()
{
    Release();

    vertices = new PosNormalTexTan[3];
    indices = new DWORD[3];
    vcount = 3;
    icount = 3;

    Material material;
    material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.power = 5.0f;
    mate = material;

    vertices[0].pos = XMFLOAT3(0.0f, 0.5f, 0.0f);
    vertices[0].normal = XMFLOAT3(0.f,0.f,-1.f);
    vertices[0].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[0].tex = XMFLOAT2(0.f,1.f);

    vertices[1].pos = XMFLOAT3(0.23f, -0.5, 0.0f);
    vertices[1].normal = XMFLOAT3(0.f,0.f,-1.f);
    vertices[1].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[1].tex = XMFLOAT2(0.f,0.f);

    vertices[2].pos = XMFLOAT3(-0.45f, -0.65f, 0.0f);
    vertices[2].normal = XMFLOAT3(0.f,0.f,-1.f);
    vertices[2].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[2].tex = XMFLOAT2(1.f,0.f);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    
    InitBuffers();
}

void Obj::CreateBox(float width, float height, float depth, const Material& m)
{
    Release();

    vertices = new PosNormalTexTan[24];
    indices = new DWORD[36];
    vcount = 24;
    icount = 36;

    mate = m;

    float halfW = width * 0.5f;
    float halfH = height * 0.5f;
    float halfD = depth * 0.5f;

    //front
    vertices[0].pos = XMFLOAT3(-halfW,-halfH,-halfD);
    vertices[0].normal = XMFLOAT3(0.f,0.f,-1.f);
    vertices[0].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[0].tex = XMFLOAT2(0.f,1.f);
    vertices[1].pos = XMFLOAT3(-halfW,halfH,-halfD);
    vertices[1].normal = XMFLOAT3(0.f,0.f,-1.f);
    vertices[1].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[1].tex = XMFLOAT2(0.f,0.f);
    vertices[2].pos = XMFLOAT3(halfW,halfH,-halfD);
    vertices[2].normal = XMFLOAT3(0.f,0.f,-1.f);
    vertices[2].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[2].tex = XMFLOAT2(1.f,0.f);
    vertices[3].pos = XMFLOAT3(halfW,-halfH,-halfD);
    vertices[3].normal = XMFLOAT3(0.f,0.f,-1.f);
    vertices[3].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[3].tex = XMFLOAT2(1.f,1.f);
    //left
    vertices[4].pos = XMFLOAT3(-halfW,-halfH,halfD);
    vertices[4].normal = XMFLOAT3(-1.f,0.f,0.f);
    vertices[4].tangent = XMFLOAT3(0.f,0.f,-1.f);
    vertices[4].tex = XMFLOAT2(0.f,1.f);
    vertices[5].pos = XMFLOAT3(-halfW,halfH,halfD);
    vertices[5].normal = XMFLOAT3(-1.f,0.f,0.f);
    vertices[5].tangent = XMFLOAT3(0.f,0.f,-1.f);
    vertices[5].tex = XMFLOAT2(0.f,0.f);
    vertices[6].pos = XMFLOAT3(-halfW,halfH,-halfD);
    vertices[6].normal = XMFLOAT3(-1.f,0.f,0.f);
    vertices[6].tangent = XMFLOAT3(0.f,0.f,-1.f);
    vertices[6].tex = XMFLOAT2(1.f,0.f);
    vertices[7].pos = XMFLOAT3(-halfW,-halfH,-halfD);
    vertices[7].normal = XMFLOAT3(-1.f,0.f,0.f);
    vertices[7].tangent = XMFLOAT3(0.f,0.f,-1.f);
    vertices[7].tex = XMFLOAT2(1.f,1.f);
    //back
    vertices[8].pos = XMFLOAT3(halfW,-halfH,halfD);
    vertices[8].normal = XMFLOAT3(0.f,0.f,1.f);
    vertices[8].tangent = XMFLOAT3(-1.f,0.f,0.f);
    vertices[8].tex = XMFLOAT2(0.f,1.f);
    vertices[9].pos = XMFLOAT3(halfW,halfH,halfD);
    vertices[9].normal = XMFLOAT3(0.f,0.f,1.f);
    vertices[9].tangent = XMFLOAT3(-1.f,0.f,0.f);
    vertices[9].tex = XMFLOAT2(0.f,0.f);
    vertices[10].pos = XMFLOAT3(-halfW,halfH,halfD);
    vertices[10].normal = XMFLOAT3(0.f,0.f,1.f);
    vertices[10].tangent = XMFLOAT3(-1.f,0.f,0.f);
    vertices[10].tex = XMFLOAT2(1.f,0.f);
    vertices[11].pos = XMFLOAT3(-halfW,-halfH,halfD);
    vertices[11].normal = XMFLOAT3(0.f,0.f,1.f);
    vertices[11].tangent = XMFLOAT3(-1.f,0.f,0.f);
    vertices[11].tex = XMFLOAT2(1.f,1.f);
    //right
    vertices[12].pos = XMFLOAT3(halfW,-halfH,-halfD);
    vertices[12].normal = XMFLOAT3(1.f,0.f,0.f);
    vertices[12].tangent = XMFLOAT3(0.f,0.f,1.f);
    vertices[12].tex = XMFLOAT2(0.f,1.f);
    vertices[13].pos = XMFLOAT3(halfW,halfH,-halfD);
    vertices[13].normal = XMFLOAT3(1.f,0.f,0.f);
    vertices[13].tangent = XMFLOAT3(0.f,0.f,1.f);
    vertices[13].tex = XMFLOAT2(0.f,0.f);
    vertices[14].pos = XMFLOAT3(halfW,halfH,halfD);
    vertices[14].normal = XMFLOAT3(1.f,0.f,0.f);
    vertices[14].tangent = XMFLOAT3(0.f,0.f,1.f);
    vertices[14].tex = XMFLOAT2(1.f,0.f);
    vertices[15].pos = XMFLOAT3(halfW,-halfH,halfD);
    vertices[15].normal = XMFLOAT3(1.f,0.f,0.f);
    vertices[15].tangent = XMFLOAT3(0.f,0.f,1.f);
    vertices[15].tex = XMFLOAT2(1.f,1.f);
    //top
    vertices[16].pos = XMFLOAT3(-halfW,halfH,-halfD);
    vertices[16].normal = XMFLOAT3(0.f,1.f,0.f);
    vertices[16].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[16].tex = XMFLOAT2(0.f,1.f);
    vertices[17].pos = XMFLOAT3(-halfW,halfH,halfD);
    vertices[17].normal = XMFLOAT3(0.f,1.f,0.f);
    vertices[17].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[17].tex = XMFLOAT2(0.f,0.f);
    vertices[18].pos = XMFLOAT3(halfW,halfH,halfD);
    vertices[18].normal = XMFLOAT3(0.f,1.f,0.f);
    vertices[18].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[18].tex = XMFLOAT2(1.f,0.f);
    vertices[19].pos = XMFLOAT3(halfW,halfH,-halfD);
    vertices[19].normal = XMFLOAT3(0.f,1.f,0.f);
    vertices[19].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[19].tex = XMFLOAT2(1.f,1.f);
    //bottom
    vertices[20].pos = XMFLOAT3(-halfW,-halfH,halfD);
    vertices[20].normal = XMFLOAT3(0.f,-1.f,0.f);
    vertices[20].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[20].tex = XMFLOAT2(0.f,1.f);
    vertices[21].pos = XMFLOAT3(-halfW,-halfH,-halfD);
    vertices[21].normal = XMFLOAT3(0.f,-1.f,0.f);
    vertices[21].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[21].tex = XMFLOAT2(0.f,0.f);
    vertices[22].pos = XMFLOAT3(halfW,-halfH,-halfD);
    vertices[22].normal = XMFLOAT3(0.f,-1.f,0.f);
    vertices[22].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[22].tex = XMFLOAT2(1.f,0.f);
    vertices[23].pos = XMFLOAT3(halfW,-halfH,halfD);
    vertices[23].normal = XMFLOAT3(0.f,-1.f,0.f);
    vertices[23].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[23].tex = XMFLOAT2(1.f,1.f);

    //Construct index
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    indices[6] = 4;
    indices[7] = 5;
    indices[8] = 6;
    indices[9] = 4;
    indices[10] = 6;
    indices[11] = 7;

    indices[12] = 8;
    indices[13] = 9;
    indices[14] = 10;
    indices[15] = 8;
    indices[16] = 10;
    indices[17] = 11;

    indices[18] = 12;
    indices[19] = 13;
    indices[20] = 14;
    indices[21] = 12;
    indices[22] = 14;
    indices[23] = 15;

    indices[24] = 16;
    indices[25] = 17;
    indices[26] = 18;
    indices[27] = 16;
    indices[28] = 18;
    indices[29] = 19;

    indices[30] = 20;
    indices[31] = 21;
    indices[32] = 22;
    indices[33] = 20;
    indices[34] = 22;
    indices[35] = 23;

    InitBuffers();
}

void Obj::CreateGrid(float width, float height, UINT m, UINT n, const Material &material)
{
    Release();

    mate = material;

    UINT32 nVertsRow = m + 1;
    UINT32 nVertsCol = n + 1;

    float oX = -width * 0.5f;
    float oZ = height * 0.5f;

    float dx = width / m;
    float dz = height /n;
    
    float dxTex = 1.f / m;
    float dyTex = 1.f /n;

    vcount = nVertsRow * nVertsCol;
    vertices = new PosNormalTexTan[vcount];

    for(UINT i=0; i<nVertsCol; ++i)
    {
        float tmpZ = oZ - dz * i;
        for(UINT j=0; j<nVertsRow; ++j)
        {
            UINT index = nVertsRow * i + j;
            vertices[index].pos.x = oX + dx * j;
            vertices[index].pos.y = 0.f;
            vertices[index].pos.z = tmpZ;

            vertices[index].normal = XMFLOAT3(0.f,1.f,0.f);
            vertices[index].tangent = XMFLOAT3(1.f,0.f,0.f);

            vertices[index].tex = XMFLOAT2(dxTex*j,dyTex*i);
        }
    }

    UINT nIndices = m * n * 6;
    icount = nIndices;
    indices = new DWORD[nIndices];
    UINT tmp = 0;
    for(UINT i=0; i<n; ++i)
    {
        for(UINT j=0; j<m; ++j)
        {
            indices[tmp] = i * nVertsRow + j;
            indices[tmp+1] = i * nVertsRow + j + 1;
            indices[tmp+2] = (i + 1) * nVertsRow + j;
            indices[tmp+3] = i * nVertsRow + j + 1;
            indices[tmp+4] = (i + 1) * nVertsRow + j + 1;
            indices[tmp+5] = (i + 1) * nVertsRow + j;

            tmp += 6;
        }
    }

    InitBuffers();
}

void Obj::CreateSphere(float radius, int slice, int stack, const Material &m)
{
    Release();

    mate = m;

    int vertsPerRow = slice + 1;
    int nRows = stack - 1;
    int nVerts = vertsPerRow * nRows + 2;
    int nIndices = (nRows-1)*slice*6 + slice * 6;

    vertices = new PosNormalTexTan[nVerts];
    indices = new DWORD[nIndices];
    vcount = nVerts;
    icount = nIndices;

    for(int i=1; i<=nRows; ++i)
    {
        float phy = XM_PI * i / stack;
        float tmpRadius = radius * sin(phy);
        for(int j=0; j<vertsPerRow; ++j)
        {
            float theta = XM_2PI * j / slice;
            UINT index = (i-1)*vertsPerRow+j;

            float x = tmpRadius*cos(theta);
            float y = radius*cos(phy);
            float z = tmpRadius*sin(theta);

            //Position
            vertices[index].pos = XMFLOAT3(x,y,z);
            //Normal
            XMVECTOR N = XMVectorSet(x,y,z,0.f);
            XMStoreFloat3(&vertices[index].normal,XMVector3Normalize(N));
            //Tangent
            XMVECTOR T = XMVectorSet(-sin(theta),0.f,cos(theta),0.f);
            XMStoreFloat3(&vertices[index].tangent,XMVector3Normalize(T));
            //Texcoord
            vertices[index].tex = XMFLOAT2(j*1.f/slice,i*1.f/stack);
        }
    }

    int size = vertsPerRow * nRows;
    //Two vertex for top and bottom
    vertices[size].pos = XMFLOAT3(0.f,radius,0.f);
    vertices[size].normal = XMFLOAT3(0.f,1.f,0.f);
    vertices[size].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[size].tex = XMFLOAT2(0.f,0.f);

    vertices[size+1].pos = XMFLOAT3(0.f,-radius,0.f);
    vertices[size+1].normal = XMFLOAT3(0.f,-1.f,0.f);
    vertices[size+1].tangent = XMFLOAT3(1.f,0.f,0.f);
    vertices[size+1].tex = XMFLOAT2(0.f,1.f);


    //Begin construct index
    UINT tmp(0);
    int start1 = 0;
    int start2 = vcount - vertsPerRow - 2;
    int top = size;
    int bottom = size + 1;
    for(int i=0; i<slice; ++i)
    {
        indices[tmp] = top;
        indices[tmp+1] = start1+i+1;
        indices[tmp+2] = start1+i;

        tmp += 3;
    }

    for(int i=0; i<slice; ++i)
    {
        indices[tmp] = bottom;
        indices[tmp+1] = start2 + i;
        indices[tmp+2] = start2 + i + 1;

        tmp += 3;
    }

    for(int i=0; i<nRows-1; ++i)
    {
        for(int j=0; j<slice; ++j)
        {
            indices[tmp] = i * vertsPerRow + j;
            indices[tmp+1] = (i + 1) * vertsPerRow + j + 1;
            indices[tmp+2] = (i + 1) * vertsPerRow + j;
            indices[tmp+3] = i * vertsPerRow + j;
            indices[tmp+4] = i * vertsPerRow + j + 1;
            indices[tmp+5] = (i + 1) * vertsPerRow + j + 1;

            tmp += 6;
        }
    }

    InitBuffers();
}

BOOL Obj::InitBuffers()
{
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(PosNormalTexTan) * vcount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = &vertices[0];
    if( RenderSystem::GetInstance()->CreateBuffer(bd, initData, verticesBuffer) == FALSE)
    {
        MessageBox(NULL, L"¥¥Ω®∂•µ„ª∫¥Ê ß∞‹", L"ERROR", MB_OK);
        Release();
        return FALSE;
    }

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(DWORD) * icount;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    initData.pSysMem = &indices[0];
    if( RenderSystem::GetInstance()->CreateBuffer(bd, initData, indicesBuffer) == FALSE)
    {
        MessageBox(NULL, L"¥¥Ω®À˜“˝ª∫¥Ê ß∞‹", L"ERROR", MB_OK);
        Release();
        return FALSE;
    }

    AssembleShadowMapVertices();
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(XMFLOAT3) * vcount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    initData.pSysMem = &shadowMapVertices[0];
    if( RenderSystem::GetInstance()->CreateBuffer(bd, initData, shadowMapVerticesBuffer) == FALSE)
    {
        MessageBox(NULL, L"¥¥Ω®“ı”∞Ã˘Õº∂•µ„ª∫¥Ê ß∞‹", L"ERROR", MB_OK);
        Release();
        return FALSE;
    }

    vertCount = icount;
    ReleasNewArray(vertices);
    ReleasNewArray(indices);
    return TRUE;
}

void Obj::SetMaterial(const Material &material)
{
    mate = material;
}

const Material& Obj::GetMaterial() const
{
    return mate;
}

void Obj::Update(float time)
{
    
}

UINT32 Obj::GetVerticeCount() const
{
    return vertCount;
}

void Obj::Release()
{
    vertCount = 0;
    ReleasNewArray(vertices);
    ReleasNewArray(indices);
    ReleasNewArray(shadowMapVertices);

    SafeRelease(verticesBuffer);
    SafeRelease(indicesBuffer);
    SafeRelease(shadowMapVerticesBuffer);
}

void Obj::AssembleShadowMapVertices()
{
    shadowMapVertices = new XMFLOAT3[vcount];
    for(int i = 0; i < vcount; ++i)
    {
        shadowMapVertices[i] = vertices[i].pos;
    }
}