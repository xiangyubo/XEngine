#include "Obj.h"

class Cube : public Obj
{

public:
    void Init(float l)
    {
        Material material;
        material.ambient = XMFLOAT4(1.0f, 0.3f, 0.4f, 1.0f);
        material.diffuse = XMFLOAT4(0.5f, 0.7f, 0.6f, 0.9f);
        material.specular = XMFLOAT4(0.1f, 0.2f, 0.8f, 1.0f);
        material.power = 4.0f;

        Obj::CreateBox(l, l, l, material);
    }
    void Update(float time)
    {
        float rotation = time / 700;
        XMMATRIX temp = XMMatrixMultiply( XMLoadFloat4x4(&world), XMMatrixRotationY(-rotation));
        XMStoreFloat4x4(&world, temp);
    }
};