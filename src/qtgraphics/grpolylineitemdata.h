

#ifndef GRPOLYLINEITEMDATA_H
#define GRPOLYLINEITEMDATA_H
#include "gritemdatainterface.h"
#include <QVector>

class QLinearGradient;
class GRPolyLineItemData : public GRItemDataInterface
{
public:
    GRPolyLineItemData();

public:
    virtual void loadItemData(const QDomElement &element);   //读取数据
    virtual void drawItemData(QPainter *painter);        //画图元
    virtual void changDeviceState(int toState);    //改变设备状态。只有五防设备才操作

    virtual GRGlobalParameter::GRItemDataType getItemType() const;
    virtual QRectF getItemRect() const;                 //返回所占的矩形空间

    virtual bool isCascadePage() const;                 //新增 是否级联到其他页面
    virtual QString getCascadePageName() const;         //新增 级联到的页面名
    virtual float getOpacity() const;
    void setRadient(QLinearGradient *gradient);         //设置母线渐变

private:
    void setPenStyle(const QString &str);

private:
    QLinearGradient  *m_gradient;//设置线条渐变
    float f_Rotation;             //旋转角度
    QString m_Stroke;           //线条颜色
    float f_Opacity;              //透明度
    float f_StrokeThickness;      //线宽
    float f_Y2;                 //起始点坐标位于基类ItemDataInterface里
    float f_X2;
    int n_StrokeLineJoin;
    int n_StrokeEndLineCap;
    QVector<QPointF> m_Vp;          //折线坐标点
    Qt::PenJoinStyle m_joinStyle;   //连接处样式：锐角连接、斜角连接、圆角连接
    Qt::PenCapStyle m_endLineCap;   //线帽样式
    Qt::PenStyle m_penStyle;        //线条样式：实现、虚线等
    qreal q_MaxXPoint;//最右边的x坐标
    qreal q_MixXPoint;//最左边的x坐标
    qreal q_MaxYPoint;//最下面的y坐标
    qreal q_MixYPoint;//最上面的y坐标

    void SetVp(const QString &str);
};

#endif // GRPOLYLINEITEMDATA_H
