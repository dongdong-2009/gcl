
#include "grellipseitemdata.h"
#include <QDomElement>
#include <QPen>
#include <QPainter>
#include <QPointF>
#include <QRectF>
#include <QPolygon>
#include <math.h>
#include "fillstyle.h"

GREllipseItemData::GREllipseItemData() :
    fillstyle(NULL)
{
}

GREllipseItemData::~GREllipseItemData()
{
    if(fillstyle != NULL)
    {
        delete fillstyle;
    }
}

void GREllipseItemData::loadItemData(const QDomElement &element)
{
    f_Rotation=element.attribute("Rotation").toFloat();

    m_Fill=element.attribute("Fill");
    this->f_height=element.attribute("Height").toFloat();          //所占矩形高度
    this->f_width=element.attribute("Width").toFloat(); //所占矩形宽度
    this->f_y=element.attribute("Top").toFloat();                //距离顶部高度
    this->f_x=element.attribute("Left").toFloat();              //距离左边距离
    m_Stroke=element.attribute("Stroke");  //线条颜色
    f_Opacity=element.attribute("Opacity").toFloat();//透明度
    f_StrokeThickness=element.attribute("StrokeThickness").toFloat();    //线条宽度
    this->m_cascadePageName = element.attribute("ConnectPageName");
    this->b_cascadePage = false;
    if(element.attribute("IsConnctToPage") == "True")
    {
         this->b_cascadePage = true;
    }

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

    q_MixXPoint=element.attribute("X").toFloat();
    q_MixYPoint=element.attribute("Y").toFloat();
    q_MaxXPoint=q_MixXPoint+this->f_width;
    q_MaxYPoint=q_MixYPoint+this->f_height;
    QDomNode child = element.firstChild();

    if(child.toElement().tagName() == "Fill")
    {
        fillstyle=new FillStyle(child.toElement());
        fillstyle->SetData( this->f_height,this->f_width,this->q_MaxXPoint,
                           this->q_MixXPoint,this->q_MaxYPoint,this->q_MixYPoint);
    }
}

void GREllipseItemData::drawItemData(QPainter *painter)
{
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
        float cenX=(q_MixXPoint+q_MaxXPoint)/2;
        float cenY=(q_MixYPoint+q_MaxYPoint)/2;
        painter->translate(cenX, cenY);     //先将画笔中心移动到旋转中心
        painter->rotate(f_Rotation);        //旋转
        painter->translate(-cenX, -cenY);   //再将画笔中心移回到缺省点

    }
    painter->setOpacity(f_Opacity);                     //设置透明度

    if(fillstyle!=NULL)
    {
        if(b_commissioning)
        {
            if(fillstyle->m_StrBrush=="LinearGradientBrush")
            {
                QLinearGradient linearGradient=fillstyle->InitLinerBrush();
                painter->setBrush(linearGradient);
            }
            else if(fillstyle->m_StrBrush=="RadialGradientBrush")
            {
                QRadialGradient radialGradient=fillstyle->InitRadialBrush();
                painter->setBrush(radialGradient);
            }
        }
        else
        {
            QBrush brush;
            brush.setStyle(Qt::SolidPattern);
            brush.setColor(Qt::gray);
            painter->setBrush(brush);
        }
    }
    else
    {
        QBrush brush;
        if(!m_Fill.isEmpty())   //有填充
        {
            if(b_commissioning)
            {
                brush.setStyle(Qt::SolidPattern);
                brush.setColor(m_Fill);
            }
            else
            {
                brush.setStyle(Qt::SolidPattern);
                brush.setColor(Qt::gray);
            }
        }
        else                    //无填充
        {
            brush.setStyle(Qt::NoBrush);
        }

        painter->setBrush(brush);
    }
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawEllipse(q_MixXPoint,q_MixYPoint,this->f_width,this->f_height);

    //旋转
    if(f_Rotation != 0)
    {
        float cenX=(q_MixXPoint+q_MaxXPoint)/2;
        float cenY=(q_MixYPoint+q_MaxYPoint)/2;
        painter->translate(cenX, cenY);     //先将画笔中心移动到旋转中心
        painter->rotate(-f_Rotation);        //旋转
        painter->translate(-cenX, -cenY);   //再将画笔中心移回到缺省点
    }

}

void GREllipseItemData::changDeviceState(int toState)
{
     return;
}

float GREllipseItemData::getOpacity() const
{
    return f_Opacity;
}

bool GREllipseItemData::isCascadePage() const
{
    return this->b_cascadePage;
}

QString GREllipseItemData::getCascadePageName() const
{
    return this->m_cascadePageName;
}

GRGlobalParameter::GRItemDataType GREllipseItemData::getItemType() const
{
    return GRGlobalParameter::GR_EllipseType;
}

QRectF GREllipseItemData::getItemRect() const
{
    return QRectF(q_MixXPoint,q_MixYPoint,this->f_width,this->f_height);
}

void GREllipseItemData::SetVp(const QString &str)
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
void GREllipseItemData::setPenStyle(const QString &str)
{
    if(str == "3 1")
    {
        m_penStyle = Qt::DashLine;
    }
    else if(str == "3 1 1 1 1 1")
    {
        m_penStyle = Qt::DashDotDotLine;
    }
    else if(str == "2 2")
    {
        m_penStyle = Qt::DotLine;
    }
    else if(str == "3 1 1 1")
    {
        m_penStyle = Qt::DashDotLine;
    }
}
