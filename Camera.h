#ifndef __CAMERA__
#define __CAMERA__

#include "PreHeader.h"

//每帧调用Frame改变一次视图矩阵
class Camera
{
public:
    Camera();
    
    //绕x轴转动，以Pi为单位
    void Pitch(float );

    //绕y轴转动，以Pi为单位
    void Yaw(float );

    //绕z轴转动，以Pi为单位
    void Roll(float );

    //沿z,x,y轴方向移动
    void MoveRightBy(float );
    void MoveUpBy(float );
    void MoveForwardBy(float );

    //变化视图矩阵
    void Frame();

    //设置眼睛的位置
    void SetEye(XMVECTOR& );

    //得到视图矩阵
    const XMMATRIX GetViewMatrix();

    //得到眼睛的位置
    XMVECTOR GetEyePosition();
private:
    XMVECTOR    eye;    //眼睛所在位置
    XMVECTOR    up;     //向上的向量
    XMVECTOR    at;     //观察对象的位置

    XMFLOAT4X4    move;   //平移矩阵
    XMFLOAT4X4    rotate; //旋转矩阵
    XMFLOAT4X4    view;   //视图矩阵(w->v)
};

#endif