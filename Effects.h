#ifndef __EFFECTS__
#define __EFFECTS__

#include "PreHeader.h"

struct SelfReleaseEffect
{
    ID3DX11Effect *effect;      //×ÅÉ«Æ÷
    unordered_map<string, ID3DX11EffectTechnique*> techs;   //technique±í

    SelfReleaseEffect();
    ~SelfReleaseEffect();

    BOOL Init(ID3D11Device* device, const string& filename);
    void Release();

private:
    SelfReleaseEffect(const SelfReleaseEffect & );
    SelfReleaseEffect& operator = (const SelfReleaseEffect& );
};

class EffectMgr
{
public:
    EffectMgr();
    virtual ~EffectMgr();

    BOOL Init(ID3D11Device* device, const string& , const vector<string>& );
    void Release();

    ID3DX11Effect* GetEffectByName(const string& );
    ID3DX11EffectTechnique* GetTechByName(const string& );

private:
    EffectMgr(const EffectMgr& );
	EffectMgr& operator=(const EffectMgr& );

protected:
    ID3DX11Effect   *currentEffect;
    ID3DX11EffectTechnique  *currentTech;

    unordered_map<string, shared_ptr<SelfReleaseEffect>>    effects;
};

#endif