#include "Scene.h"
#include "Obj.h"
#include "Model.h"
#include "Skeleton.h"
#include "Camera.h"
#include "RenderSystem.h"
#include "QTree.h"
#include "Terrain.h"
#include "Utility.h"

Scene::Scene()
{
    camera = NULL;
    qTree = NULL;
}

Scene::~Scene()
{
    SafeDelete(camera);
    renderTable.clear();
    visiableTable.clear();
    SafeDelete(camera);
}

void Scene::Init(const Rect &rect)
{
    camera = new Camera;
    XMStoreFloat4x4(&proj, XMMatrixPerspectiveFovLH(XM_PIDIV4, 1.2f, 2.0f, 500.0f));
    RenderSystem::GetInstance()->SetProjection(XMLoadFloat4x4(&proj));
    RenderSystem::GetInstance()->SetDepthTest();
    qTree = new QTree(rect);
}

void Scene::AddRenderableThing(RenderableThing &obj)
{
    if(renderTable.find((Key)&obj) == renderTable.end())
    {
        if(obj.IsHandleInput() == TRUE)
        {
            obj.AddObserveTarget(&input);
        }
        renderTable.insert(make_pair<Key, Val>((Key)&obj, Val(&obj)));
        qTree->insert(new QTreePos(obj.GetWorldPosition(), &renderTable[(Key)&obj]));
    }
}

void Scene::DeleteRenderableThing(RenderableThing &obj)
{
    auto it = renderTable.find((Key)&obj);
    if(it != renderTable.end())
    {
        obj.DeleteObserveTarget(&input);
        renderTable.erase(it);
        qTree->remove(&QTreePos(it->second->GetWorldPosition(), nullptr));
    }
}

void Scene::AddSky(SkyBox *sky)
{
    skyBox = shared_ptr<SkyBox>(sky);
}

void Scene::AddTerrain(Terrain *terr)
{
    terrain = shared_ptr<Terrain>(terr);
}

void Scene::HandleInput(Input *inp)
{
    inp->CopyData(&input);
    if(input.IsPressed('w') == TRUE || input.IsPressed('W') == TRUE)
    {
        camera->MoveForwardBy(0.5);
    }
    else if(input.IsPressed('s') == TRUE || input.IsPressed('S') == TRUE)
    {
        camera->MoveForwardBy(-0.5);
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

void Scene::AddLight(Light *li)
{
    light = shared_ptr<Light>(li);
    RenderSystem::GetInstance()->SetLight(light.get());
}

void Scene::Update(double time)
{
    input.InforObservers();
    for(auto it = renderTable.begin(); it != renderTable.end(); ++it)
    {
        it->second->Update((FLOAT)time);
        it->second->_UpdateByFather((FLOAT)time);
        it->second->GetWorldMatrix();
    }
    qTree->Update();
}

void Scene::Draw()
{
    camera->Frame();

    //ÉèÖÃÊÓ¾õ¾ØÕó
    RenderSystem::GetInstance()->SetView(camera->GetViewMatrix());
    
    //ÉèÖÃÑÛ¾¦Î»ÖÃ
    RenderSystem::GetInstance()->SetEye(camera->GetEyePosition());
    XMVECTOR pos = camera->GetEyePosition();
    XMMATRIX world = XMMatrixTranslation(pos.m128_f32[0], pos.m128_f32[1], pos.m128_f32[2]);
    XMMATRIX wvp = world * camera->GetViewMatrix() * XMLoadFloat4x4(&proj);
    if(skyBox)
    {
        RenderSystem::GetInstance()->DrawSky(skyBox.get(), wvp);
    }
    else
    {
        RenderSystem::GetInstance()->ClearBackground();
    }

    VisibleTest();
    for(auto it = visiableTable.begin(); it != visiableTable.end(); ++it)
    {
        UnfoldChildren(*it);
    }

    RenderSystem::GetInstance()->DrawRenderables(visiableTable);    

    if(terrain)
    {
        RenderSystem::GetInstance()->DrawTerrain(terrain.get());
    }
    else
    {
        
    }
    //½»»»äÖÈ¾»º³åÇø
    RenderSystem::GetInstance()->Present();
}

void Scene::Release()
{
    SafeDelete(camera);
    renderTable.clear();
    visiableTable.clear();
    SafeDelete(camera);
}

void Scene::VisibleTest()
{
    visiableTable.clear();
    auto eye = camera->GetEyePosition();
    Position lu(eye.m128_f32[0] - 100, eye.m128_f32[2] + 100);
    Position rd(eye.m128_f32[0] + 100, eye.m128_f32[2] - 100);
    Rect viewArea(lu, rd);
    auto res = qTree->findInRect(viewArea);
    for(int i = 0; i < res.size(); ++i)
    {
        visiableTable.push_back(*static_cast<shared_ptr<RenderableThing>*>(res[i].entity));
    }
}

void Scene::UnfoldChildren(Val &obj)
{
    vector<Val> children = obj->GetChildren();
    for(auto it = children.begin(); it != children.end(); ++it)
    {
        visiableTable.push_back(*it);
        UnfoldChildren(*it);
    }
}