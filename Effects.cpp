#include "Effects.h"
#include "Utility.h"

SelfReleaseEffect::SelfReleaseEffect()
{
    effect = NULL;
    techs.clear();
}

SelfReleaseEffect::~SelfReleaseEffect()
{
    techs.clear();
    SafeRelease(effect);
}

BOOL SelfReleaseEffect::Init(ID3D11Device* device, const string& filename)
{
    ID3DBlob *pTechBlob = NULL;
    ID3DBlob *pErrorBlob = NULL;
    wstring exchange;
    exchange = CA2W(filename.c_str());
    LPCWSTR file = exchange.c_str();
    if(FAILED(D3DX11CompileFromFile(file,
        NULL, 
        NULL,
        NULL,
        "fx_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS,
        0,
        NULL,
        &pTechBlob,
        &pErrorBlob,
        NULL)))
    {
        string ttmp = filename + "编译着色器失败";
        exchange = CA2W(ttmp.c_str());
        LPCWSTR temp = exchange.c_str();
        MessageBox(NULL, temp, TEXT("ERROR"), MB_OK);
        return FALSE;
    }

    if( FAILED( D3DX11CreateEffectFromMemory(pTechBlob->GetBufferPointer(),
        pTechBlob->GetBufferSize(),
        0,
        device,
        &effect)))
    {
        string ttmp = filename + "创建渲染效果失败";
        exchange = CA2W(ttmp.c_str());
        LPCWSTR temp = exchange.c_str();
        MessageBox(NULL, temp, TEXT("ERROR"), MB_OK);
        return FALSE;
    }

    D3DX11_EFFECT_DESC effDesc;
    effect->GetDesc(&effDesc);

    for(int i = 0; i < effDesc.Techniques; ++i)
    {
        ID3DX11EffectTechnique  *tech = effect->GetTechniqueByIndex(i);
        D3DX11_TECHNIQUE_DESC   techDesc;
        tech->GetDesc(&techDesc);
        techs.insert(make_pair<string, ID3DX11EffectTechnique*>
            (string(techDesc.Name), move(tech)));
    }

    return TRUE;
}

void SelfReleaseEffect::Release()
{
    techs.clear();
    SafeRelease(effect);
}


EffectMgr::EffectMgr()
{
    currentEffect = NULL;
    currentTech = NULL;
    effects.clear();
}

EffectMgr::~EffectMgr()
{
    SafeRelease(currentEffect);
    effects.clear();
}

BOOL EffectMgr::Init(ID3D11Device* device, const string& path, const vector<string>& filenames)
{
    for(auto it = filenames.begin(); it != filenames.end(); ++it)
    {
        SelfReleaseEffect *temp = new SelfReleaseEffect;
        if(temp->Init(device, path + *it) != FALSE)
        {
            effects.insert(make_pair<string, shared_ptr<SelfReleaseEffect> >
                (string(*it), shared_ptr<SelfReleaseEffect>(temp)));
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

void EffectMgr::Release()
{
    SafeRelease(currentEffect);
    effects.clear();
}

ID3DX11Effect* EffectMgr::GetEffectByName(const string& name)
{
    if(effects.find(name) == effects.end())
    {
        currentEffect = NULL;
        return currentEffect;
    }
    currentEffect = effects[name]->effect;
    return currentEffect;
}

ID3DX11EffectTechnique* EffectMgr::GetTechByName(const string& name)
{
    ID3DX11EffectTechnique *temp = currentEffect->GetTechniqueByName(name.c_str());
    if(temp == NULL)
    {
        currentTech = NULL;
        return currentTech;
    }
    currentTech = temp;
    return currentTech;
}
