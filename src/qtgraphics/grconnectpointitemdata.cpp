
#include "grconnectpointitemdata.h"
#include <QDomElement>

GRConnectPointItemData::GRConnectPointItemData()
{
}

void GRConnectPointItemData::loadItemData(const QDomElement &element)
{
    this->f_height = element.attribute("Height").toFloat();
    this->f_width = element.attribute("Width").toFloat();
    this->f_y = element.attribute("Y").toFloat();
    this->f_x = element.attribute("X").toFloat();
    m_Fill=element.attribute("Fill");
    m_ID=element.attribute("ID");
    m_ParentId=element.attribute("ParentId");
    b_IsConnectionLine=false;
    if(element.attribute("IsConnectionLine") == "true")
    {
        b_IsConnectionLine= true;
    }
}

void GRConnectPointItemData::drawItemData(QPainter *painter)
{
//    QPen pen;
//    QBrush brush;
//    brush.setStyle(Qt::SolidPattern);
//    brush.setColor(Qt::green);
//    painter->setBrush(brush);
//    pen.setColor(m_Fill);
//    painter->setPen(pen);
//    painter->drawEllipse(this->f_x-2,this->f_y-2-10,4,4);
}

void GRConnectPointItemData::changDeviceState(int toState)
{

}

GRGlobalParameter::GRItemDataType GRConnectPointItemData::getItemType() const
{
    return GRGlobalParameter::GR_ConnectPointType;
}

QRectF GRConnectPointItemData::getItemRect() const
{
    return QRectF(this->f_x,this->f_y,this->f_width,this->f_height);
}

bool GRConnectPointItemData::isCascadePage() const                 //新增 是否级联到其他页面
{
    return false;
}

float GRConnectPointItemData::getOpacity() const
{
    return 1;
}

QString GRConnectPointItemData::getCascadePageName() const         //新增 级联到的页面名
{
    return "";
}
