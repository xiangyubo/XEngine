#include "VisiableTest.fx"

cbuffer MatrixBuffer
{
	matrix  World;
	matrix  View;
	matrix  Projection;
    matrix  ShadowMapView;
    matrix  ShadowMapProjection;
    bool    shadowFlag;
};

struct VertexInputType
{
    float4 position : POSITION;
	float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4  position : SV_POSITION;
	float3  normal : NORMAL;
    float2  tex : TEXCOORD0;
    float4  FromLightPos : TEXCOORD1;
};

Texture2D   shadowMap;

PixelInputType TerrainVertexShader(VertexInputType input)
{
    PixelInputType output;
    input.position.w = 1.0f;

    output.position = mul(input.position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    
    output.FromLightPos = mul(input.position, World);    
    output.FromLightPos = mul(output.FromLightPos, ShadowMapView);
    output.FromLightPos = mul(output.FromLightPos, ShadowMapProjection);
	
    output.normal = mul(input.normal, (float3x3)World);
    output.normal = normalize(output.normal);

    output.tex = input.tex;
    return output;
}

Texture2D Texture;
SamplerState SampleType;

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
    float3 lightDirection;
	float padding;
};

float4 TerrainPixelShader(PixelInputType input) : SV_TARGET
{
    float softFactor = 1.0f;
    if(shadowFlag == true)
    {
        softFactor = IsInLightSSM(input.FromLightPos, shadowMap);
    }
    
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;


    textureColor = Texture.Sample(SampleType, input.tex);

    color = ambientColor;

    lightDir = -lightDirection;

    lightIntensity = saturate(dot(input.normal, lightDir));

	if(lightIntensity > 0.0f)
    {
        color += (diffuseColor * lightIntensity);
    }

    color = saturate(color);

    color = color * textureColor;

    return color * softFactor;
}

technique11 TerrainTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, TerrainVertexShader() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, TerrainPixelShader() ) );
    }
}