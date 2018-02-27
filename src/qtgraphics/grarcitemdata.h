
#ifndef GRARCITEMDATA_H
#define GRARCITEMDATA_H
#include "gritemdatainterface.h"

class GRArcItemData : public GRItemDataInterface
{
public:
    GRArcItemData();

public:
    virtual void loadItemData(const QDomElement &element);   //读取数据
    virtual void drawItemData(QPainter *painter);        //画图元
    virtual void changDeviceState(int toState);    //改变设备状态。只有五防设备才操作
    virtual GRGlobalParameter::GRItemDataType getItemType() const;
    virtual QRectF getItemRect() const;                 //返回所占的矩形空间
    virtual bool isCascadePage() const;                 //是否级联到其他页面
    virtual QString getCascadePageName() const;         //级联到的页面名
    virtual float getOpacity() const;                   //获取透明度

private:
    void setPenStyle(const QString &str);

private:
    float f_Rotation;             //旋转角度
    QString m_Stroke;           //线条颜色
    float f_Opacity;            //透明度
    float f_StrokeThickness;      //线宽
    float f_Y2;                 //起始点坐标位于基类ItemDataInterface里
    float f_X2;
    float f_MidX;
    float f_MidY;
    float f_CenterX;    //圆心
    float f_CenterY;
    float f_Left;
    float f_Top;
    int n_StrokeLineJoin;
    int n_StrokeEndLineCap;
    QString f_IsLargeArc;
    QString f_SweepDirection;
    Qt::PenJoinStyle m_joinStyle;  //连接处样式：锐角连接、斜角连接、圆角连接
    Qt::PenCapStyle m_endLineCap;  //线帽样式
    Qt::PenStyle m_penStyle;       //线条样式 实线、虚线等
};

#endif // GRARCITEMDATA_H
