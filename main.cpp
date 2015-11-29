#include "XEngine.h"
#include "Scene.h"
#include "Light.h"
#include "Cube.h"
#include "Ball.h"
#include "Timer.h"
#include "Model.h"
#include "Ship.h"
#include "Wall.h"
#include "Skeleton.h"
#include "SmileBoy.h"
#include "SkyBox.h"
#include "Particle.h"
#include "Sound.h"
#include "Camera.h"
#include "Geometry.h"
#include "RenderSystem.h"
#include "Terrain.h"
#include "SRVBatch.h"
#include "SpaceCompound.h"
#include "Female.h"
#include "TestScene.h"

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    XEngine *engine = XEngine::GetInstance();
    if( engine->Init(hInstance) == FALSE )
    {
        return -1;
    }

    Rect rec(Position(-10000.0, 10000.0), Position(10000.0, -10000.0));
    TestScene *scene = new TestScene;
    scene->SetTime(0);
    //Scene *scene = new Scene;
    scene->Init(rec);
    scene->camera->MoveForwardBy(-40);
    scene->camera->MoveUpBy(18);
    scene->camera->Pitch(-XM_PI / 10);

    Material material;
    material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.power = 9.0f;
    //material.texture = SRVBatch::GetInstance()->LoadSRV("media/", "cup.jpg");

    Particle *fire = new Particle;
    fire->Init("media/","fire.png");
    fire->SetPosition(0, 0, 10);
    //scene->AddRenderableThing(*fire);

    SkyBox *sky = new SkyBox;
    //sky->Init("textures/", "skymap.dds");
    sky->Init("textures/", "Above_The_Sea.dds");
    //sky->Init("textures/", "desert_skymap.dds");
    scene->AddSky(sky);

    Terrain *terrain = new Terrain;
    //terrain->Init("terrain/testHight.bmp", "terrain/", "dirt01.dds");
    terrain->Init("terrain/heightmap01.bmp", "terrain/", "grass.jpg");
    scene->AddTerrain(terrain);

    Wall *wall = new Wall;
    wall->Init(30, 30, 0.3);
    wall->Pitch(XM_PI / 2);
    wall->SetPosition(0, -3, 0);
    //scene->AddRenderableThing(*wall);

    //¿É¼ûĞÔ²âÊÔ
    Wall *wall0 = new Wall;
    wall0->Init(30, 30, 0.3);
    wall0->Pitch(XM_PI / 2);
    wall0->SetPosition(1000, -3, 0);
    //scene->AddRenderableThing(*wall0);

    Obj *obj = new Obj;
    obj->CreateBox(1, 3, 2, material);
    obj->SetPosition(5, 1, 12);
    obj->Yaw(XM_PI / 3);
    obj->Roll(XM_PI / 3);
    obj->SetScale(1.3);
    //scene->AddRenderableThing(*obj);

    Cube *cube = new Cube;
    cube->Init(2);
    cube->Pitch(XM_PI / 3);
    cube->Roll(XM_PI / 3);
    cube->SetMaterial(material);
    //scene->AddRenderableThing(*cube);

    Ball *ball = new Ball;
    ball->Init(1);
    ball->SetPosition(0, 0, 6);

    Obj *objInWall = new Obj;
    objInWall->CreateBox(1, 1, 1, material);
    objInWall->SetPosition(0, 1, 0);
    ball->AddChild(objInWall);

    //scene->AddRenderableThing(*ball);
    

    Ship *ship = new Ship;
    //ship->Init();
    //ship->SetPosition(-10, 0, 0);
    //scene->AddRenderableThing(*ship);

    Model *test = new Model;
    test->LoadModel("media/", "chair.obj");
    test->SetScale(1.7);
    test->SetPosition(-15, 0, 0);
    test->Pitch(-1.2);
    test->Yaw(-1.5);
    //scene->AddRenderableThing(*test);


    //SmileBoy *smileBoy= new SmileBoy;
    //smileBoy->Init();
    //smileBoy->SetHandleInput(FALSE);
    //scene->AddRenderableThing(*smileBoy);

    Female *female = new Female;
    female->Init();
    //female->SetPosition(-7, 0,0);
    female->SetHandleInput(TRUE);
    scene->AddRenderableThing(*female);

    RenderSystem::GetInstance()->SetShadowOpen(true);
    engine->LoadScene(scene);


    {
        Light *light = new Light;
        light->type = 0;
        light->position = XMFLOAT4(0.0f, 5.0f, -5.0f, 1.0f);
        XMStoreFloat4(&light->direction, XMVector4Normalize(XMVectorSet(1.0f, -1.0f, 0.0f, 0.0f)));
        light->ambient = XMFLOAT4(0.4f, 0.2f, 0.3f, 1.0f);
        light->diffuse = XMFLOAT4(0.3f, 0.4f, 0.5f, 1.0f);
        light->specular = XMFLOAT4(0.5f, 0.3f, 0.2f, 1.0f);
        light->attenuation0 = 0.0f;
        light->attenuation1 = 0.0f;
        light->attenuation2 = 0.0f;
        light->alpha = 0.0f;
        light->beta = 0.0f;
        light->falloff = 0.0f;
        scene->AddLight(light);
    }
    //*******************************************
    engine->Run();

    return 0; 
}
