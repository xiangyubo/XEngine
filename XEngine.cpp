#include "XEngine.h"
#include "Input.h"
#include "Scene.h"
#include "Timer.h"
#include "RenderSystem.h"
#include "Sound.h"
#include "Utility.h"

static const int SCREEN_WIDTH = 700;//GetSystemMetrics(SM_CXSCREEN);
static const int SCREEN_HEIGHT = 600;//GetSystemMetrics(SM_CYSCREEN);

XEngine::XEngine()
{
    input = NULL;
    timer = NULL;
    renderSystem = NULL;

    hwnd = NULL;
    appName = NULL;
    hinstance = NULL;

    currentScene = NULL;
}

XEngine* XEngine::GetInstance()
{
    static XEngine instance;
    return &instance;
}

BOOL XEngine::Init(HINSTANCE instance)
{
    input = new Input();
    if(!input || input->Init(SCREEN_WIDTH, SCREEN_HEIGHT) == FALSE)
    {
        MessageBox(hwnd, L"初始化输入设备失败", L"Error", MB_OK);
        return FALSE;
    }

    hinstance = instance;
    appName = L"XEngine";
    if(InitWnd(SCREEN_WIDTH, SCREEN_HEIGHT) != TRUE)
    {
        return FALSE;
    }

    renderSystem = RenderSystem::GetInstance();
    if(!renderSystem || renderSystem->Init(hwnd) == FALSE)
    {
        MessageBox(hwnd, L"初始化渲染系统失败", L"Error", MB_OK);
        return FALSE;
    }

    timer = new Timer;
    if(timer->InitTimer() == false)
    {
        MessageBox(hwnd, L"初始化计时器失败", L"Error", MB_OK);
        return FALSE;
    }

    soundSystem = SoundMgr::GetInstance();
    if(soundSystem->Init(hwnd) == FALSE)
    {
        MessageBox(hwnd, L"初始化声音系统失败", L"Error", MB_OK);
        return FALSE;
    }

    music = new Sound;
    music->LoadFile("media/qs.wav");
    music->Play();

    return TRUE;
}

void XEngine::Run()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while(TRUE)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))    
        {                                                                                               
            TranslateMessage(&msg);                     
            DispatchMessage(&msg);
        }
        if(msg.message == WM_QUIT)
        {
            break;
        }
        if(currentScene != NULL)
        {   
            //场景获得当前输入
            currentScene->HandleInput(input);

            //更新场景
            currentScene->Update(timer->GetDelta());

            //渲染场景
            if(currentScene != NULL)
            {
                currentScene->Draw();
            }
        }
    }
}

BOOL XEngine::InitWnd(int w, int h)
{
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);

    wc.hInstance = hinstance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName= appName;

    wc.hIcon = LoadIcon(hinstance, (LPCTSTR)IDI_APPLICATION);
    wc.hIconSm = LoadIcon( wc.hInstance, ( LPCTSTR )IDI_APPLICATION );
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszMenuName = NULL;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;

    if (RegisterClassEx(&wc) == FALSE)
    {
        MessageBox(hwnd, L"注册WIN32窗体失败!", NULL, 0);  
        return FALSE;
    }

    RECT wr = {0, 0, w, h};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    hwnd = CreateWindowEx(NULL, 
        appName,
        appName,
        WS_OVERLAPPEDWINDOW,
        100,
        30,
        wr.right - wr.left,
        wr.bottom - wr.top,
        NULL,
        NULL,
        hinstance, 
        (void*)this);


    if( !hwnd )
    {
        MessageBox(hwnd, L"注册WIN32窗体失败!", NULL, 0);  
        return FALSE;
    }

    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    return TRUE;
}

bool CALLBACK XEngine::MessageHandler(UINT msg, WPARAM wparam, LPARAM lparam)
{
    input->Frame(msg, wparam, lparam);
    return true;
}

LRESULT CALLBACK XEngine::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch(umessage)
    {
    case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

    case WM_CLOSE:
        {
            PostQuitMessage(0);		
            return 0;
        }

    default:
        {
            XEngine::GetInstance()->MessageHandler(umessage, wparam, lparam);
            return DefWindowProc(hwnd, umessage, wparam, lparam);
        }
    }
}

void XEngine::LoadScene(Scene* sce)
{
    currentScene = sce;
}

void XEngine::DumpScene()
{
    if(currentScene != NULL)
    {
        currentScene->Release();
        delete currentScene;
        currentScene = NULL;
    }
}

Scene* XEngine::GetCurrentScene()
{
    return currentScene;
}

void XEngine::Shutdown()
{
    if(renderSystem != NULL)
    {
        renderSystem->Shutdown();
    }
    SafeDelete(input);
    SafeDelete(timer);
    SafeRelease(currentScene);
}
