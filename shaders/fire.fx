//////////////////////////////////////////////////////////////////////////
// 每帧更新的变量
//////////////////////////////////////////////////////////////////////////
cbuffer cbPerFrame
{
	// 眼睛所在位置
	float4 gEyePosW;
	
	// 粒子系统的位置
	float4 gEmitPosW;

	// 粒子发射的方向
	float4 gEmitDirW;
	
	// 游戏时间
	float gGameTime;
	// 时间步长
	float gTimeStep;

	float4x4 gViewProj; 
};

//////////////////////////////////////////////////////////////////////////
// 固定的常量
//////////////////////////////////////////////////////////////////////////
cbuffer cbFixed
{
	// 粒子的加速度
	float3 gAccelW = {0.0f, 7.8f, 0.0f};
	
	// Texture coordinates used to stretch texture over quad 
	// when we expand point particle into a quad.
	float2 gQuadTexC[4] = 
	{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};
};
 
// 用来渲染粒子的纹理
Texture2DArray gTexArray;

// 用来在着色器中产生随机数的随机纹理
Texture1D gRandomTex;
 
SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

DepthStencilState NoDepthWrites
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
};

BlendState AdditiveBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

//////////////////////////////////////////////////////////////////////////
// 用来从随机纹理中得到随机的方向向量
//////////////////////////////////////////////////////////////////////////
float3 RandUnitVec3(float offset)
{
	// 游戏时间加上偏移量来从随机纹理中得到随机数
	float u = (gGameTime + offset);
	
	// 获得范围为[-1,1]的向量
	float3 v = gRandomTex.SampleLevel(gTriLinearSam, u, 0);
	
	// 将得到的向量转换为单位向量
	return normalize(v);
}
 
//////////////////////////////////////////////////////////////////////////
// 流输出Tech
//////////////////////////////////////////////////////////////////////////
#define PT_EMITTER 0
#define PT_FLARE 1
 
//////////////////////////////////////////////////////////////////////////
// 粒子的结构
//////////////////////////////////////////////////////////////////////////
struct Particle
{
	float3 initialPosW : POSITION;
	float3 initialVelW : VELOCITY;
	float2 sizeW       : SIZE;
	float age          : AGE;
	uint type          : TYPE;
};

//////////////////////////////////////////////////////////////////////////
// 顶点着色器不做处理
//////////////////////////////////////////////////////////////////////////
Particle StreamOutVS(Particle vIn)
{
	return vIn;
}

// The stream-out GS is just responsible for emitting 
// new particles and destroying old particles.  The logic
// programed here will generally vary from particle system
// to particle system, as the destroy/spawn rules will be 
// different.

//////////////////////////////////////////////////////////////////////////
// 几何着色器中进行粒子的产生和销毁
// 要实现不同的粒子系统的效果，需要修改这里面的逻辑
//////////////////////////////////////////////////////////////////////////
[maxvertexcount(2)]
void StreamOutGS(point Particle gIn[1], 
                 inout PointStream<Particle> ptStream)
{	
	// 老化粒子
	gIn[0].age += gTimeStep;
	
	if( gIn[0].type == PT_EMITTER )
	{	
		// 每0.005s创建一个新的粒子
		// 即每秒创建200个粒子
		if( gIn[0].age > 0.005f )
		{
			//产生随机数
			float3 vRandom = RandUnitVec3(0.0f);
			vRandom.x *= 0.5f;
			vRandom.z *= 0.5f;
			
			//创建新的粒子
			Particle p;
			p.initialPosW = gEmitPosW.xyz;
			p.initialVelW = 4.0f*vRandom;
			p.sizeW       = float2(1.0f, 1.0f);
			p.age         = 0.0f;
			p.type        = PT_FLARE;
			
			ptStream.Append(p);
			
			// reset the time to emit
			gIn[0].age = 0.0f;
		}
		
		// always keep emitters
		ptStream.Append(gIn[0]);
	}
	else
	{
		// 销毁粒子存活时间大于1秒的
		if( gIn[0].age <= 1.0f )
			ptStream.Append(gIn[0]);
	}		
}

//流输出
GeometryShader gsStreamOut = ConstructGSWithSO( 
	CompileShader( gs_5_0, StreamOutGS() ), 
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x" );
	
technique11 StreamOutTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, StreamOutVS() ) );
        SetGeometryShader( gsStreamOut );
        
        // disable pixel shader for stream-out only
        SetPixelShader(NULL);
        
        // we must also disable the depth buffer for stream-out only
        SetDepthStencilState( DisableDepth, 0 );
    }
}

//////////////////////////////////////////////////////////////////////////
// 绘制Tech
//////////////////////////////////////////////////////////////////////////

struct VS_OUT
{
	float3 posW  : POSITION;
	float2 sizeW : SIZE;
	float4 color : COLOR;
	uint   type  : TYPE;
};

VS_OUT DrawVS(Particle vIn)
{
	VS_OUT vOut;
	
	float t = vIn.age;
	
	// constant acceleration equation
	vOut.posW = 0.5f*t*t*gAccelW + t*vIn.initialVelW + vIn.initialPosW;
	
	// fade color with time
	float opacity = 1.0f - smoothstep(0.0f, 1.0f, t/1.0f);
	vOut.color = float4(1.0f, 1.0f, 1.0f, opacity);
	
	vOut.sizeW = vIn.sizeW;
	vOut.type  = vIn.type;
	
	return vOut;
}

struct GS_OUT
{
	float4 posH  : SV_Position;
	float4 color : COLOR;
	float2 texC  : TEXCOORD;
};

// 在这个集合着色器中将点扩展为始终朝向摄像机的四边形
[maxvertexcount(4)]
void DrawGS(point VS_OUT gIn[1], 
            inout TriangleStream<GS_OUT> triStream)
{	
	// do not draw emitter particles.
	if( gIn[0].type != PT_EMITTER )
	{
		//
		// Compute world matrix so that billboard faces the camera.
		//
		float3 look  = normalize(gEyePosW.xyz - gIn[0].posW);
		float3 right = normalize(cross(float3(0,1,0), look));
		float3 up    = cross(look, right);
		
		float4x4 W;
		W[0] = float4(right,       0.0f);
		W[1] = float4(up,          0.0f);
		W[2] = float4(look,        0.0f);
		W[3] = float4(gIn[0].posW, 1.0f);

		float4x4 WVP = mul(W, gViewProj);
		
		//
		// Compute 4 triangle strip vertices (quad) in local space.
		// The quad faces down the +z axis in local space.
		//
		float halfWidth  = 0.5f*gIn[0].sizeW.x;
		float halfHeight = 0.5f*gIn[0].sizeW.y;
	
		float4 v[4];
		v[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f);
		v[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);
		v[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);
		v[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);
		
		//
		// Transform quad vertices to world space and output 
		// them as a triangle strip.
		//
		GS_OUT gOut;
		[unroll]
		for(int i = 0; i < 4; ++i)
		{
			gOut.posH  = mul(v[i], WVP);
			gOut.texC  = gQuadTexC[i];
			gOut.color = gIn[0].color;
			triStream.Append(gOut);
		}	
	}
}

float4 DrawPS(GS_OUT pIn) : SV_TARGET
{
	return gTexArray.Sample(gTriLinearSam, float3(pIn.texC, 0))*pIn.color;
}

technique11 DrawTech
{
    pass P0
    {
        SetVertexShader(   CompileShader( vs_5_0, DrawVS() ) );
        SetGeometryShader( CompileShader( gs_5_0, DrawGS() ) );
        SetPixelShader(    CompileShader( ps_5_0, DrawPS() ) );
        
        SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetDepthStencilState( NoDepthWrites, 0 );
    }
}