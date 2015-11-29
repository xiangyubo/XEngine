#include "Obj.h"

class Ball : public Obj
{
private:
    float rotation;
public:
    void Init(float radius)
    {
        Material material;
        material.ambient = XMFLOAT4(1.0f, 0.2f, 0.3f, 1.0f);
        material.diffuse = XMFLOAT4(0.9f, 0.2f, 0.1f, 0.9f);
        material.specular = XMFLOAT4(0.9f, 0.2f, 0.2f, 1.0f);
        material.power = 4.0f;
        float rotation = 0.0f;

        Obj::CreateSphere(radius, radius * XM_2PI * 2, radius * XM_2PI, material);
    }

    void Update(float time)
    {
        rotation = time / 500;
        XMMATRIX temp = XMMatrixMultiply( XMLoadFloat4x4(&world), XMMatrixRotationY(rotation));
        XMStoreFloat4x4(&world, temp);
    }
};