cbuffer ConstantBuffer
{
    matrix  World;
	matrix  ShadowMapView;
	matrix  ShadowMapProjection;
    float4x4 gWorldViewProj;
}

struct VS_INPUT
{
    float3  Pos : POSITION;
};

struct PS_INPUT
{
    float4  Pos : SV_POSITION;
};

float c = 80.0f;

PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;

    output.Pos = mul(float4(input.Pos, 1.0f), World);
    output.Pos = mul(output.Pos, ShadowMapView);
    output.Pos = mul(output.Pos, ShadowMapProjection);

    return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
    return float4(input.Pos.z, input.Pos.z*input.Pos.z, exp(c * input.Pos.z), 1);
}

RasterizerState Depth
{
	DepthBias = 100000;
    DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};

technique11 ShadowMapRender
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        //SetPixelShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
        SetRasterizerState(Depth);
    }
}

