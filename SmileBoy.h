#include "Skeleton.h"

class SmileBoy : public Skeleton
{
public:
    void Init()
    {
        LoadSkeleton("skeleton/boy/", "boy.md5mesh");
        LoadAnimation("skeleton/boy/", "boy.md5anim");
        SetScale(0.05);
        SetPosition(0, 0, -4);
    }

    void HandleInput(Input *input)
    {
        if(input->IsPressed('w') == TRUE || input->IsPressed('W') == TRUE)
        {
            SetPosition(x, y, z+0.2);
        }
        else if(input->IsPressed('s') == TRUE || input->IsPressed('S') == TRUE)
        {
            SetPosition(x, y, z-0.2);
        }
        else if(input->IsPressed('a') == TRUE || input->IsPressed('A') == TRUE)
        {
            SetPosition(x-0.2, y, z);
        }
        else if(input->IsPressed('d') == TRUE || input->IsPressed('D') == TRUE)
        {
            SetPosition(x+0.2, y, z+0.05);
        }
    }
};