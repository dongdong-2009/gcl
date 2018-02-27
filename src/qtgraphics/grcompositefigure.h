
#ifndef GRCOMPOSITEFIGURE_H
#define GRCOMPOSITEFIGURE_H
#include "gritemdatainterface.h"

class GroupState;
class GCLGRAPHICSSHARED_EXPORT GRCompositeFigure : public GRItemDataInterface
{
public:
    GRCompositeFigure();
    ~GRCompositeFigure();

public:
    virtual void loadItemData(const QDomElement &element);   //读取数据
    virtual void drawItemData(QPainter *painter);        //画图元
    virtual void changDeviceState(int toState);    //改变设备状态。只有五防设备才操作

    virtual GRGlobalParameter::GRItemDataType getItemType() const;
    virtual QRectF getItemRect() const;

    virtual bool isCascadePage() const;                 //新增 是否级联到其他页面
    virtual QString getCascadePageName() const;         //新增 级联到的页面名

    virtual float getOpacity() const;

    float getRotation() const;    //获取旋转角度。只有组合符号、设备等需要旋转

    float getCenX() const
    {
        return f_x + f_width / 2;
    }

    float getCenY() const
    {
        return f_y + f_height / 2;
    }

private:
    float f_Rotation;             //旋转角度
    float f_Opacity;
    GroupState *m_groupState;   //非设备，只有一个组合
};

#endif // GRCOMPOSITEFIGURE_H
