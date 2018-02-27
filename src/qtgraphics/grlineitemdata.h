
#ifndef GRLINEITEMDATA_H
#define GRLINEITEMDATA_H
#include "gritemdatainterface.h"

class GRLineItemData : public GRItemDataInterface
{
public:
    GRLineItemData();

public:
    virtual void loadItemData(const QDomElement &element);   //读取数据
    virtual void drawItemData(QPainter *painter);        //画图元
    virtual void changDeviceState(int toState);    //改变设备状态。只有五防设备才操作

    virtual GRGlobalParameter::GRItemDataType getItemType() const;
    virtual QRectF getItemRect() const;

    virtual bool isCascadePage() const;                 //新增 是否级联到其他页面
    virtual QString getCascadePageName() const;         //新增 级联到的页面名

    virtual float getOpacity() const;                   //新增 获取透明度

private:
    void setPenStyle(const QString &str);

private:
    float f_Rotation;             //旋转角度
    QString m_Stroke;           //线条颜色
    float f_Opacity;              //透明度
    float f_StrokeThickness;      //线宽

    float f_Y2;                 //起始点坐标位于基类ItemDataInterface里
    float f_X2;
    int n_StrokeLineJoin;
    int n_StrokeEndLineCap;

    Qt::PenJoinStyle m_joinStyle;   //连接处样式：锐角连接、斜角连接、圆角连接
    Qt::PenCapStyle m_endLineCap;   //线帽样式
    Qt::PenStyle m_penStyle;        //线条样式：实现、虚线等
};

#endif // GRLINEITEMDATA_H
