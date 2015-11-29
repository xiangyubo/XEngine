#include "Skeleton.h"

class Female : public Skeleton
{
public:
    void Init()
    {
        LoadSkeleton("skeleton/Female/", "Female.md5mesh");
        LoadAnimation("skeleton/Female/", "Female.md5anim");
        SetScale(0.75);
        Yaw(3.14f);
    }

    virtual void Update(float time)
    {

    }

    void MoveForwardBy(float z)
    {
        XMMATRIX temp = XMMatrixMultiply(XMLoadFloat4x4(&move), XMMatrixTranslation(0, 0, z));
        XMStoreFloat4x4(&move, temp); 
    }

    virtual void HandleInput(Input *input)
    {
        if(input->IsPressed('w') == TRUE || input->IsPressed('W') == TRUE)
        {
            MoveForwardBy(-0.2f);
            PlayAnimation(0.04f, 0);
        }
        else if(input->IsPressed('s') == TRUE || input->IsPressed('S') == TRUE)
        {
            MoveForwardBy(0.2f);
        }
        else if(input->IsPressed('a') == TRUE || input->IsPressed('A') == TRUE)
        {
            Yaw(-0.2f);
        }
        else if(input->IsPressed('d') == TRUE || input->IsPressed('D') == TRUE)
        {
            Yaw(0.2f);
        }
    }
};