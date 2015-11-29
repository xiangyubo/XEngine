SamplerState SMSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
};

SamplerState PCFSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
};

SamplerState VSMSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
};

SamplerState ESMSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
};

float c = 80.0f;

bool IsInLight(float2 texPos)
{
    if(texPos.x < 0 || texPos.x > 1 || texPos.y < 0 || texPos.y > 1)
    {
        return false;
    }
    return true;
}

//标准阴影贴图计算光照因子
float IsInLightSSM(float4 input, Texture2D shadowMap)
{
    //重构经过插值的坐标
    float3 pos = input.xyz / input.w;

        pos.x =  pos.x/2 + 0.5;
    pos.y = -pos.y/2 + 0.5;

    if(IsInLight(pos) == true)
    {
        //阴影贴图采样深度信息
        float shadowMapDepth = shadowMap.Sample(SMSampler, pos.xy).r;

        //如果裁剪坐标系中的z值比阴影贴图中的值大，则在阴影中
        if( shadowMapDepth < pos.z)
        {
            return 0;
        }
        return 1.0f;
    }
    return 1.0f;
}

//PCF方式计算光照因子
float IsInLightPCF(float4 input, Texture2D shadowMap)
{
    //重构经过插值的坐标
    float3 pos = input.xyz / input.w;

        pos.x =  pos.x/2 + 0.5;
    pos.y = -pos.y/2 + 0.5;

    if(IsInLight(pos) == true)
    {
        float dx = 1 / 512;
        const float2 offsets[9] = 
        {
            float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
            float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
            float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
        };

        //阴影贴图采样深度信息
        float percentLit = 0.0f;
        [unroll]
        for(int i = 0; i < 9; ++i)
        {
            if(pos.z <= shadowMap.Sample(PCFSampler, pos.xy+offsets[i]).r)
            {
                percentLit += 1.0f;
            }
        }

        return percentLit / 9.0f;
    }
    return 1.0f;
}

//VSM计算光照因子
float IsInLightVSM(float4 input, Texture2D shadowMap)
{
    //重构经过插值的坐标
    float3 pos = input.xyz / input.w;

        pos.x =  pos.x/2 + 0.5;
    pos.y = -pos.y/2 + 0.5;

    if(IsInLight(pos) == true)
    {
        //阴影贴图采样深度信息
        float2 moment = shadowMap.Sample(VSMSampler, pos.xy).rg;

            //如果裁剪坐标系中的z值比阴影贴图中的值大，则在阴影中
            if( moment.x < pos.z)
            {
                float variance = moment.y - moment.x*moment.x;
                float d = pos.z - moment.x;
                float pmax = variance / (variance + d*d);
                return pmax;
            }
            return 1.0f;
    }
    return 1.0f;
}

//ESM计算光照因子
float IsInLightESM(float4 input, Texture2D shadowMap)
{
    //重构经过插值的坐标
    float3 pos = input.xyz / input.w;

        pos.x =  pos.x/2 + 0.5;
    pos.y = -pos.y/2 + 0.5;

    if(IsInLight(pos) == true)
    {
        //阴影贴图采样深度信息
        float shadowMapDepth = shadowMap.Sample(SMSampler, pos.xy).b;
        return saturate(shadowMapDepth*exp(-c * pos.z));
    }
    return 1.0f;
}