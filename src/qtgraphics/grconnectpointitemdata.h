
#ifndef GRCONNECTPOINTITEMDATA_H
#define GRCONNECTPOINTITEMDATA_H
#include "gritemdatainterface.h"

class GRConnectPointItemData : public GRItemDataInterface
{
public:
    GRConnectPointItemData();

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
    QString m_Fill;
    QString m_ID;
    QString m_ParentId;
    bool b_IsConnectionLine;
};

#endif // GRCONNECTPOINTITEMDATA_H
