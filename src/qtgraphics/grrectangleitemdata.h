
#ifndef GRRECTANGLEITEMDATA_H
#define GRRECTANGLEITEMDATA_H
#include "gritemdatainterface.h"
#include "fillstyle.h"

class GRRectangleItemData : public GRItemDataInterface
{
public:
    GRRectangleItemData();
    ~GRRectangleItemData();

public:
    virtual void loadItemData(const QDomElement &element);   //读取数据
    virtual void drawItemData(QPainter *painter);        //画图元
    virtual void changDeviceState(int toState);    //改变设备状态。只有五防设备才操作

    virtual GRGlobalParameter::GRItemDataType getItemType() const;
    virtual QRectF getItemRect() const;                 //返回所占的矩形空间

    virtual bool isCascadePage() const;                 //新增 是否级联到其他页面
    virtual QString getCascadePageName() const;         //新增 级联到的页面名
    virtual float getOpacity() const;

    virtual void drawVarialRectangle(QPainter *painter, float value);

    QString getRectName() const
    {
        return m_rectName;
    }

private:
    void SetVp(const QString &str);
    void setPenStyle(const QString &str);

private:
    float f_Rotation;               //旋转角度
    QString m_Fill;                 //填充颜色
    QString m_hatchStyle;           //影线填充类别
    QString m_startColor;           //影像填充开始颜色，即线条颜色
    QString m_endColor;             //影像跳虫结束颜色，即背景颜色
    QString m_Stroke; //线条颜色
    float f_Opacity;
    int f_StrokeThickness;   //线条宽度
    QVector<QPointF> m_Vp;          //多边形坐标点
    qreal q_MaxXPoint;//最右边的x坐标
    qreal q_MixXPoint;//最左边的x坐标
    qreal q_MaxYPoint;//最下面的y坐标
    qreal q_MixYPoint;//最上面的y坐标
    int n_StrokeLineJoin;
    int n_StrokeEndLineCap;

    QString m_rectName;     //矩形名称

    Qt::PenJoinStyle m_joinStyle;   //连接处样式：锐角连接、斜角连接、圆角连接
    Qt::PenCapStyle m_endLineCap;   //线帽样式
    Qt::PenStyle m_penStyle;        //线条样式：实现、虚线等

    FillStyle *fillstyle;
};

#endif // GRRECTANGLEITEMDATA_H
