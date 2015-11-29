#include "Model.h"
#include "RenderSystem.h"
#include "Utility.h"
#include "SRVBatch.h"

Model::Model()
{
    type = MODEL;
    vertCount = 0;
}

Model::~Model()
{
    meshs.clear();
    meshs.resize(0);
}

BOOL Model::LoadModel(const string &path, const string &fileName)
{
    Release();

    filePath = path;
    string fullFileName(path);
    fullFileName += fileName;

    if(ParseVertices(fullFileName) == FALSE)
    {
        return FALSE;
    }
    if(ParseSubset(fullFileName) == FALSE)
    {
        return FALSE;
    }
    if(InitBuffers() == FALSE)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Model::ParseVertices(const string &fileName)
{
    ifstream inFile;
    inFile.open(fileName);
    if(inFile.fail())
    {
        MessageBox(NULL, L"打开模型顶点文件失败", L"ERROR", MB_OK);
        return FALSE;
    }

    string input;
    for(inFile>>input; inFile.eof() != TRUE; inFile>>input)
    {
        if(input == "v")
        {
            XMFLOAT3 pos;
            inFile>>pos.x;
            inFile>>pos.y;
            inFile>>pos.z;

            poses.push_back( pos );
        }
        else if(input == "vt")
        {
            XMFLOAT2 tc;
            inFile>>tc.x;
            inFile>>tc.y;

            texturePoses.push_back( tc );
        }
        else if(input == "vn")
        {
            XMFLOAT3 nor;
            inFile>>nor.x;
            inFile>>nor.y;
            inFile>>nor.z;

            normals.push_back( nor );
        }
        else if(input == "mtllib")
        {
            // 解析材质文件
            string matFileName;
            inFile>>matFileName;
            if(ParseMaterialFile(matFileName) == FALSE)
            {
                return FALSE;
            }
        }
        else
        {
            inFile.ignore( 1024 , '\n' );
        }
    }
    inFile.close();

    return TRUE;
}

BOOL Model::ParseMaterialFile(const string &fileName)
{
    ifstream inFile;
    string fullFileName = filePath + fileName;
    inFile.open(fullFileName);
    if(inFile.fail())
    {
        MessageBox(NULL, L"打开材质文件失败", L"ERROR", MB_OK);
        return FALSE;
    }

    string      mateName;
    string      input;   
    for(inFile>>input; inFile.eof() == FALSE; )
    {	
        if(input == "newmtl")
        {
            Material *temp = new Material;
            inFile>>mateName;
            while(inFile.eof() == FALSE)
            {
                inFile>>input;
                if(input == "Ka")
                {
                    inFile>>temp->ambient.x;
                    inFile>>temp->ambient.y;
                    inFile>>temp->ambient.z;
                    temp->ambient.w = 0;
                }
                else if(input == "Kd")
                {
                    inFile>>temp->diffuse.x;
                    inFile>>temp->diffuse.y;
                    inFile>>temp->diffuse.z;
                    temp->diffuse.w = 0;
                }
                else if(input == "Ks")
                {
                    inFile>>temp->specular.x;
                    inFile>>temp->specular.y;
                    inFile>>temp->specular.z;
                    temp->specular.w = 0;
                }
                else if(input == "illum")
                {
                    int illum;
                    inFile>>illum;
                    //暂时忽略
                }
                else if(input == "Ns")
                {
                    inFile>>temp->power;
                }
                else if(input == "map_Kd")
                {
                    string line, mapFileName;
                    std::getline(inFile, line);
                    std::istringstream iin(line);
                    iin>>mapFileName;
                    if(mapFileName.empty() == FALSE)
                    {
                        temp->texture = SRVBatch::GetInstance()->LoadSRV(filePath, mapFileName);
                        if(temp->texture == NULL)
                        {
                            MessageBox(NULL, L"创建模型纹理视图失败", L"Error", MB_OK);
                            return FALSE;
                        }
                    }
                }
                else if(input == "newmtl")
                {
                    break;
                }
                else
                {
                    inFile.ignore( 1024 , '\n' );
                }
            }
            mateTable.insert(make_pair<string, shared_ptr<Material>>(std::move(mateName), shared_ptr<Material>(temp)));
        }
        else
        {
            inFile.ignore( 1024 , '\n' );
            inFile>>input;
        }
    }
    inFile.close();
    return TRUE;
}

BOOL Model::ParseSubset(const string &fileName)
{
    ifstream inFile;
    inFile.open(fileName);
    if(inFile.fail())
    {
        MessageBox(NULL, L"打开顶点文件失败", L"ERROR", MB_OK);
        return FALSE;
    }

    SubMesh subMesh;
    unsigned long startIndex = 0;
    unsigned long indexCount = 0;

    string input;
    for(inFile>>input; inFile.eof() == FALSE; )
    {	
        if(input == "usemtl")
        {
            subMesh.startIndex = startIndex;
            inFile>>subMesh.mate;

            if(mateTable.find(subMesh.mate) == mateTable.end())
            {
                MessageBox(NULL, L"没有找到材质资源", L"Error", MB_OK);
                return FALSE;
            }

            while(inFile.eof() == FALSE)
            {
                inFile>>input;
                if(input == "f")
                {
                    PosNormalTexTan vertex[3];
                    int posIndex[3];
                    int texIndex[3];
                    int normalIndex[3];

                    char ch;
				
                    inFile >> posIndex[0] >> ch >> texIndex[0] >> ch >> normalIndex[0]
                    >> posIndex[1] >> ch >> texIndex[1] >> ch >> normalIndex[1]
                    >> posIndex[2] >> ch >> texIndex[2] >> ch >> normalIndex[2];

                    for(int i = 0; i != 3; ++i)
                    {
                        vertex[i].pos = poses[posIndex[i] - 1];
                        vertex[i].tex = texturePoses[texIndex[i] - 1];
                        vertex[i].normal = normals[normalIndex[i] - 1];
                        vertices.push_back(vertex[i]);
                        indices.push_back(indexCount);
                        indexCount++;
                    }
                }
                else if(input == "usemtl")
                {
                    break;
                }
            }
            subMesh.indexCount = indexCount - startIndex;
            meshs.push_back(subMesh);
            startIndex = indexCount;
        }
        else
        {
            inFile.ignore( 1024 , '\n' );
            inFile>>input;
        }
    }
    inFile.close();
    poses.clear();
    poses.resize(0);
    normals.clear();
    normals.resize(0);
    texturePoses.clear();
    texturePoses.resize(0);
    return TRUE;
}

BOOL Model::InitBuffers()
{
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(PosNormalTexTan) * vertices.size();
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = &vertices[0];
    if( RenderSystem::GetInstance()->CreateBuffer(bd, initData, verticesBuffer) == FALSE)
    {
        MessageBox(NULL, L"创建顶点缓存失败", L"ERROR", MB_OK);
        return FALSE;
    }

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(DWORD) * indices.size();
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    initData.pSysMem = &indices[0];
    if( RenderSystem::GetInstance()->CreateBuffer(bd, initData, indicesBuffer) == FALSE)
    {
        MessageBox(NULL, L"创建索引缓存失败", L"ERROR", MB_OK);
        return FALSE;
    }

    AssembleShadowMapVertices();
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(XMFLOAT3) * vertices.size();
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    initData.pSysMem = &shadowMapVertices[0];
    if( RenderSystem::GetInstance()->CreateBuffer(bd, initData, shadowMapVerticesBuffer) == FALSE)
    {
        MessageBox(NULL, L"创建阴影贴图顶点缓存失败", L"ERROR", MB_OK);
        Release();
        return FALSE;
    }

    vertCount = indices.size();
    vertices.clear();
    vertices.resize(0);
    indices.clear();
    indices.resize(0);

    return TRUE;
}

void Model::Release()
{
    meshs.clear();
    meshs.resize(0);

    SafeRelease(verticesBuffer);
    SafeRelease(indicesBuffer);
    SafeRelease(shadowMapVerticesBuffer);
    ReleasNewArray(shadowMapVertices);
}

void Model::Update(float time)
{
    
}

const vector<Model::SubMesh>& Model::GetMeshes() const
{
    return meshs;
}

const Model::MateTable& Model::GetMateTable() const
{
    return mateTable;
}

void Model::AssembleShadowMapVertices()
{
    shadowMapVertices = new XMFLOAT3[vertices.size()];
    for(int i = 0; i < vertices.size(); ++i)
    {
        shadowMapVertices[i] = vertices[i].pos;
    }
}