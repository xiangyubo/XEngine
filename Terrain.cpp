#include "Terrain.h"
#include "Texture.h"
#include "Vertice.h"
#include "RenderSystem.h"
#include "Utility.h"

const int TEXTURE_REPEAT = 8;

//Terrain::Terrain()
//{
//    
//}
//
//Terrain::Terrain(const Terrain &tt)
//{
//    
//}
//
//Terrain::~Terrain()
//{
//    Shutdown();
//}
//
//bool Terrain::Init(char* heightMapFilename, WCHAR* textureFilename)
//{
//	bool result;
//
//	// Load in the height map for the terrain.
//	result = LoadHeightMap(heightMapFilename);
//	if(!result)
//	{
//		return false;
//	}
//
//	// Normalize the height of the height map.
//	NormalizeHeightMap();
//
//	// Calculate the normals for the terrain data.
//	result = CalculateNormals();
//	if(!result)
//	{
//		return false;
//	}
//
//	// Calculate the texture coordinates.
//	CalculateTextureCoordinates();
//
//	// Load the texture.
//    result = LoadTexture(RenderSystem::GetInstance()->getDevice(), textureFilename);
//	if(!result)
//	{
//		return false;
//	}
//
//	// Initialize the vertex and index buffer that hold the geometry for the terrain.
//	result = InitializeBuffers(RenderSystem::GetInstance()->getDevice());
//	if(!result)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//void Terrain::Shutdown()
//{
//	ReleaseTexture();
//	ShutdownBuffers();
//	ShutdownHeightMap();
//}
//
//bool Terrain::LoadHeightMap(char* filename)
//{
//	FILE* filePtr;
//	int error;
//	unsigned int count;
//	BITMAPFILEHEADER bitmapFileHeader;
//	BITMAPINFOHEADER bitmapInfoHeader;
//	int imageSize, i, j, k, index;
//	unsigned char* bitmapImage;
//	unsigned char height;
//
//
//	// Open the height map file in binary.
//	error = fopen_s(&filePtr, filename, "rb");
//	if(error != 0)
//	{
//		return false;
//	}
//
//	// Read in the file header.
//	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
//	if(count != 1)
//	{
//		return false;
//	}
//
//	// Read in the bitmap info header.
//	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
//	if(count != 1)
//	{
//		return false;
//	}
//
//	// Save the dimensions of the terrain.
//	terrainWidth = bitmapInfoHeader.biWidth;
//	terrainHeight = bitmapInfoHeader.biHeight;
//
//	// Calculate the size of the bitmap image data.
//	imageSize = terrainWidth * terrainHeight * 3;
//
//	// Allocate memory for the bitmap image data.
//	bitmapImage = new unsigned char[imageSize];
//	if(!bitmapImage)
//	{
//		return false;
//	}
//
//	// Move to the beginning of the bitmap data.
//	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
//
//	// Read in the bitmap image data.
//	count = fread(bitmapImage, 1, imageSize, filePtr);
//	if(count != imageSize)
//	{
//		return false;
//	}
//
//	// Close the file.
//	error = fclose(filePtr);
//	if(error != 0)
//	{
//		return false;
//	}
//
//	// Create the structure to hold the height map data.
//	heightMap = new HeightMapType[terrainWidth * terrainHeight];
//	if(!heightMap)
//	{
//		return false;
//	}
//
//	// Initialize the position in the image data buffer.
//	k=0;
//
//	// Read the image data into the height map.
//	for(j=0; j<terrainHeight; j++)
//	{
//		for(i=0; i<terrainWidth; i++)
//		{
//			height = bitmapImage[k];
//			
//			index = (terrainHeight * j) + i;
//
//			heightMap[index].x = (float)i;
//			heightMap[index].y = (float)height;
//			heightMap[index].z = (float)j;
//
//			k+=3;
//		}
//	}
//
//	// Release the bitmap image data.
//	delete [] bitmapImage;
//	bitmapImage = 0;
//
//	return true;
//}
//
//
//void Terrain::NormalizeHeightMap()
//{
//	int i, j;
//
//
//	for(j=0; j<terrainHeight; j++)
//	{
//		for(i=0; i<terrainWidth; i++)
//		{
//			heightMap[(terrainHeight * j) + i].y /= 15.0f;
//		}
//	}
//
//	return;
//}
//
//
//bool Terrain::CalculateNormals()
//{
//	int i, j, index1, index2, index3, index, count;
//	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
//	VectorType* normals;
//
//
//	// Create a temporary array to hold the un-normalized normal vectors.
//	normals = new VectorType[(terrainHeight-1) * (terrainWidth-1)];
//	if(!normals)
//	{
//		return false;
//	}
//
//	// Go through all the faces in the mesh and calculate their normals.
//	for(j=0; j<(terrainHeight-1); j++)
//	{
//		for(i=0; i<(terrainWidth-1); i++)
//		{
//			index1 = (j * terrainHeight) + i;
//			index2 = (j * terrainHeight) + (i+1);
//			index3 = ((j+1) * terrainHeight) + i;
//
//			// Get three vertic from the face.
//			vertex1[0] = heightMap[index1].x;
//			vertex1[1] = heightMap[index1].y;
//			vertex1[2] = heightMap[index1].z;
//		
//			vertex2[0] = heightMap[index2].x;
//			vertex2[1] = heightMap[index2].y;
//			vertex2[2] = heightMap[index2].z;
//		
//			vertex3[0] = heightMap[index3].x;
//			vertex3[1] = heightMap[index3].y;
//			vertex3[2] = heightMap[index3].z;
//
//			// Calculate the two vectors for this face.
//			vector1[0] = vertex1[0] - vertex3[0];
//			vector1[1] = vertex1[1] - vertex3[1];
//			vector1[2] = vertex1[2] - vertex3[2];
//			vector2[0] = vertex3[0] - vertex2[0];
//			vector2[1] = vertex3[1] - vertex2[1];
//			vector2[2] = vertex3[2] - vertex2[2];
//
//			index = (j * (terrainHeight-1)) + i;
//
//			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
//			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
//			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
//			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
//		}
//	}
//
//	// Now go through all the vertic and take an average of each face normal 	
//	// that the vertex touches to get the averaged normal for that vertex.
//	for(j=0; j<terrainHeight; j++)
//	{
//		for(i=0; i<terrainWidth; i++)
//		{
//			// Initialize the sum.
//			sum[0] = 0.0f;
//			sum[1] = 0.0f;
//			sum[2] = 0.0f;
//
//			// Initialize the count.
//			count = 0;
//
//			// Bottom left face.
//			if(((i-1) >= 0) && ((j-1) >= 0))
//			{
//				index = ((j-1) * (terrainHeight-1)) + (i-1);
//
//				sum[0] += normals[index].x;
//				sum[1] += normals[index].y;
//				sum[2] += normals[index].z;
//				count++;
//			}
//
//			// Bottom right face.
//			if((i < (terrainWidth-1)) && ((j-1) >= 0))
//			{
//				index = ((j-1) * (terrainHeight-1)) + i;
//
//				sum[0] += normals[index].x;
//				sum[1] += normals[index].y;
//				sum[2] += normals[index].z;
//				count++;
//			}
//
//			// Upper left face.
//			if(((i-1) >= 0) && (j < (terrainHeight-1)))
//			{
//				index = (j * (terrainHeight-1)) + (i-1);
//
//				sum[0] += normals[index].x;
//				sum[1] += normals[index].y;
//				sum[2] += normals[index].z;
//				count++;
//			}
//
//			// Upper right face.
//			if((i < (terrainWidth-1)) && (j < (terrainHeight-1)))
//			{
//				index = (j * (terrainHeight-1)) + i;
//
//				sum[0] += normals[index].x;
//				sum[1] += normals[index].y;
//				sum[2] += normals[index].z;
//				count++;
//			}
//			
//			// Take the average of the faces touching this vertex.
//			sum[0] = (sum[0] / (float)count);
//			sum[1] = (sum[1] / (float)count);
//			sum[2] = (sum[2] / (float)count);
//
//			// Calculate the length of this normal.
//			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));
//			
//			// Get an index to the vertex location in the height map array.
//			index = (j * terrainHeight) + i;
//
//			// Normalize the final shared normal for this vertex and store it in the height map array.
//			heightMap[index].nx = (sum[0] / length);
//			heightMap[index].ny = (sum[1] / length);
//			heightMap[index].nz = (sum[2] / length);
//		}
//	}
//
//	// Release the temporary normals.
//	delete [] normals;
//	normals = 0;
//
//	return true;
//}
//
//
//void Terrain::ShutdownHeightMap()
//{
//	if(heightMap)
//	{
//		delete [] heightMap;
//		heightMap = 0;
//	}
//
//	return;
//}
//
//
//void Terrain::CalculateTextureCoordinates()
//{
//	int incrementCount, i, j, tuCount, tvCount;
//	float incrementValue, tuCoordinate, tvCoordinate;
//
//
//	// Calculate how much to increment the texture coordinates by.
//	incrementValue = (float)TEXTURE_REPEAT / (float)terrainWidth;
//
//	// Calculate how many times to repeat the texture.
//	incrementCount = terrainWidth / TEXTURE_REPEAT;
//
//	// Initialize the tu and tv coordinate values.
//	tuCoordinate = 0.0f;
//	tvCoordinate = 1.0f;
//
//	// Initialize the tu and tv coordinate indexes.
//	tuCount = 0;
//	tvCount = 0;
//
//	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
//	for(j=0; j<terrainHeight; j++)
//	{
//		for(i=0; i<terrainWidth; i++)
//		{
//			// Store the texture coordinate in the height map.
//			heightMap[(terrainHeight * j) + i].tu = tuCoordinate;
//			heightMap[(terrainHeight * j) + i].tv = tvCoordinate;
//
//			// Increment the tu texture coordinate by the increment value and increment the index by one.
//			tuCoordinate += incrementValue;
//			tuCount++;
//
//			// Check if at the far right end of the texture and if so then start at the beginning again.
//			if(tuCount == incrementCount)
//			{
//				tuCoordinate = 0.0f;
//				tuCount = 0;
//			}
//		}
//
//		// Increment the tv texture coordinate by the increment value and increment the index by one.
//		tvCoordinate -= incrementValue;
//		tvCount++;
//
//		// Check if at the top of the texture and if so then start at the bottom again.
//		if(tvCount == incrementCount)
//		{
//			tvCoordinate = 1.0f;
//			tvCount = 0;
//		}
//	}
//
//	return;
//}
//
//
//bool Terrain::LoadTexture(ID3D11Device* device, WCHAR* filename)
//{
//	bool result;
//
//
//	// Create the texture object.
//	texture = new Texture;
//	if(!texture)
//	{
//		return false;
//	}
//
//	// Initialize the texture object.
//	result = texture->Initialize(RenderSystem::GetInstance()->getDevice(), filename);
//	if(!result)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//
//void Terrain::ReleaseTexture()
//{
//	// Release the texture object.
//	if(texture)
//	{
//		texture->Shutdown();
//		delete texture;
//		texture = 0;
//	}
//
//	return;
//}
//
//
//bool Terrain::InitializeBuffers(ID3D11Device* device)
//{
//	unsigned long* indices;
//	int index, i, j;
//	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
//    D3D11_SUBRESOURCE_DATA vertexData, indexData;
//	HRESULT result;
//	int index1, index2, index3, index4;
//	float tu, tv;
//
//
//	// Calculate the number of vertic in the terrain mesh.
//    vcount = (terrainWidth - 1) * (terrainHeight - 1) * 6;
//
//	// Set the index count to the same as the vertex count.
//	icount = vcount;
//
//	// Create the vertex array.
//    vertic = new VERTICE::PosNormalTex[vcount];
//	if(!vertic)
//	{
//		return false;
//	}
//
//	// Create the index array.
//	indices = new unsigned long[icount];
//	if(!indices)
//	{
//		return false;
//	}
//
//	// Initialize the index to the vertex buffer.
//	index = 0;
//
//	// Load the vertex and index array with the terrain data.
//	for(j=0; j<(terrainHeight-1); j++)
//	{
//		for(i=0; i<(terrainWidth-1); i++)
//		{
//			index1 = (terrainHeight * j) + i;          // Bottom left.
//			index2 = (terrainHeight * j) + (i+1);      // Bottom right.
//			index3 = (terrainHeight * (j+1)) + i;      // Upper left.
//			index4 = (terrainHeight * (j+1)) + (i+1);  // Upper right.
//
//			// Upper left.
//			tv = heightMap[index3].tv;
//
//			// Modify the texture coordinates to cover the top edge.
//			if(tv == 1.0f) { tv = 0.0f; }
//
//            vertic[index].pos = XMFLOAT3(heightMap[index3].x, heightMap[index3].y, heightMap[index3].z);
//            vertic[index].tex = XMFLOAT2(heightMap[index3].tu, tv);
//			vertic[index].normal = XMFLOAT3(heightMap[index3].nx, heightMap[index3].ny, heightMap[index3].nz);
//			indices[index] = index;
//			index++;
//
//			// Upper right.
//			tu = heightMap[index4].tu;
//			tv = heightMap[index4].tv;
//
//			// Modify the texture coordinates to cover the top and right edge.
//			if(tu == 0.0f) { tu = 1.0f; }
//			if(tv == 1.0f) { tv = 0.0f; }
//
//			vertic[index].pos = XMFLOAT3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
//			vertic[index].tex = XMFLOAT2(tu, tv);
//			vertic[index].normal = XMFLOAT3(heightMap[index4].nx, heightMap[index4].ny, heightMap[index4].nz);
//			indices[index] = index;
//			index++;
//
//			// Bottom left.
//            vertic[index].pos = XMFLOAT3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
//            vertic[index].tex = XMFLOAT2(heightMap[index1].tu, heightMap[index1].tv);
//			vertic[index].normal = XMFLOAT3(heightMap[index1].nx, heightMap[index1].ny, heightMap[index1].nz);
//			indices[index] = index;
//			index++;
//
//			// Bottom left.
//            vertic[index].pos = XMFLOAT3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
//			vertic[index].tex = XMFLOAT2(heightMap[index1].tu, heightMap[index1].tv);
//			vertic[index].normal = XMFLOAT3(heightMap[index1].nx, heightMap[index1].ny, heightMap[index1].nz);
//			indices[index] = index;
//			index++;
//
//			// Upper right.
//			tu = heightMap[index4].tu;
//			tv = heightMap[index4].tv;
//
//			// Modify the texture coordinates to cover the top and right edge.
//			if(tu == 0.0f) { tu = 1.0f; }
//			if(tv == 1.0f) { tv = 0.0f; }
//
//			vertic[index].pos = XMFLOAT3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
//			vertic[index].tex = XMFLOAT2(tu, tv);
//			vertic[index].normal = XMFLOAT3(heightMap[index4].nx, heightMap[index4].ny, heightMap[index4].nz);
//			indices[index] = index;
//			index++;
//
//			// Bottom right.
//			tu = heightMap[index2].tu;
//
//			// Modify the texture coordinates to cover the right edge.
//			if(tu == 0.0f) { tu = 1.0f; }
//
//			vertic[index].pos = XMFLOAT3(heightMap[index2].x, heightMap[index2].y, heightMap[index2].z);
//			vertic[index].tex = XMFLOAT2(tu, heightMap[index2].tv);
//			vertic[index].normal = XMFLOAT3(heightMap[index2].nx, heightMap[index2].ny, heightMap[index2].nz);
//			indices[index] = index;
//			index++;
//		}
//	}
//
//	// Set up the description of the static vertex buffer.
//    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//    vertexBufferDesc.ByteWidth = sizeof(VERTICE::PosNormalTex) * vcount;
//    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//    vertexBufferDesc.CPUAccessFlags = 0;
//    vertexBufferDesc.MiscFlags = 0;
//	vertexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the vertex data.
//    ZeroMemory(&vertexData, sizeof(vertexData));
//    vertexData.pSysMem = vertic;
//	vertexData.SysMemPitch = 0;
//	vertexData.SysMemSlicePitch = 0;
//
//	// Now create the vertex buffer.
//    result = RenderSystem::GetInstance()->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &verticesBuffer);
//	if(FAILED(result))
//	{
//		return false;
//	}
//
//	// Set up the description of the static index buffer.
//    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//    indexBufferDesc.ByteWidth = sizeof(unsigned long) * icount;
//    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//    indexBufferDesc.CPUAccessFlags = 0;
//    indexBufferDesc.MiscFlags = 0;
//	indexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the index data.
//    ZeroMemory(&indexData, sizeof(indexData));
//    indexData.pSysMem = indices;
//	indexData.SysMemPitch = 0;
//	indexData.SysMemSlicePitch = 0;
//
//	// Create the index buffer.
//	result = RenderSystem::GetInstance()->getDevice()->CreateBuffer(&indexBufferDesc, &indexData, &indicesBuffer);
//	if(FAILED(result))
//	{
//		return false;
//	}
//
//	// Release the arrays now that the buffers have been created and loaded.
//	delete [] vertic;
//	vertic = 0;
//
//	delete [] indices;
//	indices = 0;
//
//	return true;
//}
//
//ID3D11ShaderResourceView* Terrain::GetTexture()
//{
//    return texture->GetTexture();
//}
//
//void Terrain::ShutdownBuffers()
//{
//    SafeRelease(indicesBuffer);
//    SafeRelease(verticesBuffer);
//}

Terrain::Terrain()
{
    vertexBuffer = NULL;
    indexBuffer = NULL;
    heightMap = NULL;
    texture = NULL;
}


Terrain::Terrain(const Terrain& other)
{
}


Terrain::~Terrain()
{
}


bool Terrain::Init(const string &hightMapFile, const string &path, const string &fileName)
{
    bool result;

    result = LoadHeightMap(hightMapFile);
    if(!result)
    {
        return false;
    }

    NormalizeHeightMap();

    result = CalculateNormals();
    if(!result)
    {
        return false;
    }

    CalculateTextureCoordinates();

    result = LoadTexture(path, fileName);
    if(!result)
    {
        return false;
    }

    result = InitializeBuffers(RenderSystem::GetInstance()->getDevice());
    if(!result)
    {
        return false;
    }

    return true;
}


void Terrain::Shutdown()
{
    ReleaseTexture();
    ShutdownBuffers();
    ShutdownHeightMap();
}

int Terrain::GetIndexCount()
{
    return indexCount;
}


ID3D11ShaderResourceView* Terrain::GetTexture()
{
    return texture->GetTexture();
}


bool Terrain::LoadHeightMap(const string &filename)
{
    FILE* filePtr;
    int error;
    unsigned int count;
    BITMAPFILEHEADER bitmapFileHeader;
    BITMAPINFOHEADER bitmapInfoHeader;
    int imageSize, i, j, k, index;
    unsigned char* bitmapImage = NULL;
    unsigned char height;


    // Open the height map file in binary.
    error = fopen_s(&filePtr, filename.c_str(), "rb");
    if(error != 0)
    {
        return false;
    }

    // Read in the file header.
    count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
    if(count != 1)
    {
        return false;
    }

    // Read in the bitmap info header.
    count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
    if(count != 1)
    {
        return false;
    }

    // Save the dimensions of the terrain.
    terrainWidth = bitmapInfoHeader.biWidth;
    terrainHeight = bitmapInfoHeader.biHeight;

    // Calculate the size of the bitmap image data.
    imageSize = terrainWidth * terrainHeight * 3;

    // Allocate memory for the bitmap image data.
    bitmapImage = new unsigned char[imageSize];
    if(!bitmapImage)
    {
        return false;
    }

    // Move to the beginning of the bitmap data.
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    // Read in the bitmap image data.
    count = fread(bitmapImage, 1, imageSize, filePtr);
    if(count != imageSize)
    {
        return false;
    }

    // Close the file.
    error = fclose(filePtr);
    if(error != 0)
    {
        return false;
    }

    // Create the structure to hold the height map data.
    heightMap = new HeightMapType[terrainWidth * terrainHeight];
    if(!heightMap)
    {
        return false;
    }

    // Initialize the position in the image data buffer.
    k=0;

    // Read the image data into the height map.
    for(j=0; j<terrainHeight; j++)
    {
        for(i=0; i<terrainWidth; i++)
        {
            height = bitmapImage[k];

            index = (terrainHeight * j) + i;

            heightMap[index].x = (float)i;
            heightMap[index].y = (float)height;
            heightMap[index].z = (float)j;

            k+=3;
        }
    }

    ReleasNewArray(bitmapImage);

    return true;
}


void Terrain::NormalizeHeightMap()
{
    int i, j;


    for(j=0; j<terrainHeight; j++)
    {
        for(i=0; i<terrainWidth; i++)
        {
            heightMap[(terrainHeight * j) + i].y /= 15.0f;
        }
    }

    return;
}


bool Terrain::CalculateNormals()
{
    int i, j, index1, index2, index3, index, count;
    XMFLOAT3    vertex1, vertex2, vertex3;
    XMFLOAT3    vector1, vector2, vector3;
    float       sum[3], length;
    XMFLOAT3    *normals = NULL;

    normals = new XMFLOAT3[(terrainHeight-1) * (terrainWidth-1)];
    if(!normals)
    {
        return false;
    }

    //计算所有网格上四边形的法线向量
    for(j=0; j<(terrainHeight-1); j++)
    {
        for(i=0; i<(terrainWidth-1); i++)
        {
            index1 = (j * terrainHeight) + i;
            index2 = (j * terrainHeight) + (i+1);
            index3 = ((j+1) * terrainHeight) + i;

            //得到四边形上的三个顶点
            vertex1.x = heightMap[index1].x;
            vertex1.y = heightMap[index1].y;
            vertex1.z = heightMap[index1].z;

            vertex2.x = heightMap[index2].x;
            vertex2.y = heightMap[index2].y;
            vertex2.z = heightMap[index2].z;

            vertex3.x = heightMap[index3].x;
            vertex3.y = heightMap[index3].y;
            vertex3.z = heightMap[index3].z;

            //计算四边形内两条向量
            vector1.x = vertex1.x - vertex3.x;
            vector1.y = vertex1.y - vertex3.y;
            vector1.z = vertex1.z - vertex3.z;
            vector2.x = vertex3.x - vertex2.x;
            vector2.y = vertex3.y - vertex2.y;
            vector2.z = vertex3.z - vertex2.z;

            index = (j * (terrainHeight-1)) + i;

            //通过两向量叉乘计算出垂直的法向量
            normals[index].x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
            normals[index].y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
            normals[index].z = (vector1.x * vector2.y) - (vector1.y * vector2.x);
        }
    }

    //对所有顶点遍历相邻四边形的法向量，计算平均值后并归一化
    for(j=0; j<terrainHeight; j++)
    {
        for(i=0; i<terrainWidth; i++)
        {
            sum[0] = 0.0f;
            sum[1] = 0.0f;
            sum[2] = 0.0f;
            count = 0;

            //左下方四边形
            if(((i-1) >= 0) && ((j-1) >= 0))
            {
                index = ((j-1) * (terrainHeight-1)) + (i-1);
                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
                count++;
            }

            //右下方四边形
            if((i < (terrainWidth-1)) && ((j-1) >= 0))
            {
                index = ((j-1) * (terrainHeight-1)) + i;
                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
                count++;
            }

            //左上方四边形
            if(((i-1) >= 0) && (j < (terrainHeight-1)))
            {
                index = (j * (terrainHeight-1)) + (i-1);
                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
                count++;
            }

            //右上方四边形
            if((i < (terrainWidth-1)) && (j < (terrainHeight-1)))
            {
                index = (j * (terrainHeight-1)) + i;
                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
                count++;
            }

            sum[0] = (sum[0] / (float)count);
            sum[1] = (sum[1] / (float)count);
            sum[2] = (sum[2] / (float)count);
            length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));
            index = (j * terrainHeight) + i;

            //归一化平均值后的法向量
            heightMap[index].nx = (sum[0] / length);
            heightMap[index].ny = (sum[1] / length);
            heightMap[index].nz = (sum[2] / length);
        }
    }
    ReleasNewArray(normals);
    return true;
}


void Terrain::ShutdownHeightMap()
{
    ReleasNewArray(heightMap);
}


void Terrain::CalculateTextureCoordinates()
{
    //计算纹理步进长度.
    float incrementValue = (float)TEXTURE_REPEAT / (float)terrainWidth;
    float tuCoordinate = 0.0f, tvCoordinate = 1.0f;

    //计算纹理重复次数
    int incrementCount = terrainWidth / TEXTURE_REPEAT;
    int tuCount = 0, tvCount = 0;

    //遍历所有顶点计算纹理坐标
    for(int j=0; j<terrainHeight; j++)
    {
        for(int i=0; i<terrainWidth; i++)
        {
            heightMap[(terrainHeight * j) + i].tu = tuCoordinate;
            heightMap[(terrainHeight * j) + i].tv = tvCoordinate;

            tuCoordinate += incrementValue;
            tuCount++;

            if(tuCount == incrementCount)
            {
                tuCoordinate = 0.0f;
                tuCount = 0;
            }
        }

        tvCoordinate -= incrementValue;
        tvCount++;

        if(tvCount == incrementCount)
        {
            tvCoordinate = 1.0f;
            tvCount = 0;
        }
    }
}


bool Terrain::LoadTexture(const string &path, const string &filename)
{
    bool result;

    texture = new Texture;
    if(!texture)
    {
        return false;
    }

    result = texture->Initialize(path, filename);
    if(!result)
    {
        return false;
    }

    return true;
}


void Terrain::ReleaseTexture()
{
    if(texture)
    {
        texture->Shutdown();
        delete texture;
        texture = 0;
    }

    return;
}

const XMMATRIX Terrain::GetWorldMatrix() const
{
    XMMATRIX ret = XMMatrixIdentity();
    ret = XMMatrixMultiply(ret, XMMatrixTranslation(-terrainWidth/2, 0, -terrainHeight/3));

    return ret;
}

bool Terrain::InitializeBuffers(ID3D11Device* device)
{
    VertexType* vertices = NULL;
    unsigned long* indices = NULL;
    int index, i, j;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    int index1, index2, index3, index4;
    float tu, tv;


    // Calculate the number of vertices in the terrain mesh.
    vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 6;

    // Set the index count to the same as the vertex count.
    indexCount = vertexCount;

    // Create the vertex array.
    vertices = new VertexType[vertexCount];
    if(!vertices)
    {
        return false;
    }

    // Create the index array.
    indices = new unsigned long[indexCount];
    if(!indices)
    {
        return false;
    }

    // Initialize the index to the vertex buffer.
    index = 0;

    // Load the vertex and index array with the terrain data.
    for(j=0; j<(terrainHeight-1); j++)
    {
        for(i=0; i<(terrainWidth-1); i++)
        {
            index1 = (terrainHeight * j) + i;          // Bottom left.
            index2 = (terrainHeight * j) + (i+1);      // Bottom right.
            index3 = (terrainHeight * (j+1)) + i;      // Upper left.
            index4 = (terrainHeight * (j+1)) + (i+1);  // Upper right.

            // Upper left.
            tv = heightMap[index3].tv;

            // Modify the texture coordinates to cover the top edge.
            if(tv == 1.0f) { tv = 0.0f; }

            vertices[index].position = D3DXVECTOR3(heightMap[index3].x, heightMap[index3].y, heightMap[index3].z);
            vertices[index].texture = D3DXVECTOR2(heightMap[index3].tu, tv);
            vertices[index].normal = D3DXVECTOR3(heightMap[index3].nx, heightMap[index3].ny, heightMap[index3].nz);
            indices[index] = index;
            index++;

            // Upper right.
            tu = heightMap[index4].tu;
            tv = heightMap[index4].tv;

            // Modify the texture coordinates to cover the top and right edge.
            if(tu == 0.0f) { tu = 1.0f; }
            if(tv == 1.0f) { tv = 0.0f; }

            vertices[index].position = D3DXVECTOR3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
            vertices[index].texture = D3DXVECTOR2(tu, tv);
            vertices[index].normal = D3DXVECTOR3(heightMap[index4].nx, heightMap[index4].ny, heightMap[index4].nz);
            indices[index] = index;
            index++;

            // Bottom left.
            vertices[index].position = D3DXVECTOR3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
            vertices[index].texture = D3DXVECTOR2(heightMap[index1].tu, heightMap[index1].tv);
            vertices[index].normal = D3DXVECTOR3(heightMap[index1].nx, heightMap[index1].ny, heightMap[index1].nz);
            indices[index] = index;
            index++;

            // Bottom left.
            vertices[index].position = D3DXVECTOR3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
            vertices[index].texture = D3DXVECTOR2(heightMap[index1].tu, heightMap[index1].tv);
            vertices[index].normal = D3DXVECTOR3(heightMap[index1].nx, heightMap[index1].ny, heightMap[index1].nz);
            indices[index] = index;
            index++;

            // Upper right.
            tu = heightMap[index4].tu;
            tv = heightMap[index4].tv;

            // Modify the texture coordinates to cover the top and right edge.
            if(tu == 0.0f) { tu = 1.0f; }
            if(tv == 1.0f) { tv = 0.0f; }

            vertices[index].position = D3DXVECTOR3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
            vertices[index].texture = D3DXVECTOR2(tu, tv);
            vertices[index].normal = D3DXVECTOR3(heightMap[index4].nx, heightMap[index4].ny, heightMap[index4].nz);
            indices[index] = index;
            index++;

            // Bottom right.
            tu = heightMap[index2].tu;

            // Modify the texture coordinates to cover the right edge.
            if(tu == 0.0f) { tu = 1.0f; }

            vertices[index].position = D3DXVECTOR3(heightMap[index2].x, heightMap[index2].y, heightMap[index2].z);
            vertices[index].texture = D3DXVECTOR2(tu, heightMap[index2].tv);
            vertices[index].normal = D3DXVECTOR3(heightMap[index2].nx, heightMap[index2].ny, heightMap[index2].nz);
            indices[index] = index;
            index++;
        }
    }

    // Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    ReleasNewArray(vertices);
    ReleasNewArray(indices);

    return true;
}


void Terrain::ShutdownBuffers()
{
    SafeRelease(indexBuffer);
    SafeRelease(vertexBuffer);
}

//获取顶点缓冲
ID3D11Buffer*const& Terrain::GetVerticeBuffer() const
{
    return vertexBuffer;
}

//获取索引缓冲
ID3D11Buffer*const& Terrain::GetIndexBuffer() const
{
    return indexBuffer;
}
