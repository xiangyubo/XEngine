#ifndef __XENGINE__
#define __XENGINE__

#include "PreHeader.h"


class Input;
class Timer;
class RenderSystem;
class SoundMgr;
class Sound;
class Scene;

class XEngine
{
private:
    Input           *input;         //输入设备（键盘，鼠标）
    Timer           *timer;         //计时器    
    SoundMgr        *soundSystem;  //声音系统
    Sound           *music;
    RenderSystem    *renderSystem;    //渲染系统
   
    //windows应用程序相关组件
    HWND        hwnd;
    LPCWSTR     appName;
    HINSTANCE   hinstance;

    //当前用于渲染的场景
    Scene   *currentScene;
    
public:
    ~XEngine(){};

    static XEngine* GetInstance();

    //初始化引擎
    BOOL Init(HINSTANCE );

    //关闭引擎
    void Shutdown();

    //运行引擎，接受消息队列
    void Run();

    //载入一个场景
    void LoadScene(Scene* );

    //卸载一个场景
    void DumpScene();

    //获取当前场景
    Scene* GetCurrentScene();

    void SetInputHandler(Input* );

    //消息处理函数
    bool CALLBACK MessageHandler(UINT , WPARAM , LPARAM );
    
    //消息处理函数
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

private:
    BOOL InitWnd(int , int );
    XEngine();
    XEngine(const XEngine& );
    XEngine& operator = (const XEngine& );
};

#endif 