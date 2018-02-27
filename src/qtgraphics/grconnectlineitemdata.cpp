
#include "grconnectlineitemdata.h"
#include <QDomElement>
#include <QPainter>

GRConnectLineItemData::GRConnectLineItemData()
{
}

void GRConnectLineItemData::loadItemData(const QDomElement &element)
{
    f_Rotation = element.attribute("Rotation").toFloat();
    m_Stroke = element.attribute("Stroke");
    f_Opacity = element.attribute("Opacity").toFloat();
    f_StrokeThickness = element.attribute("StrokeThickness").toFloat();
    this->m_cascadePageName = element.attribute("ConnctToPage");
    this->b_cascadePage = false;
    if(element.attribute("IsConnctToPage") == "True")
    {
        this->b_cascadePage = true;
    }
    this->f_y = element.attribute("X").toFloat();
    this->f_x = element.attribute("Y").toFloat();

    n_StrokeLineJoin = element.attribute("StrokeLineJoinpsm").toInt();
    n_LineCap = element.attribute("LineCappsm").toInt();

    n_StrokeEndLineCap = element.attribute("StrokeEndLineCap").toInt();
    n_StrokeStartLineCap = element.attribute("StrokeStartLineCap").toInt();
    m_ID=element.attribute("ID");
    m_SourceID=element.attribute("SourceID");
    m_SinkID=element.attribute("SinkID");
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

    switch(n_LineCap)  //qt只支持3种线帽
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
    QString pcStr=element.attribute("PC");
    if(!pcStr.isEmpty())
    {
        SetVp(pcStr);
    }
}

void GRConnectLineItemData::drawItemData(QPainter *painter)
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

    painter->setOpacity(f_Opacity);                     //设置透明度

    painter->drawPolyline(m_Vp.constData(),m_Vp.count());//绘制折线
}

void GRConnectLineItemData::changDeviceState(int toState)
{
}

float GRConnectLineItemData::getOpacity() const
{
    return f_Opacity;
}

GRGlobalParameter::GRItemDataType GRConnectLineItemData::getItemType() const
{
    return GRGlobalParameter::GR_ConnectLineType;
}

QRectF GRConnectLineItemData::getItemRect() const
{
    return QRectF(f_MinX,f_MinY,f_MaxX-f_MinX,f_MaxY-f_MinY);
}

bool GRConnectLineItemData::isCascadePage() const                 //新增 是否级联到其他页面
{
    return this->b_cascadePage;
}

QString GRConnectLineItemData::getCascadePageName() const         //新增 级联到的页面名
{
    return this->m_cascadePageName;
}
void GRConnectLineItemData::setPenStyle(const QString &str)
{
    if(str == "Dash")
    {
        m_penStyle = Qt::DashLine;
    }
    else if(str == "DashDotDot")
    {
        m_penStyle = Qt::DashDotDotLine;
    }
    else if(str == "Dot")
    {
        m_penStyle = Qt::DotLine;
    }
    else if(str == "DashDot")
    {
        m_penStyle = Qt::DashDotLine;
    }
    else if(str == "Solid")
    {
        m_penStyle = Qt::SolidLine;
    }
}
void GRConnectLineItemData::SetVp(const QString &str)
{
    f_MinX=100000;
    f_MaxX=0;
    f_MinY=100000;
    f_MaxY=0;
    QStringList strList = str.split(QChar(';'));
    for(int index=0;index<strList.count();index++)
    {
        QStringList str = strList.at(index).split(QChar(','));
        float fx=0.0;
        float fy=0.0;
        fx=str.at(0).toFloat();
        fy=str.at(1).toFloat();

        if(f_MinX>fx)
            f_MinX=fx;
        if(f_MaxX<fx)
            f_MaxX=fx;
        if(f_MinY>fy)
            f_MinY=fy;
        if(f_MaxY<fy)
            f_MaxY=fy;
        QPointF point(fx, fy);
        m_Vp.append(point);
    }

}
