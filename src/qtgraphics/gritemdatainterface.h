
#ifndef GRITEMDATAINTERFACE_H
#define GRITEMDATAINTERFACE_H
#include "graphics_global.h"
#include "grglobalparameter.h"
#include <QRectF>
#include <QString>
#include <QColor>

class QDomElement;
class QPainter;
class GCLGRAPHICSSHARED_EXPORT GRItemDataInterface
{
public:
    GRItemDataInterface();
    virtual ~GRItemDataInterface();

public:
    //设置图元的页面号
    void setPageIndex(int index);
    //获取图元页面号
    int getPageIndex() const;
    //设置是否投运
    void setCommissioning(bool enable);

public:
    virtual void loadItemData(const QDomElement &element) = 0;   //读取数据
    virtual void drawItemData(QPainter *painter) = 0;        //画图元
    virtual void changDeviceState(int toState) = 0;    //改变设备状态。只有五防设备才操作
    virtual GRGlobalParameter::GRItemDataType getItemType() const = 0;
    virtual QRectF getItemRect() const = 0;                 //返回所占的矩形空间
    virtual bool isCascadePage() const = 0;                 //是否级联到其他页面
    virtual QString getCascadePageName() const = 0;         //级联到的页面名
    virtual float getOpacity() const = 0;                   //获取透明度

    //2014.5.30添加，用于画遥测值
    virtual void drawUserText(int argc, QPainter *painter, const QString &text, const QString &color = QString());
    //2014.6.29添加，用于画可变柱状图
    virtual void drawVarialRectangle(QPainter *painter, float value);

protected:
    float f_height;       //高
    float f_width;        //宽
    float f_x;          //x坐标   对于直线、折线等有多个坐标值的，此字段为起始点坐标
    float f_y;          //y坐标   对于直线、折线等有多个坐标值的，此字段为起始点坐标
    int n_pageIndex;    //图元所处的页号
    bool b_cascadePage; //是否级联
    QString m_cascadePageName;  //级联页面名
    bool b_commissioning;   //是否投运，如果不投运，所有线条及填充都是灰色
};

#endif // GRITEMDATAINTERFACE_H
