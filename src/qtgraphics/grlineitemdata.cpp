
#include "grlineitemdata.h"
#include <QDomElement>
#include <QPen>
#include <QPainter>
#include <QPointF>
#include <QRectF>
#include <QDebug>
//#include <QLinearGradient>

GRLineItemData::GRLineItemData()
{
}

//void LineItemData::~LineItemData()
//{
//}

void GRLineItemData::loadItemData(const QDomElement &element)
{
    f_Rotation = element.attribute("Rotation").toFloat();
    m_Stroke = element.attribute("Stroke");
    f_Opacity = element.attribute("Opacity").toFloat();
    f_StrokeThickness = element.attribute("StrokeThickness").toFloat();
    this->m_cascadePageName = element.attribute("ConnectPageName");
    this->b_cascadePage = false;
    if(element.attribute("IsConnctToPage") == "True")
    {
        this->b_cascadePage = true;
    }

    f_Y2 = element.attribute("Y2").toFloat();;
    f_X2 = element.attribute("X2").toFloat();

    this->f_y = element.attribute("Y1").toFloat();
    this->f_x = element.attribute("X1").toFloat();

    n_StrokeLineJoin = element.attribute("StrokeLineJoin").toInt();
    n_StrokeEndLineCap = element.attribute("n_StrokeEndLineCap").toInt();

    switch(n_StrokeLineJoin)
    {
    case 0:
        m_joinStyle = Qt::BevelJoin;
        break;
    case 1:
        m_joinStyle = Qt::MiterJoin;
        break;
    case 2:
        m_joinStyle = Qt::RoundJoin;
        break;
    default:
        m_joinStyle = Qt::BevelJoin;
        break;
    }

    switch(n_StrokeEndLineCap)  //qt只支持3种线帽
    {
    case 0:
        m_endLineCap = Qt::FlatCap;
        break;
    case 1:
        m_endLineCap = Qt::SquareCap;
        break;
    case 2:
        m_endLineCap = Qt::RoundCap;
        break;
    default:
        m_endLineCap = Qt::FlatCap;
        break;
    }

    m_penStyle = Qt::SolidLine;
    QString dashStr = element.attribute("DashStyle");
    if(!dashStr.isEmpty())
    {
        this->setPenStyle(dashStr);
    }
}

void GRLineItemData::drawItemData(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);     //设置反锯齿，可以使斜线平滑

    if(f_StrokeThickness == 0)  //线宽为0，其他参数都不在设置
    {
        painter->setPen(Qt::NoPen);
    }
    else
    {
        QPen pen;
        pen.setWidthF(f_StrokeThickness);                    //设置线宽
        if(this->b_commissioning)
        {
            pen.setColor(m_Stroke);                 //设置线条颜色
        }
        else
        {
            pen.setColor(Qt::gray);
        }
        pen.setJoinStyle(m_joinStyle);                      //设置连接处样式
        pen.setCapStyle(m_endLineCap);                      //设置线帽样式
        pen.setStyle(m_penStyle);                           //设置线条样式

        painter->setPen(pen);
    }

    //旋转
    if(f_Rotation != 0)
    {
        //计算旋转圆心，为图元所在的矩形中心
        float cenX = (this->f_x + f_X2) / 2;
        float cenY = (this->f_y + f_Y2) / 2;

        painter->translate(cenX, cenY);     //先将画笔中心移动到旋转中心
        painter->rotate(f_Rotation);        //旋转
        painter->translate(-cenX, -cenY);   //再将画笔中心移回到缺省点
    }

    painter->setOpacity(f_Opacity);                     //设置透明度
    painter->drawLine(QPointF(this->f_x, this->f_y), QPointF(f_X2, f_Y2));

    if(f_Rotation != 0)
    {
        //计算旋转圆心，为图元所在的矩形中心
        float cenX = (this->f_x + f_X2) / 2;
        float cenY = (this->f_y + f_Y2) / 2;

        painter->translate(cenX, cenY);     //先将画笔中心移动到旋转中心
        painter->rotate(-f_Rotation);        //旋转
        painter->translate(-cenX, -cenY);   //再将画笔中心移回到缺省点
    }
}

void GRLineItemData::changDeviceState(int toState)
{
    return;
}

GRGlobalParameter::GRItemDataType GRLineItemData::getItemType() const
{
    return GRGlobalParameter::GR_LineType;
}

QRectF GRLineItemData::getItemRect() const
{
    return QRectF(QPointF(this->f_x, this->f_y), QPointF(f_X2, f_Y2));
}

bool GRLineItemData::isCascadePage() const
{
    return this->b_cascadePage;
}

QString GRLineItemData::getCascadePageName() const
{
    return this->m_cascadePageName;
}

float GRLineItemData::getOpacity() const
{
    return f_Opacity;
}

void GRLineItemData::setPenStyle(const QString &str)
{
    if(str == "Solid")
    {
        m_penStyle = Qt::SolidLine;
    }
    else if(str == "Dash")
    {
        m_penStyle = Qt::DashLine;
    }
    else if(str == "Dot")
    {
        m_penStyle = Qt::DotLine;
    }
    else if(str == "DashDot")
    {
        m_penStyle = Qt::DashDotLine;
    }
    else if(str== "Custom")
    {
        m_penStyle = Qt::CustomDashLine;
    }
    else if(str=="DashDotDot")
    {
        m_penStyle = Qt::DashDotDotLine;
    }
    else if(str == "Null")
    {
        m_penStyle = Qt::NoPen;
    }
}
