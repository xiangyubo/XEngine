//只是为了初始化各种节点，不能实际用作渲染

struct Pos
{
	float3 Pos : POSITION;
};

struct PosNormalTex
{
	float3 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD;
};

struct PosNormalTexTan
{
	float3 Pos : POSITION;
	float3 Norm : NORMAL;
	float3 Tan : TANGENT;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
};

struct Particle
{
	float3 Pos : POSITION;
	float3 initialVelW : VELOCITY;
	float2 sizeW       : SIZE;
	float age          : AGE;
	uint type          : TYPE;
};

VertexOut VS_Pos(Pos input)
{
	VertexOut output;
	output.Pos = float4(input.Pos, 1.0f);
	
	return output;
}

VertexOut VS_PosNormalTex(PosNormalTex input)
{
	VertexOut output;
	output.Pos = float4(input.Pos, 1.0f);
	
	return output;
}

VertexOut VS_PosNormalTexTan(PosNormalTexTan input)
{
	VertexOut output;
	output.Pos = float4(input.Pos, 1.0f);
	
	return output;
}

VertexOut VS_Partical(Particle input)
{
	VertexOut output;
	output.Pos = float4(input.Pos, 1.0f);
	
	return output;
}

void PS(VertexOut pin)
{

}

technique11 PosTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS_Pos() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}


technique11 PosNormalTexTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS_PosNormalTex() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}


technique11 PosNormalTexTanTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS_PosNormalTexTan() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}

technique11 ParticalTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS_Partical() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}