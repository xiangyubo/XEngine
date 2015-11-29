#include "Skeleton.h"
#include "SRVBatch.h"
#include "RenderSystem.h"

Skeleton::~Skeleton()
{
    Release();
}

void Skeleton::InQuots(ifstream& infile, string& str)
{
    if(str[str.size()-1] != '"')
    {
        char checkChar;
        bool jointNameFound = FALSE;
        while(!jointNameFound)
        {
            checkChar = infile.get();

            if(checkChar == '"')
            {
                jointNameFound = TRUE;		
            }
            str += checkChar;															
        }
    }
    str.erase(str.size()-1, 1);
    str.erase(0, 1);
}

BOOL Skeleton::LoadSkeleton(const string &filePath, const string &fileName)
{
    string file = filePath + fileName;
    ifstream fileIn(file.c_str());
    string checkString;

    int numJoints;
    int numMeshes;

    if(fileIn.fail())
    {
        MessageBox(NULL, TEXT("打开骨骼文件失败"), TEXT("ERROR"), MB_OK);
        return FALSE;
    }

    while( fileIn.eof() != TRUE )
    {
        checkString.clear();
        fileIn >> checkString;
        if(checkString == "MD5Version")
        {
            int version;
            fileIn >> version;
            if(version != 10)
            {
                MessageBox(0, TEXT("骨骼模型版本错误"), TEXT("Error"), MB_OK);
                return FALSE;
            }
        }
        else if ( checkString == "commandline" )
        {
            std::getline(fileIn, checkString);	// Ignore the rest of this line
        }
        else if ( checkString == "numJoints" )
        {
            fileIn >> numJoints;		// Store number of joints
        }
        else if ( checkString == "numMeshes" )
        {
            fileIn >> numMeshes;		// Store number of meshes or meshs which we will call them
        }
        else if ( checkString == "joints" )
        {
            ParserJoints(fileIn, numJoints);
        }
        else if ( checkString == "mesh")
        {
            ParserMesh(fileIn, filePath);
        }
    }

    if(InitBuffers() != TRUE)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL Skeleton::LoadAnimation(const string &filePath, const string &fileName)
{
    ModelAnimation tempAnim;
    string file = filePath + fileName;
    ifstream fileIn(file.c_str());
    string checkString;

    if(fileIn.fail())
    {
        MessageBox(NULL, TEXT("打开动画文件失败"), TEXT("ERROR"), MB_OK);
        return FALSE;
    }

    while( fileIn.eof() != TRUE )
    {
        checkString.clear();
        fileIn >> checkString;
        if ( checkString == "MD5Version" )		// Get MD5 version (this function supports version 10)
        {
            int version;
            fileIn >> version;
            if(version != 10)
            {
                MessageBox(0, TEXT("动画模型版本错误"), TEXT("Error"), MB_OK);
                return FALSE;
            }
        }
        else if ( checkString == "commandline" )
        {
            std::getline(fileIn, checkString);	// Ignore the rest of this line
        }
        else if ( checkString == "numFrames" )
        {
            fileIn >> tempAnim.numFrames;				// Store number of frames in this animation
        }
        else if ( checkString == "numJoints" )
        {
            fileIn >> tempAnim.numJoints;				// Store number of joints (must match .md5mesh)
        }
        else if ( checkString == "frameRate" )
        {
            fileIn >> tempAnim.frameRate;				// Store animation's frame rate (frames per second)
        }
        else if ( checkString == "numAnimatedComponents" )
        {
            fileIn >> tempAnim.numAnimatedComponents;	// Number of components in each frame section
        }
        else if ( checkString == "hierarchy" )
        {
            BOOL res = ParserHierarchy(fileIn, tempAnim);
            if(res == FALSE)
            {
                MessageBox(0, TEXT("解析动画结构失败"), TEXT("Error"), MB_OK);
                return FALSE;
            }
        }
        else if ( checkString == "bounds" )
        {
            ParserBoundingBox(fileIn, tempAnim);
        }
        else if ( checkString == "baseframe" )
        {
            ParserBaseFrame(fileIn, tempAnim);
        }
        else if ( checkString == "frame" )
        {
            LoadAndBuildFrames(fileIn, tempAnim);
        }
    }
    tempAnim.frameTime = 1.0f / tempAnim.frameRate;						// Set the time per frame
    tempAnim.totalAnimTime = tempAnim.numFrames * tempAnim.frameTime;	// Set the total time the animation takes
    tempAnim.currAnimTime = 0.0f;										// Set the current time to zero
    animations.push_back(tempAnim);
    return TRUE;
}

void Skeleton::ParserJoints(ifstream& fileIn, int numJoints)
{
    Joint tempJoint;
    string checkString;
    fileIn >> checkString;				// Skip the "{"

    for(int i = 0; i < numJoints; i++)
    {
        fileIn >> tempJoint.name;		// Store joints name
        InQuots(fileIn, tempJoint.name);
        fileIn >> tempJoint.parentID;	// Store Parent joint's ID
        fileIn >> checkString;			// Skip the "("
        fileIn >> tempJoint.pos.x >> tempJoint.pos.z >> tempJoint.pos.y;
        fileIn >> checkString >> checkString;	// Skip the ")" and "("
        fileIn >> tempJoint.orientation.x >> tempJoint.orientation.z >> tempJoint.orientation.y;

        float t = 1.0f - ( tempJoint.orientation.x * tempJoint.orientation.x )
            - ( tempJoint.orientation.y * tempJoint.orientation.y )
            - ( tempJoint.orientation.z * tempJoint.orientation.z );
        if ( t < 0.0f )
        {
            tempJoint.orientation.w = 0.0f;
        }
        else
        {
            tempJoint.orientation.w = -sqrtf(t);
        }
        std::getline(fileIn, checkString);		// Skip rest of this line
        joints.push_back(tempJoint);	        // Store the joint into this models joint vector
    }
    fileIn >> checkString;					    // Skip the "}"
}

void Skeleton::ParserMesh(ifstream &fileIn, const string &filePath)
{
    SubMesh subset;
    string  checkString;
    int numVerts, numTris, numWeights;

    fileIn >> checkString;					// Skip the "{"
    fileIn >> checkString;
    while ( checkString != "}" )			// Read until '}'
    {
        if(checkString == "shader")		// Load the texture or material
        {						
            string fileNamePath;
            fileIn >> fileNamePath;			// Get texture's filename
            InQuots(fileIn, fileNamePath);
            subset.texture = SRVBatch::GetInstance()->LoadSRV(filePath, fileNamePath);
            std::getline(fileIn, checkString);				// Skip rest of this line
        }
        else if ( checkString == "numverts")
        {
            fileIn >> numVerts;								// Store number of vertices
            std::getline(fileIn, checkString);				// Skip rest of this line
            for(int i = 0; i < numVerts; i++)
            {
                VertexInfo tempVertInfo;
                PosNormalTexTan tempVert;

                fileIn >> checkString >> checkString >> checkString;	// Skip "vert # ("
                fileIn >> tempVert.tex.x >> tempVert.tex.y;	// Store tex coords
                fileIn >> checkString;						// Skip ")"
                fileIn >> tempVertInfo.StartWeight;			// Index of first weight this vert will be weighted to
                fileIn >> tempVertInfo.WeightCount;         // Number of weights for this vertex
                std::getline(fileIn, checkString);			// Skip rest of this line

                subset.vertInfos.push_back(tempVertInfo);		// Push back this vertex into meshs vertex vector
                subset.vertices.push_back(tempVert);
            }
        }
        else if ( checkString == "numtris")
        {
            fileIn >> numTris;
            std::getline(fileIn, checkString);				// Skip rest of this line
            DWORD tempIndex;

            for(int i = 0; i < numTris; i++)				// Loop through each triangle
            {
                fileIn >> checkString;						// Skip "tri"
                fileIn >> checkString;						// Skip tri counter

                for(int k = 0; k < 3; k++)					// Store the 3 indices
                {
                    fileIn >> tempIndex;
                    subset.indices.push_back(tempIndex);
                }
                std::getline(fileIn, checkString);			// Skip rest of this line
            }
        }
        else if ( checkString == "numweights")
        {
            fileIn >> numWeights;
            std::getline(fileIn, checkString);				// Skip rest of this line
            Weight tempWeight;
            for(int i = 0; i < numWeights; i++)
            {
                fileIn >> checkString >> checkString;		// Skip "weight #"
                fileIn >> tempWeight.jointID;				// Store weight's joint ID
                fileIn >> tempWeight.bias;					// Store weight's influence over a vertex
                fileIn >> checkString;						// Skip "("
                fileIn >> tempWeight.pos.x					// Store weight's pos in joint's local space
                    >> tempWeight.pos.z
                    >> tempWeight.pos.y;
                std::getline(fileIn, checkString);			// Skip rest of this line
                subset.weights.push_back(tempWeight);		// Push back tempWeight into meshs Weight array
            }
        }
        else
        {
            std::getline(fileIn, checkString);
        }
        fileIn >> checkString;
    }
    BuildPosition(subset);
    BuildNormal(subset, numTris);
    meshs.push_back(subset);
}

void Skeleton::BuildPosition(SubMesh &subset)
{
    for ( int i = 0; i < subset.vertices.size(); ++i )
    {
        PosNormalTexTan tempVert;
        tempVert.pos = XMFLOAT3(0, 0, 0);

        for ( int j = 0; j < subset.vertInfos[i].WeightCount; ++j )
        {
            Weight      tempWeight = subset.weights[subset.vertInfos[i].StartWeight + j];
            Joint       tempJoint = joints[tempWeight.jointID];
            XMFLOAT3    rotatedPoint;

            XMVECTOR tempJointOrientation = XMVectorSet(tempJoint.orientation.x, tempJoint.orientation.y, tempJoint.orientation.z, tempJoint.orientation.w);
            XMVECTOR tempWeightPos = XMVectorSet(tempWeight.pos.x, tempWeight.pos.y, tempWeight.pos.z, 0.0f);
            XMVECTOR tempJointOrientationConjugate = XMVectorSet(-tempJoint.orientation.x, -tempJoint.orientation.y, -tempJoint.orientation.z, tempJoint.orientation.w);

            XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightPos), tempJointOrientationConjugate));

            tempVert.pos.x += ( tempJoint.pos.x + rotatedPoint.x ) * tempWeight.bias;
            tempVert.pos.y += ( tempJoint.pos.y + rotatedPoint.y ) * tempWeight.bias;
            tempVert.pos.z += ( tempJoint.pos.z + rotatedPoint.z ) * tempWeight.bias;
        }
        subset.vertices[i].pos = tempVert.pos;
    }
}

void Skeleton::BuildNormal(SubMesh &subset, int numTriangles)
{
    std::vector<XMFLOAT3> tempNormal;
    XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);
    float vecX, vecY, vecZ;

    XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    for(int i = 0; i < numTriangles; ++i)
    {
        vecX = subset.vertices[subset.indices[(i*3)]].pos.x - subset.vertices[subset.indices[(i*3)+2]].pos.x;
        vecY = subset.vertices[subset.indices[(i*3)]].pos.y - subset.vertices[subset.indices[(i*3)+2]].pos.y;
        vecZ = subset.vertices[subset.indices[(i*3)]].pos.z - subset.vertices[subset.indices[(i*3)+2]].pos.z;		
        edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our first edge

        //Get the vector describing another edge of our triangle (edge 2,1)
        vecX = subset.vertices[subset.indices[(i*3)+2]].pos.x - subset.vertices[subset.indices[(i*3)+1]].pos.x;
        vecY = subset.vertices[subset.indices[(i*3)+2]].pos.y - subset.vertices[subset.indices[(i*3)+1]].pos.y;
        vecZ = subset.vertices[subset.indices[(i*3)+2]].pos.z - subset.vertices[subset.indices[(i*3)+1]].pos.z;		
        edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our second edge

        //Cross multiply the two edge vectors to get the un-normalized face normal
        XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));

        tempNormal.push_back(unnormalized);
    }

    //Compute vertex normals (normal Averaging)
    XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    int facesUsing = 0;
    float tX, tY, tZ;	//temp axis variables

    //Go through each vertex
    for(int i = 0; i < subset.vertices.size(); ++i)
    {
        //Check which triangles use this vertex
        for(int j = 0; j < numTriangles; ++j)
        {
            if(subset.indices[j*3] == i ||
                subset.indices[(j*3)+1] == i ||
                subset.indices[(j*3)+2] == i)
            {
                tX = XMVectorGetX(normalSum) + tempNormal[j].x;
                tY = XMVectorGetY(normalSum) + tempNormal[j].y;
                tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

                normalSum = XMVectorSet(tX, tY, tZ, 0.0f);	//If a face is using the vertex, add the unormalized face normal to the normalSum

                facesUsing++;
            }
        }

        normalSum = normalSum / facesUsing;
        normalSum = XMVector3Normalize(normalSum);
        subset.vertices[i].normal.x = -XMVectorGetX(normalSum);
        subset.vertices[i].normal.y = -XMVectorGetY(normalSum);
        subset.vertices[i].normal.z = -XMVectorGetZ(normalSum);

        normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        facesUsing = 0;
    }
}

BOOL Skeleton::InitBuffers()
{
    D3D11_BUFFER_DESC   indexBufferDesc;
    D3D11_BUFFER_DESC   vertexBufferDesc;
    D3D11_BUFFER_DESC   shadowBufferDesc;
    D3D11_SUBRESOURCE_DATA  indexInitData;
    D3D11_SUBRESOURCE_DATA  vertexInitData;
    D3D11_SUBRESOURCE_DATA  shadowInitData;

    AssembleShadowMapVertices();
    for(int i = 0; i < meshs.size(); ++i)
    {
        ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );
        ZeroMemory( &indexInitData, sizeof(indexInitData));
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth = sizeof(DWORD) * meshs[i].indices.size();
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags = 0;
        indexInitData.pSysMem = &meshs[i].indices[0];
        if( RenderSystem::GetInstance()->CreateBuffer(indexBufferDesc, indexInitData, meshs[i].indicesBuffer) == FALSE)
        {
            MessageBox(0, TEXT("骨骼索引初缓冲始化失败"), TEXT("Error"), MB_OK);
            return FALSE;
        }

        ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );
        ZeroMemory( &vertexInitData, sizeof(vertexInitData) );
        vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;		// We will be updating this buffer, so we must set as dynamic
        vertexBufferDesc.ByteWidth = sizeof( PosNormalTexTan ) * meshs[i].vertices.size();
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// Give CPU power to write to buffer
        vertexBufferDesc.MiscFlags = 0;
        vertexInitData.pSysMem = &meshs[i].vertices[0];
        if( RenderSystem::GetInstance()->CreateBuffer( vertexBufferDesc, vertexInitData, meshs[i].verticesBuffer) == FALSE)
        {
            MessageBox(0, TEXT("骨骼顶点缓冲初始化失败"), TEXT("Error"), MB_OK);
            return FALSE;
        }

        ZeroMemory( &shadowBufferDesc, sizeof(shadowBufferDesc));
        ZeroMemory( &shadowInitData, sizeof(shadowInitData));
        shadowBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        shadowBufferDesc.ByteWidth = sizeof(XMFLOAT3) * meshs[i].positions.size();
        shadowBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        shadowBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        shadowBufferDesc.MiscFlags = 0;
        shadowInitData.pSysMem = &meshs[i].positions[0];
        if( RenderSystem::GetInstance()->CreateBuffer(shadowBufferDesc, shadowInitData, meshs[i].shadowMapVerticesBuffer) == FALSE)
        {
            MessageBox(0, TEXT("骨骼阴影缓冲初始化失败"), TEXT("Error"), MB_OK);
            return FALSE;
        }
    }
    return TRUE;
}

void Skeleton::AssembleShadowMapVertices()
{ 
    for(int i = 0; i < meshs.size(); ++i)
    {
        meshs[i].positions.clear();
        for(int j = 0; j < meshs[i].vertices.size(); ++j)
        {
            meshs[i].positions.push_back(meshs[i].vertices[j].pos);
        }
    }
}

void Skeleton::Release()
{
    for(auto it = meshs.begin(); it != meshs.end(); ++it)
    {
        SafeRelease(it->verticesBuffer);
        SafeRelease(it->indicesBuffer);
        SafeRelease(it->shadowMapVerticesBuffer);
    }
}

void Skeleton::Update(float time)
{
    float timeFactor = 0.002f;
    PlayAnimation( time * timeFactor,0);
}

const vector<Skeleton::SubMesh>& Skeleton::GetMeshes() const
{
    return meshs;
}

BOOL Skeleton::ParserHierarchy(ifstream &fileIn, ModelAnimation &tempAnim)
{
    string checkString;
    fileIn >> checkString;          // Get "{"
    for(int i = 0; i < tempAnim.numJoints; i++)	// Load in each joint
    {
        AnimJointInfo tempJoint;

        fileIn >> tempJoint.name;		// Get joints name
        InQuots(fileIn, tempJoint.name);

        fileIn >> tempJoint.parentID;			// Get joints parent ID
        fileIn >> tempJoint.flags;				// Get flags
        fileIn >> tempJoint.startIndex;			// Get joints start index

        // Make sure the joint exists in the model, and the parent ID's match up
        // because the bind pose (md5mesh) joint hierarchy and the animations (md5anim)
        // joint hierarchy must match up
        bool jointMatchFound = FALSE;
        for(int k = 0; k < joints.size(); k++)
        {
            if(joints[k].name == tempJoint.name)
            {
                if(joints[k].parentID == tempJoint.parentID)
                {
                    jointMatchFound = TRUE;
                    tempAnim.jointInfo.push_back(tempJoint);
                }
            }
        }
        if(!jointMatchFound)                    // If the skeleton system does not match up, return FALSE
        {
            return FALSE;						// You might want to add an error message here
        }
        std::getline(fileIn, checkString);		// Skip rest of this line
    }
    return TRUE;
}

void Skeleton::ParserBoundingBox(ifstream &fileIn, ModelAnimation &tempAnim)
{
    string checkString;
    fileIn >> checkString;						// Skip opening bracket "{"

    for(int i = 0; i < tempAnim.numFrames; i++)
    {
        BoundingBox tempBB;

        fileIn >> checkString;					// Skip "("
        fileIn >> tempBB.min.x >> tempBB.min.z >> tempBB.min.y;
        fileIn >> checkString >> checkString;	// Skip ") ("
        fileIn >> tempBB.max.x >> tempBB.max.z >> tempBB.max.y;
        fileIn >> checkString;					// Skip ")"

        tempAnim.frameBounds.push_back(tempBB);
    }
}

void Skeleton::ParserBaseFrame(ifstream &fileIn, ModelAnimation &tempAnim)
{
    string checkString;
    fileIn >> checkString;						// Skip opening bracket "{"

    for(int i = 0; i < tempAnim.numJoints; i++)
    {
        Joint tempBFJ;

        fileIn >> checkString;						// Skip "("
        fileIn >> tempBFJ.pos.x >> tempBFJ.pos.z >> tempBFJ.pos.y;
        fileIn >> checkString >> checkString;		// Skip ") ("
        fileIn >> tempBFJ.orientation.x >> tempBFJ.orientation.z >> tempBFJ.orientation.y;
        fileIn >> checkString;						// Skip ")"

        tempAnim.baseFrameJoints.push_back(tempBFJ);
    }
}

void Skeleton::LoadAndBuildFrames(ifstream &fileIn, ModelAnimation &tempAnim)
{
    string checkString;
    FrameData tempFrame;

    fileIn >> tempFrame.frameID;		// Get the frame ID
    fileIn >> checkString;				// Skip opening bracket "{"
    for(int i = 0; i < tempAnim.numAnimatedComponents; i++)
    {
        float tempData;
        fileIn >> tempData;				// Get the data

        tempFrame.frameData.push_back(tempData);
    }
    tempAnim.frameData.push_back(tempFrame);

    std::vector<Joint> tempSkeleton;
    for(int i = 0; i < tempAnim.jointInfo.size(); i++)
    {
        int k = 0;						// Keep track of position in frameData array

        // Start the frames joint with the base frame's joint
        Joint tempFrameJoint = tempAnim.baseFrameJoints[i];

        tempFrameJoint.parentID = tempAnim.jointInfo[i].parentID;

        // Notice how I have been flipping y and z. this is because some modeling programs such as
        // 3ds max (which is what I use) use a right handed coordinate system. Because of this, we
        // need to flip the y and z axes. If your having problems loading some models, it's possible
        // the model was created in a left hand coordinate system. in that case, just reflip all the
        // y and z axes in our md5 mesh and anim loader.
        if(tempAnim.jointInfo[i].flags & 1)		// pos.x	( 000001 )
            tempFrameJoint.pos.x = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

        if(tempAnim.jointInfo[i].flags & 2)		// pos.y	( 000010 )
            tempFrameJoint.pos.z = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

        if(tempAnim.jointInfo[i].flags & 4)		// pos.z	( 000100 )
            tempFrameJoint.pos.y = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

        if(tempAnim.jointInfo[i].flags & 8)		// orientation.x	( 001000 )
            tempFrameJoint.orientation.x = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

        if(tempAnim.jointInfo[i].flags & 16)	// orientation.y	( 010000 )
            tempFrameJoint.orientation.z = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

        if(tempAnim.jointInfo[i].flags & 32)	// orientation.z	( 100000 )
            tempFrameJoint.orientation.y = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

        // Compute the quaternions w
        float t = 1.0f - ( tempFrameJoint.orientation.x * tempFrameJoint.orientation.x )
            - ( tempFrameJoint.orientation.y * tempFrameJoint.orientation.y )
            - ( tempFrameJoint.orientation.z * tempFrameJoint.orientation.z );
        if ( t < 0.0f )
        {
            tempFrameJoint.orientation.w = 0.0f;
        }
        else
        {
            tempFrameJoint.orientation.w = -sqrtf(t);
        }

        // Now, if the upper arm of your skeleton moves, you need to also move the lower part of your arm, and then the hands, and then finally the fingers (possibly weapon or tool too)
        // This is where joint hierarchy comes in. We start at the top of the hierarchy, and move down to each joints child, rotating and translating them based on their parents rotation
        // and translation. We can assume that by the time we get to the child, the parent has already been rotated and transformed based of it's parent. We can assume this because
        // the child should never come before the parent in the files we loaded in.
        if(tempFrameJoint.parentID >= 0)
        {
            Joint parentJoint = tempSkeleton[tempFrameJoint.parentID];

            // Turn the XMFLOAT3 and 4's into vectors for easier computation
            XMVECTOR parentJointOrientation = XMVectorSet(parentJoint.orientation.x, parentJoint.orientation.y, parentJoint.orientation.z, parentJoint.orientation.w);
            XMVECTOR tempJointPos = XMVectorSet(tempFrameJoint.pos.x, tempFrameJoint.pos.y, tempFrameJoint.pos.z, 0.0f);
            XMVECTOR parentOrientationConjugate = XMVectorSet(-parentJoint.orientation.x, -parentJoint.orientation.y, -parentJoint.orientation.z, parentJoint.orientation.w);

            // Calculate current joints position relative to its parents position
            XMFLOAT3 rotatedPos;
            XMStoreFloat3(&rotatedPos, XMQuaternionMultiply(XMQuaternionMultiply(parentJointOrientation, tempJointPos), parentOrientationConjugate));

            // Translate the joint to model space by adding the parent joint's pos to it
            tempFrameJoint.pos.x = rotatedPos.x + parentJoint.pos.x;
            tempFrameJoint.pos.y = rotatedPos.y + parentJoint.pos.y;
            tempFrameJoint.pos.z = rotatedPos.z + parentJoint.pos.z;

            // Currently the joint is oriented in its parent joints space, we now need to orient it in
            // model space by multiplying the two orientations together (parentOrientation * childOrientation) <- In that order
            XMVECTOR tempJointOrient = XMVectorSet(tempFrameJoint.orientation.x, tempFrameJoint.orientation.y, tempFrameJoint.orientation.z, tempFrameJoint.orientation.w);
            tempJointOrient = XMQuaternionMultiply(parentJointOrientation, tempJointOrient);

            // Normalize the orienation quaternion
            tempJointOrient = XMQuaternionNormalize(tempJointOrient);
            XMStoreFloat4(&tempFrameJoint.orientation, tempJointOrient);
        }
        // Store the joint into our temporary frame skeleton
        tempSkeleton.push_back(tempFrameJoint);
    }
    // Push back our newly created frame skeleton into the animation's frameSkeleton array
    tempAnim.frameSkeleton.push_back(tempSkeleton);
    fileIn >> checkString;				// Skip closing bracket "}"
}

void Skeleton::PlayAnimation(float deltaTime, int animation)
{
    animations[animation].currAnimTime += deltaTime;
    if(animations[animation].currAnimTime > animations[animation].totalAnimTime)
        animations[animation].currAnimTime = 0.0f;

    //计算当前帧
    float currentFrame = animations[animation].currAnimTime * animations[animation].frameRate;	
    int frame0 = floorf( currentFrame );
    int frame1 = frame0 + 1;

    //确保不超过最大帧	
    if(frame0 == animations[animation].numFrames-1)
        frame1 = 0;

    float interpolation = currentFrame - frame0;	//依时间计算插值因子

    std::vector<Joint> interpolatedSkeleton;		//创建插值骨骼，然后计算插值骨骼
    for( int i = 0; i < animations[animation].numJoints; i++)
    {
        Joint tempJoint;
        Joint joint0 = animations[animation].frameSkeleton[frame0][i];		// Get the i'th joint of frame0's skeleton
        Joint joint1 = animations[animation].frameSkeleton[frame1][i];		// Get the i'th joint of frame1's skeleton

        tempJoint.parentID = joint0.parentID;											// Set the tempJoints parent id

        // Turn the two quaternions into XMVECTORs for easy computations
        XMVECTOR joint0Orient = XMVectorSet(joint0.orientation.x, joint0.orientation.y, joint0.orientation.z, joint0.orientation.w);
        XMVECTOR joint1Orient = XMVectorSet(joint1.orientation.x, joint1.orientation.y, joint1.orientation.z, joint1.orientation.w);

        // Interpolate positions
        tempJoint.pos.x = joint0.pos.x + (interpolation * (joint1.pos.x - joint0.pos.x));
        tempJoint.pos.y = joint0.pos.y + (interpolation * (joint1.pos.y - joint0.pos.y));
        tempJoint.pos.z = joint0.pos.z + (interpolation * (joint1.pos.z - joint0.pos.z));

        // Interpolate orientations using spherical interpolation (Slerp)
        XMStoreFloat4(&tempJoint.orientation, XMQuaternionSlerp(joint0Orient, joint1Orient, interpolation));

        interpolatedSkeleton.push_back(tempJoint);		// Push the joint back into our interpolated skeleton
    }
    for ( int k = 0; k < meshs.size(); k++)
    {
        for ( int i = 0; i < meshs[k].vertices.size(); ++i )
        {
            PosNormalTexTan tempVert = meshs[k].vertices[i];
            tempVert.pos = XMFLOAT3(0, 0, 0);	// Make sure the vertex's pos is cleared first
            tempVert.normal = XMFLOAT3(0,0,0);	// Clear vertices normal

            // Sum up the joints and weights information to get vertex's position and normal
            for ( int j = 0; j < meshs[k].vertInfos[i].WeightCount; ++j )
            {
                Weight tempWeight = meshs[k].weights[meshs[k].vertInfos[i].StartWeight + j];
                Joint tempJoint = interpolatedSkeleton[tempWeight.jointID];

                // Convert joint orientation and weight pos to vectors for easier computation
                XMVECTOR tempJointOrientation = XMVectorSet(tempJoint.orientation.x, tempJoint.orientation.y, tempJoint.orientation.z, tempJoint.orientation.w);
                XMVECTOR tempWeightPos = XMVectorSet(tempWeight.pos.x, tempWeight.pos.y, tempWeight.pos.z, 0.0f);

                // We will need to use the conjugate of the joint orientation quaternion
                XMVECTOR tempJointOrientationConjugate = XMQuaternionInverse(tempJointOrientation);

                // Calculate vertex position (in joint space, eg. rotate the point around (0,0,0)) for this weight using the joint orientation quaternion and its conjugate
                // We can rotate a point using a quaternion with the equation "rotatedPoint = quaternion * point * quaternionConjugate"
                XMFLOAT3 rotatedPoint;
                XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightPos), tempJointOrientationConjugate));

                // Now move the verices position from joint space (0,0,0) to the joints position in world space, taking the weights bias into account
                tempVert.pos.x += ( tempJoint.pos.x + rotatedPoint.x ) * tempWeight.bias;
                tempVert.pos.y += ( tempJoint.pos.y + rotatedPoint.y ) * tempWeight.bias;
                tempVert.pos.z += ( tempJoint.pos.z + rotatedPoint.z ) * tempWeight.bias;

                // Compute the normals for this frames skeleton using the weight normals from before
                // We can comput the normals the same way we compute the vertices position, only we don't have to translate them (just rotate)
                XMVECTOR tempWeightNormal = XMVectorSet(tempWeight.normal.x, tempWeight.normal.y, tempWeight.normal.z, 0.0f);

                // Rotate the normal
                XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightNormal), tempJointOrientationConjugate));

                // Add to vertices normal and ake weight bias into account
                tempVert.normal.x -= rotatedPoint.x * tempWeight.bias;
                tempVert.normal.y -= rotatedPoint.y * tempWeight.bias;
                tempVert.normal.z -= rotatedPoint.z * tempWeight.bias;
            }

            meshs[k].positions[i] = tempVert.pos;				// Store the vertices position in the position vector instead of straight into the vertex vector
            meshs[k].vertices[i].normal = tempVert.normal;		// Store the vertices normal
            XMStoreFloat3(&meshs[k].vertices[i].normal, XMVector3Normalize(XMLoadFloat3(&meshs[k].vertices[i].normal)));
        }
        // Put the positions into the vertices for this subset
        for(int i = 0; i < meshs[k].vertices.size(); i++)
        {
            meshs[k].vertices[i].pos = meshs[k].positions[i];
        }

        // Update the meshs vertex buffer
        // First lock the buffer
        D3D11_MAPPED_SUBRESOURCE mappedVertBuff;
        RenderSystem::GetInstance()->GetDeviceContext()->Map(meshs[k].verticesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertBuff);
        memcpy(mappedVertBuff.pData, &meshs[k].vertices[0], (sizeof(PosNormalTexTan) * meshs[k].vertices.size()));
        RenderSystem::GetInstance()->GetDeviceContext()->Unmap(meshs[k].verticesBuffer, 0);

        RenderSystem::GetInstance()->GetDeviceContext()->Map(meshs[k].shadowMapVerticesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertBuff);
        memcpy(mappedVertBuff.pData, &meshs[k].positions[0], (sizeof(XMFLOAT3) * meshs[k].positions.size()));
        RenderSystem::GetInstance()->GetDeviceContext()->Unmap(meshs[k].shadowMapVerticesBuffer, 0);

        // The line below is another way to update a buffer. You will use this when you want to update a buffer less
        // than once per frame, since the GPU reads will be faster (the buffer was created as a DEFAULT buffer instead
        // of a DYNAMIC buffer), and the CPU writes will be slower. You can try both methods to find out which one is faster
        // for you. if you want to use the line below, you will have to create the buffer with D3D11_USAGE_DEFAULT instead
        // of D3D11_USAGE_DYNAMIC
        //d3d11DevCon->UpdateSubresource( meshs[k].vertBuff, 0, NULL, &meshs[k].vertices[0], 0, 0 );
    }
}


