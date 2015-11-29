#include "Model.h"

class SpaceCompound : public Model
{
public:
    void Init()
    {
        Model::LoadModel("media/", "spaceCompound.obj");
        Pitch(-XM_PIDIV2);
        SetScale(0.3);
    }

    void Update(float time)
    {
        float rotation = time / 300;
        XMMATRIX temp = XMMatrixMultiply( XMLoadFloat4x4(&world), XMMatrixRotationY(rotation));
        XMStoreFloat4x4(&world, temp);
    }
};