cbuffer ConstantBuffer
{
    matrix  World;
	matrix  View;
	matrix  Projection;
    float4  EyePosition;
}

cbuffer MaterialBuffer
{
    float4  MatAmbient;
    float4  MatDiffuse;
    float4  MatSpecular;
    float   MatPower;
}

struct VS_INPUT
{
    float4  Pos : POSITION;
    float4  Norm : NORMAL;
    float4  Tan : TANGENT;
    float2  Tex : TEXCOORD;
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};


VS_OUT VShader(VS_INPUT input)
{
    VS_OUT  output = (VS_OUT)0;
    output.Pos = input.Pos;
    output.Color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    return output;
}


float4 PShader(VS_OUT input) : SV_TARGET
{
    return input.Color;
}

technique11 T_Test
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VShader() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PShader() ) );
    }
}