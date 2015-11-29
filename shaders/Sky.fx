cbuffer cbPerFrame
{
	float4x4 WorldViewProj;
};
 
TextureCube skyMap;
SamplerState Sampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexIn
{
	float3 Pos : POSITION;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
    float3 TexC : POSITION;
};
 
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.Pos = mul(float4(vin.Pos, 1.0f), WorldViewProj).xyww;
	
	vout.TexC = vin.Pos;
	
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return skyMap.Sample(Sampler, pin.TexC);
}

RasterizerState NoCull
{
    CullMode = None;
};

DepthStencilState LessEqualDSS
{
    DepthFunc = LESS_EQUAL;
};

technique11 SkyTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
        
        SetRasterizerState(NoCull);
        SetDepthStencilState(LessEqualDSS, 0);
    }
}
