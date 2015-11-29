#include "VisiableTest.fx"

cbuffer ConstantBuffer
{
    matrix  World;
	matrix  View;
	matrix  Projection;
    matrix  ShadowMapView;
	matrix  ShadowMapProjection;
    float4x4 gShadowTransform; 
    float4  EyePosition;
    bool    shadowFlag;
}

cbuffer MaterialBuffer
{
    float4  MatAmbient;
    float4  MatDiffuse;
    float4  MatSpecular;
    float   MatPower;
    bool    textureOn;
}
Texture2D   Texture;
SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
};

SamplerComparisonState samShadow
{
	Filter   = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS;
};

Texture2D   shadowMap;

cbuffer LightBuffer
{
    int     type;
    float4  LightPos;
    float4  LightDir;
    float4  LightAmb;
    float4  LightDiff;
    float4  LightSpe;
    float   LightAtt0;
    float   LightAtt1;
    float   LightAtt2;
    float   LightAlpha;
    float   LightBeta;
    float   LightFallOff;
}

struct VS_INPUT
{
    float3  Pos : POSITION;
    float3  Norm : NORMAL;
    float3  Tan : TANGENT;
    float2  Tex : TEXCOORD;
};

struct PS_INPUT
{
    float4  Pos : SV_POSITION;
    float4  Norm : NORMAL;
    float4  ViewDirection : TEXCOORD1;
    float4  LightVector : TEXCOORD2;    //对点光源和聚光灯有效，前3各分量记录“光照向量”，最后一个分量记录光照距离
    float4  FromLightPos : TEXCOORD3;    //在光源视角下的坐标
    float2  Tex : TEXCOORD;
};

//顶点着色器
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;

    output.Pos = mul(float4(input.Pos, 1.0f), World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    output.FromLightPos = mul(float4(input.Pos, 1.0f), World);    
    output.FromLightPos = mul(output.FromLightPos, ShadowMapView);
    output.FromLightPos = mul(output.FromLightPos, ShadowMapProjection);
    
    output.Norm = mul(float4(input.Norm, 0.0f), World);
    output.Norm = normalize(output.Norm);

    float4 worldPosition = mul(float4(input.Pos, 1.0f), World);   //一定要非常慎重世界坐标！
    output.ViewDirection = EyePosition - worldPosition; //一切的计算都是世界坐标里面！
    output.ViewDirection = normalize(output.ViewDirection); //所以千万不能用视图投影之后的坐标！

    output.LightVector = LightPos - worldPosition;
    output.LightVector = normalize(output.LightVector);
    output.LightVector.w = length(LightPos - worldPosition);
    
    output.Tex = input.Tex;

    return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
    float2 tex;
    tex.x = 0.5;
    tex.y = 0.5;
    //float4 col = Texture.Sample(Sampler, tex);
    float4 col = Texture.Sample(Sampler, input.Tex);
    return col;
}

//平行光像素着色器
float4 PSDirectionalLight(PS_INPUT input) : SV_Target
{
    float softFactor = 1.0f;
    if(shadowFlag == true)
    {
        softFactor = IsInLightESM(input.FromLightPos, shadowMap);
    }

    float4 textureColor;
    float4 finalColor;
	float4 ambientColor, diffuseColor, specularColor;
	
	//光照方向,和光线照射方向相反
	float4 lightVector = -LightDir;

	ambientColor = MatAmbient * LightAmb;

	float diffuseFactor = dot(lightVector, input.Norm);
	if(diffuseFactor > 0.0f)
	{
		diffuseColor = MatDiffuse * LightDiff * diffuseFactor;

		float4 reflection = reflect(-lightVector, input.Norm);
		float specularFactor = pow(max(dot(reflection, input.ViewDirection), 0.0f), MatPower);

		specularColor = MatSpecular * LightSpe * specularFactor;
	}

	finalColor = ambientColor + diffuseColor;
    if(textureOn == true)
    {
        textureColor = Texture.Sample(Sampler, input.Tex);
        return textureColor;
        finalColor = finalColor * textureColor;
        return textureColor;
    }
    else
    {
        finalColor = saturate(finalColor + specularColor);
        return finalColor * softFactor;
    }
}

//点光源着色器
float4 PSPointLight(PS_INPUT input) : SV_Target
{
    float softFactor = 1.0f;
    if(shadowFlag == true)
    {
        softFactor = IsInLightSSM(input.FromLightPos, shadowMap);
    }

    float4 textureColor;
    float4 finalColor;
	float4 ambientColor, diffuseColor, specularColor;

	float4 lightVector = input.LightVector;

	ambientColor = MatAmbient * LightAmb;

	float diffuseFactor = dot(lightVector, input.Norm);
	if(diffuseFactor > 0.0f)
	{
		diffuseColor = MatDiffuse * LightDiff * diffuseFactor;

		float4 reflection = reflect(-lightVector, input.Norm);
		float specularFactor = pow(max(dot(reflection, input.ViewDirection), 0.0f), MatPower);

		specularColor = MatSpecular * LightSpe * specularFactor;
	}

	//距离衰减因子
	float d = input.LightVector.w;
	float att = LightAtt0 + LightAtt1 * d + LightAtt2 * d * d;

    finalColor = ambientColor + diffuseColor;
    if(textureOn)
    {
        textureColor = Texture.Sample(Sampler, input.Tex);
        finalColor = finalColor * textureColor;
    }
    finalColor = saturate(finalColor + specularColor)/att;

	return finalColor * softFactor;
}

//聚光光源着色器
float4 PSSpotLight(PS_INPUT input) : SV_Target
{
    float softFactor = 1.0f;
    if(shadowFlag == true)
    {
        softFactor = IsInLightSSM(input.FromLightPos, shadowMap);
    }

    float4  textureColor;
    float4  finalColor = 0;
    float4  ambientColor = 0, diffuseColor = 0, specularColor = 0;

    float4  lightVector = input.LightVector;
    float3 lightDir = LightDir.xyz;
    lightDir = normalize(lightDir);

    //判断光照区域
    float cosTheta = dot(-lightVector.xyz, lightDir);
    if(cosTheta < cos(LightBeta/2))
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    //环境光
    ambientColor = LightAmb * MatAmbient;
    
    //漫反射光
    float diffuseFactor = dot(input.Norm, lightVector);
    if(diffuseFactor > 0)
    {
        diffuseColor = MatDiffuse * LightDiff * diffuseFactor;
    }

    //镜面光
    float4 reflection = reflect(-lightVector, input.Norm);
    reflection = normalize(reflection);
    float specularFactor = pow(max(dot(reflection, input.ViewDirection), 0.0f), MatPower);
    specularColor = MatSpecular * LightSpe * specularFactor;

    //距离衰减因子
    float d = input.LightVector.w;
    float att = LightAtt0 + LightAtt1 * d + LightAtt2 * d * d;

    if(cosTheta > cos(LightAlpha/2))
    {
        finalColor = ambientColor + diffuseColor;
        if(textureOn)
        {
            textureColor = Texture.Sample(Sampler, input.Tex);
            finalColor = finalColor * textureColor;
        }
        finalColor = saturate(finalColor + specularColor);
    }
    else
    {
        float spotFactor = pow((cosTheta - cos(LightBeta / 2)) / (cos(LightAlpha / 2) - cos(LightBeta / 2)), 1);
        finalColor = ambientColor + diffuseColor;
        if(textureOn)
        {
            textureColor = Texture.Sample(Sampler, input.Tex);
            finalColor = finalColor * textureColor;
        }
        finalColor = spotFactor * saturate(finalColor + specularColor)/att;
    }
    
    return finalColor * softFactor;
}

technique11 T_DirLight
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        //SetPixelShader( CompileShader( ps_5_0, PS() ) );
        SetPixelShader( CompileShader( ps_5_0, PSDirectionalLight() ) );
    }
}

technique11 T_PointLight
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSPointLight() ) );
    }
}

technique11 T_SpotLight
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSSpotLight() ) );
    }
}
