#include "SkyBox.h"
#include "SRVBatch.h"

SkyBox::SkyBox()
{
    skySRV = NULL;
}

SkyBox::~SkyBox()
{
    skySRV = NULL;
}

BOOL SkyBox::Init(const string &path, const string &fileName)
{
    CreateSphere(50, 80, 40);
    skySRV = SRVBatch::GetInstance()->LoadSRV(path, fileName);
    if(skySRV == NULL)
    {
        return FALSE;
    }
    return TRUE;
}

ID3D11ShaderResourceView* SkyBox::GetSkyResource() const
{
    return skySRV;
}