#ifndef __LIGHT__
#define __LIGHT__

#include"PreHeader.h"

//光照
struct Light
{
    int         type;       //光照类型
    XMFLOAT4    position;   //光源位置
    XMFLOAT4    direction;  //光照方向

    XMFLOAT4    ambient;    //环境光强度
    XMFLOAT4    diffuse;    //漫反射光强度
    XMFLOAT4    specular;   //镜面光强度

    float       attenuation0;   //常量衰减因子
    float       attenuation1;   //一次衰减因子
    float       attenuation2;   //二次衰减因子

    float       alpha;      //聚光内锥角度
    float       beta;       //聚光外锥角度
    float       falloff;    //聚光衰减系数，一般为1.0f
};

struct Material
{
    XMFLOAT4    ambient;    //环境光反射率
    XMFLOAT4    diffuse;    //漫反射光反射率
    XMFLOAT4    specular;   //镜面反射光反射率
    float       power;      //镜面反射系数
    ID3D11ShaderResourceView    *texture;   //纹理视图

    Material()
    {
        ambient.x = ambient.y = ambient.z = ambient.w = 0.5;
        diffuse.x = diffuse.y = diffuse.z = diffuse.w = 0.5;
        specular.x = specular.y = specular.z = specular.w = 0.5;
        power = 0.5;
        texture = NULL;
    }

    ~Material()
    {
        texture = NULL;
    }
};

#endif