
#include "grpolylineitemdata.h"
#include <QPainter>
#include <QDomElement>
#include <QPen>
#include <QPainter>
#include <QPointF>
#include <QRectF>

GRPolyLineItemData::GRPolyLineItemData():m_gradient(NULL)
{
}

void GRPolyLineItemData::loadItemData(const QDomElement &element)
{
    f_Rotation = element.attribute("Rotation").toFloat();
    m_Stroke = element.attribute("Stroke");
    f_Opacity = element.attribute("Opacity").toFloat();
    f_StrokeThickness = element.attribute("StrokeThickness").toFloat();
    this->m_cascadePageName = element.attribute("ConnectPageName");

    this->f_height=element.attribute("Height").toFloat();          //所占矩形高度
    this->f_width=element.attribute("Width").toFloat();            //所占矩形宽度

    this->b_cascadePage = false;
    if(element.attribute("IsConnctToPage") == "True")
    {
        this->b_cascadePage = true;
    }
    this->f_y = element.attribute("Y").toFloat();
    this->f_x = element.attribute("X").toFloat();

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

void GRPolyLineItemData::drawItemData(QPainter *painter)
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
    if(m_gradient!=NULL)//为画笔设置画刷
    {
        pen.setBrush(*m_gradient);

    }
    painter->setPen(pen);
    if(f_Rotation != 0)
    {
        //计算旋转圆心，为图元所在的矩形中心
        float cenX=(q_MixXPoint+q_MaxXPoint)/2;
        float cenY=(q_MixYPoint+q_MaxYPoint)/2;
        painter->translate(cenX, cenY);     //先将画笔中心移动到旋转中心
        painter->rotate(f_Rotation);        //旋转
        painter->translate(-cenX, -cenY);   //再将画笔中心移回到缺省点
    }
    painter->setOpacity(f_Opacity);                     //设置透明度
    painter->drawPolyline(m_Vp.constData(),m_Vp.count());//绘制折线

    //旋转
    if(f_Rotation != 0)
    {
        //计算旋转圆心，为图元所在的矩形中心
        float cenX=(q_MixXPoint+q_MaxXPoint)/2;
        float cenY=(q_MixYPoint+q_MaxYPoint)/2;
        painter->translate(cenX, cenY);     //先将画笔中心移动到旋转中心
        painter->rotate(-f_Rotation);        //旋转
        painter->translate(-cenX, -cenY);   //再将画笔中心移回到缺省点
    }
}

void GRPolyLineItemData::changDeviceState(int toState)
{
    return;
}
bool GRPolyLineItemData::isCascadePage() const
{
    return this->b_cascadePage;
}
// 级联到的页面名
QString GRPolyLineItemData::getCascadePageName() const
{
    return this->m_cascadePageName;
}

float GRPolyLineItemData::getOpacity() const
{
    return f_Opacity;
}

GRGlobalParameter::GRItemDataType GRPolyLineItemData::getItemType() const
{
    return GRGlobalParameter::GR_PolylineType;
}

QRectF GRPolyLineItemData::getItemRect() const
{
   return QRectF(this->f_x,this->f_y,this->f_width,this->f_height);
}
void GRPolyLineItemData::setPenStyle(const QString &str)
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
void GRPolyLineItemData::SetVp(const QString &str)
{
    QStringList strList = str.split(QChar(','));
    q_MaxXPoint=0;
    q_MaxYPoint=0;
    q_MixXPoint=10000;
    q_MixYPoint=10000;
    //int index=1;
    float temp = 0.0;
    for(int index=0;index<strList.count();index++)
    {
        float a = strList.at(index).toFloat();//以下找出xy的最大最小坐标
        if(index%2==0)//x坐标
        {
            if(a>q_MaxXPoint)
                q_MaxXPoint=a;
            if(a<q_MixXPoint)
                q_MixXPoint=a;
            temp = a;
        }
        else//y坐标
        {
            if(a>q_MaxYPoint)
                q_MaxYPoint=a;
            if(a<q_MixYPoint)
                q_MixYPoint=a;
            QPointF point(temp, a);
            m_Vp.append(point);
        }
    }

}
void GRPolyLineItemData::setRadient(QLinearGradient *gradient)
{
    m_gradient=gradient;
}
