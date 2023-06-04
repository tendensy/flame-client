#pragma once

#pragma pack(push, 1)
class CVector2D
{
public:
    CVector2D(float X, float Y) { x = X; y = Y ;} 
    float x, y;
};
#pragma pack(pop)