#ifndef __SKYBOX__
#define __SKYBOX__

#include "PreHeader.h"
#include "Obj.h"

class SkyBox :public Obj
{
private:
    ID3D11ShaderResourceView* skySRV;
public:
    SkyBox();
    ~SkyBox();
    BOOL Init(const string &, const string & );
    ID3D11ShaderResourceView* GetSkyResource() const;
private:
    SkyBox(const SkyBox& );
    SkyBox& operator=(const SkyBox& );
};

#endif
