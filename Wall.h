#include "Obj.h"

class Wall : public Obj
{
public:
    void Init(float high, float width, float thick)
    {
        Material material;
        material.ambient = XMFLOAT4(0.75f,0.75f,0.75f,1.0f);
        material.diffuse = XMFLOAT4(0.75f,0.75f,0.75f,1.0f);
        material.specular = XMFLOAT4(0.75f,0.75f,0.75f,1.0f);
        material.power = 3.0f;

        Obj::CreateBox(high, width, thick, material);
    }


};