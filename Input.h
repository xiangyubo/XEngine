#ifndef __INPUT__
#define __INPUT__

#include"PreHeader.h"

class RenderableThing;

class Input
{
public:
    struct MouseState
    {
        POINTS  location;       //鼠标位置
        BOOL    leftButton;     //左键是否按下
        BOOL    righBtutton;    //右键是否按下
    };

    Input(){}

    virtual ~Input(){}

    BOOL Init(int , int );

    //复制输入信息，不拷贝观察者队列
    void CopyData(Input *);

    //查询键盘是否按下某个按键
    BOOL IsPressed(unsigned int );
    
    //获得鼠标状态
    MouseState GetMouseState();
    
    //每帧调用一次，用于更新键盘和鼠标状态
    void Frame(UINT uMessage, WPARAM wParam, LPARAM lParam);

    //添加观察者
    void AddObserver(RenderableThing *);

    //删除观察者
    void DeleteObserver(RenderableThing *);

    //通知所有观察者
    void InforObservers();

private:
    static const unsigned int MAX_KEYS = 256;   //键盘最大按键
    BOOL keyboard[MAX_KEYS];    //键盘按键记录
    MouseState  mouse;  //鼠标状态
    vector<RenderableThing*> observers;
};

#endif