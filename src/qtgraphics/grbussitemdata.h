
#ifndef GRBUSSITEMDATA_H
#define GRBUSSITEMDATA_H
#include "gritemdatainterface.h"

class GRPolyLineItemData;
class QLinearGradient;
class GRBussItemData : public GRItemDataInterface
{
public:
    GRBussItemData();
    ~GRBussItemData();

public:
    virtual void loadItemData(const QDomElement &element);   //读取数据
    virtual void drawItemData(QPainter *painter);        //画图元
    virtual void changDeviceState(int toState);    //改变设备状态。只有五防设备才操作

    virtual GRGlobalParameter::GRItemDataType getItemType() const;
    virtual QRectF getItemRect() const;                 //返回所占的矩形空间

    virtual bool isCascadePage() const;                 //新增 是否级联到其他页面
    virtual QString getCascadePageName() const;         //新增 级联到的页面名
    virtual float getOpacity() const;                   //新增 获取透明度

private:
    void loadGradient(const QDomElement &element, int index);

private:
    QString m_deviceMainNumber;             //主编号
    QString m_ID;
    QString m_voltage;
    bool b_IsElectrification;               //是否带电

    GRPolyLineItemData *m_polyLine;           //母线实质上由一个折线组成

    QLinearGradient *m_curGradient;         //当前渐变  带电与否，是用渐变来区分的
    QLinearGradient *m_linearGradient_0;    //不带电
    QLinearGradient *m_linearGradient_1;    //带电
};

#endif // GRBUSSITEMDATA_H
