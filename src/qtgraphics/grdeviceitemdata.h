
#ifndef GRDEVICEITEMDATA_H
#define GRDEVICEITEMDATA_H
#include "gritemdatainterface.h"
#include "groupstate.h"

class GCLGRAPHICSSHARED_EXPORT GRDeviceItemData : public GRItemDataInterface
{
public:
    GRDeviceItemData();

public:
    virtual void loadItemData(const QDomElement &element);   //读取数据
    virtual void drawItemData(QPainter *painter);        //画图元
    virtual void changDeviceState(int toState);    //改变设备状态。只有五防设备才操作
    virtual GRGlobalParameter::GRItemDataType getItemType() const;
    virtual QRectF getItemRect() const;                 //返回所占的矩形空间
    virtual bool isCascadePage() const;                 //新增 是否级联到其他页面
    virtual QString getCascadePageName() const;         //新增 级联到的页面名
    virtual float getOpacity() const;   //获取透明度
    virtual void drawUserText(int argc, QPainter *painter, const QString &text, const QString &color);
    virtual void drawVarialRectangle(QPainter *painter, float value);


    int getZIndex() const;
    float getRotation() const;    //获取旋转角度。只有组合符号、设备等需要旋转

    QString getDeviceID() const
    {
        return m_ID;
    }

    QString getDeviceTypeName() const
    {
        return m_DeviceType;
    }

    QString getDeviceMainNumber() const
    {
        return m_DevicesNumber;
    }

    QString getDeviceDoubleNumber() const
    {
        return m_DevicesName;
    }

    QString getDeviceVoltage() const       //电压等级
    {
        return m_voltageClass;
    }

//    QString getDeviceBay() const
//    {
//        return m_bay;
//    }

    float getCenX() const
    {
        return f_x + f_width / 2;
    }

    float getCenY() const
    {
        return f_y + f_height / 2;
    }

    QPointF getItemPos() const
    {
        return QPointF(f_x, f_y);
    }

    QPointF getLabelItemPos() const
    {
        return QPointF(f_LabelX, f_LabelY);
    }

    QString getLabelTextColor() const
    {
        return m_textFillColor;
    }

    int getLabelTextFontSize() const
    {
        return n_textFontSize;
    }

    QString getLabelFontFamily() const
    {
        return m_textFontFamily;
    }

    bool isLabelItemVisbale() const
    {
        if(n_LableVisible == 1)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    int getCurState() const
    {
        return n_curState;
    }

    int getDeviceStateCount() const
    {
        return m_groupVect.count();
    }

    void setDeviceDoubleName(const QString &devName)    //设置双编号
    {
        m_DevicesName = devName;
    }

    void setDeviceTypeName(const QString &typeName)
    {
        m_DeviceType = typeName;
    }

    int getDeviceActiveState() const
    {
        return n_curState;
    }

//    void setDeviceVoltage(const QString &voltage)
//    {
//        m_voltageClass = voltage;
//    }

//    void setDeviceBay(const QString &bay)
//    {
//        m_bay = bay;
//    }

private:
    float f_LabelX;             //文本X坐标
    float f_LabelY;             //文本Y坐标
    float f_Rotation;             //旋转角度
    float f_Opacity;            //透明度
    int n_LableVisible;         //文本是否可见
    int n_Zindex;               //Z value

//    QString m_bay;              //间隔
    QString m_voltageClass;     //电压等级
    QString m_DevicesName;      //设备双编号
    QString m_DeviceType;       //设备类型
    QString m_DevicesNumber;    //设备主编号
    QString m_ID;               //设备ID

    QString m_textFillColor;    //文本颜色
    int n_textFontSize;         //文本字体大小
    QString m_textFontFamily;   //字体类型

    QVector<GroupState *> m_groupVect;

    int n_curState;             //设备当前状态
};

#endif // GRDEVICEITEMDATA_H
