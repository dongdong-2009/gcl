
#include "grbezieritemdata.h"
#include <QDomElement>
#include <QPen>
#include <QPainter>
#include <QPointF>
#include <QRectF>

GRBezierItemData::GRBezierItemData()
{
}

void GRBezierItemData::loadItemData(const QDomElement &element)
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
    //以下存贝塞尔坐标点
    f_CP2Y= element.attribute("CP2Y").toFloat();
    f_CP2X= element.attribute("CP2X").toFloat();
    f_CP1Y= element.attribute("CP1Y").toFloat();
    f_CP1X= element.attribute("CP1X").toFloat();
    f_Y2= element.attribute("Y2").toFloat();
    f_X2= element.attribute("X2").toFloat();
    f_Y1= element.attribute("Y1").toFloat();
    f_X1= element.attribute("X1").toFloat();

    this->f_y = element.attribute("Top").toFloat();
    this->f_x = element.attribute("Left").toFloat();
    this->f_height=element.attribute("Height").toFloat();          //所占矩形高度
    this->f_width=element.attribute("Width").toFloat();          //所占矩形宽度

    n_StrokeLineJoin = element.attribute("StrokeLineJoinpsm").toInt();
    n_StrokeEndLineCap = element.attribute("LineCappsm").toInt();

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
    QString dashStr = element.attribute("StrokeDashArray");
    if(!dashStr.isEmpty())
    {
        this->setPenStyle(dashStr);
    }
}

void GRBezierItemData::drawItemData(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);     //设置反锯齿，可以使斜线平滑

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

    //旋转
    if(f_Rotation != 0)
    {
        //计算旋转圆心，为图元所在的矩形中心
        float cenX = (this->f_x + this->f_width) / 2;
        float cenY = (this->f_y + this->f_height) / 2;

        painter->translate(cenX, cenY);     //先将画笔中心移动到旋转中心
        painter->rotate(f_Rotation);        //旋转
        painter->translate(-cenX, -cenY);   //再将画笔中心移回到缺省点
    }

    painter->setOpacity(f_Opacity);                     //设置透明度

    QPainterPath path;

    path.moveTo(f_X1,f_Y1);
    path.cubicTo(f_X2,f_Y2,f_CP1X,f_CP1Y,f_CP2X,f_CP2Y);
    painter->drawPath(path);
    //旋转
    if(f_Rotation != 0)
    {
        //计算旋转圆心，为图元所在的矩形中心
        float cenX = (this->f_x + this->f_width) / 2;
        float cenY = (this->f_y + this->f_height) / 2;

        painter->translate(cenX, cenY);     //先将画笔中心移动到旋转中心
        painter->rotate(-f_Rotation);        //旋转
        painter->translate(-cenX, -cenY);   //再将画笔中心移回到缺省点
    }

}

float GRBezierItemData::getOpacity() const
{
    return f_Opacity;
}

void GRBezierItemData::changDeviceState(int toState)
{
    return;
}

bool GRBezierItemData::isCascadePage() const
{
    return  this->b_cascadePage;
}

QString GRBezierItemData::getCascadePageName() const
{
    return this->m_cascadePageName;
}

GRGlobalParameter::GRItemDataType GRBezierItemData::getItemType() const
{
    return GRGlobalParameter::GR_BezierType;
}

QRectF GRBezierItemData::getItemRect() const
{
     return QRectF(this->f_x,this->f_y,this->f_width,this->f_height);
}

void GRBezierItemData::setPenStyle(const QString &str)
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
        m_penStyle=Qt::CustomDashLine;
    }
    else if(str=="DashDotDot")
    {
        m_penStyle=Qt::DashDotDotLine;
    }
}
