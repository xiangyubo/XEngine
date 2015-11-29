#include "Scene.h"
#include "XEngine.h"

class TestScene : public Scene
{
private:
    double   culTime;
public:
    virtual void HandleInput(Input *inp)
    {
        inp->CopyData(&input);
        if(input.IsPressed('w') == TRUE || input.IsPressed('W') == TRUE)
        {
            camera->MoveForwardBy(0.2f);
        }
        else if(input.IsPressed('s') == TRUE || input.IsPressed('S') == TRUE)
        {
            camera->MoveForwardBy(-0.2f);
        }
        else if(input.IsPressed('a') == TRUE || input.IsPressed('A') == TRUE)
        {
            camera->MoveRightBy(-0.5);
        }
        else if(input.IsPressed('d') == TRUE || input.IsPressed('D') == TRUE)
        {
            camera->MoveRightBy(0.5);
        }
        else if(input.IsPressed('q') == TRUE || input.IsPressed('Q') == TRUE)
        {
            camera->Yaw(0.005);
        }
        else if(input.IsPressed('e') == TRUE || input.IsPressed('E') == TRUE)
        {
            camera->Roll(0.005);
        }
        else if(input.IsPressed('z') == TRUE || input.IsPressed('Z') == TRUE)
        {
            camera->Pitch(0.005);
        }

    }

    virtual void Update(double time)
    {
        input.InforObservers();
        culTime += time;
        //if(culTime < 1000.0)
        //{
        Scene::Update(time);
        //}
        //else
        //{
        //    XEngine::GetInstance()->DumpScene();
        //}
    }

    void SetTime(double tt)
    {
        culTime = tt;
    }
};