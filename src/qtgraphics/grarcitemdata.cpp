
#include "grarcitemdata.h"
#include <QDomElement>
#include <QPen>
#include <QPainter>
#include <QPointF>
#include <QRectF>
//#include <QtMath>
#include <QtCore/qmath.h>

#define PI 3.1415926

GRArcItemData::GRArcItemData()
{
}

void GRArcItemData::loadItemData(const QDomElement &element)
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

    f_Y2 = element.attribute("Y2").toFloat();
    f_X2 = element.attribute("X2").toFloat();
    f_MidY = element.attribute("MidY").toFloat();
    f_MidX = element.attribute("MidX").toFloat();
    f_Left = element.attribute("Left").toFloat();
    f_Top = element.attribute("Top").toFloat();
    f_width = element.attribute("Width").toFloat();
    f_height = element.attribute("Height").toFloat();
    f_CenterX = element.attribute("CenterX").toFloat();
    f_CenterY = element.attribute("CenterY").toFloat();
    f_IsLargeArc = element.attribute("IsLargeArc");
    f_SweepDirection = element.attribute("SweepDirection");

    this->f_y = element.attribute("Y1").toFloat();
    this->f_x = element.attribute("X1").toFloat();

    n_StrokeLineJoin = element.attribute("n_StrokeLineJoin").toInt();
    n_StrokeEndLineCap = element.attribute("n_StrokeEndLineCap").toInt();

    switch(n_StrokeLineJoin)            //连接点设置
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

    switch(n_StrokeEndLineCap)           //线帽设置
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

void GRArcItemData::drawItemData(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);   //设置反锯齿
    QPen pen;
    pen.setWidthF(f_StrokeThickness);          //设置线宽
    if(this->b_commissioning)
    {
        pen.setColor(m_Stroke);                 //设置线条颜色
    }
    else
    {
        pen.setColor(Qt::gray);
    }

    pen.setJoinStyle(m_joinStyle);          //设置连接外样式
    pen.setCapStyle(m_endLineCap);          //设置线帽样式
    pen.setStyle(m_penStyle);               //设置线条样式
    painter->setPen(pen);

    //旋转
    if(f_Rotation != 0)
    {
#if 0
        //计算旋转圆心，为图元所在的矩形中心
        float CenX = (this->f_Left + this->f_width / 2);
        float CenY = (this->f_Top + this->f_height / 2);

        painter->translate(CenX,CenY);    //先将画笔中心移动到旋转中心
        painter->rotate(f_Rotation);      //旋转
        painter->translate(-CenX,-CenY);   //再将画笔中心移动到省缺点
#endif

        painter->translate(f_CenterX, f_CenterY);    //先将画笔中心移动到旋转中心
        painter->rotate(f_Rotation);      //旋转
        painter->translate(-f_CenterX, -f_CenterY);   //再将画笔中心移动到省缺点
    }

    painter->setOpacity(f_Opacity);
    float rrr = sqrt((f_x - f_CenterX) * (f_x - f_CenterX) + (f_y - f_CenterY) * (f_y - f_CenterY));
    if(rrr == 0.0)
    {
        return;
    }
    QRectF rectangle(f_CenterX - rrr , f_CenterY - rrr, 2 * rrr, 2 * rrr);
    qreal startCon = 0.0;
    qreal endCon = 0.0;
    //  确定坐标结束和开始位置
    if(f_X2 > f_CenterX && f_Y2 < f_CenterY)
    {
        endCon = qAcos(fabs(f_X2 - f_CenterX) / rrr) * 180 / PI;
    }else if(f_X2 < f_CenterX && f_Y2 < f_CenterY)
    {
        endCon = (PI - qAcos( fabs(f_X2 - f_CenterX) / rrr)) * 180 / PI;
    }else if(f_X2 < f_CenterX && f_Y2 > f_CenterY)
    {
        endCon = (PI + qAcos(fabs(f_X2 - f_CenterX) / rrr )) * 180/ PI;
    }else if(f_X2 > f_CenterX && f_Y2 > f_CenterY)
    {
        endCon = (2*PI - qAcos( fabs(f_X2 - f_CenterX) / rrr)) * 180/ PI;
    }

    // qDebug() << "endCon = " << endCon;

    if(f_x > f_CenterX && f_y < f_CenterY)
    {
        startCon = qAcos(fabs(f_x - f_CenterX) / rrr) * 180 / PI;
    }else if(f_x < f_CenterX && f_y < f_CenterY)
    {
        startCon = (PI -( qAcos(fabs(f_x - f_CenterX) / rrr )) )* 180 / PI;
    }else if(f_x < f_CenterX && f_y > f_CenterY)
    {
        startCon =  (PI + qAcos(fabs(f_x - f_CenterX) / rrr)) * 180 / PI;
    }else if(f_x > f_CenterX && f_y > f_CenterY)
    {
        startCon = (2*PI - qAcos( fabs(f_x - f_CenterX) / rrr)) * 180 / PI;
    }

    // qDebug() << "startCon = " << startCon;

    int startAngle = startCon * 16;   //弧线开始角度
    int endAngle = endCon * 16;       //弧线结束角度
//  计算drawArc函数所需的3个参数
    if(endAngle > startAngle )
    {
        if(f_SweepDirection == "Counterclockwise")
        {
        int spanAngle = endAngle - startAngle;   //夹角
        // qDebug() << "spanAngle=" << spanAngle;
        painter->drawArc(rectangle,startAngle,spanAngle );
        }else if(f_SweepDirection == "Clockwise")
        {
        int spanAngle = 360*16 - endAngle + startAngle;
        // qDebug() << "spanAngle=" << spanAngle;
        painter->drawArc(rectangle,startAngle,-spanAngle);
        }
    }else if(endAngle < startAngle)
    {
        if(f_SweepDirection == "Counterclockwise")
        {
        int spanAngle = 360*16 - startAngle + endAngle;
        // qDebug() << "spanAngle=" << spanAngle;
        painter->drawArc(rectangle,startAngle,spanAngle );
        }else if(f_SweepDirection == "Clockwise")
        {
        int spanAngle = startAngle - endAngle;
        // qDebug() << "spanAngle=" << spanAngle;
        painter->drawArc(rectangle,startAngle,-spanAngle );
        }

    }
    if(f_Rotation != 0)
    {
#if 0
        //计算旋转圆心，为图元所在的矩形中心
        float cenX = (this->f_Left + this->f_width / 2);
        float cenY = (this->f_Top + this->f_height / 2);

        painter->translate(cenX,cenY);    //先将画笔中心移动到旋转中心
        painter->rotate(f_Rotation);      //旋转
        painter->translate(-cenX,-cenY);   //再将画笔中心移动到省缺点
#endif

        painter->translate(f_CenterX, f_CenterY);    //先将画笔中心移动到旋转中心
        painter->rotate(f_Rotation);      //旋转
        painter->translate(-f_CenterX, -f_CenterY);   //再将画笔中心移动到省缺点
    }

}

void GRArcItemData::changDeviceState(int toState)
{

}

GRGlobalParameter::GRItemDataType GRArcItemData::getItemType() const
{
    return GRGlobalParameter::GR_ArcType;
}

QRectF GRArcItemData::getItemRect() const
{
    return QRectF(f_Left,f_Top,f_width,f_height);
}

bool GRArcItemData::isCascadePage() const                //是否级联到其他页面
{
    return this->b_cascadePage;
}

QString GRArcItemData::getCascadePageName() const            //级联到的页面名
{
    return this->m_cascadePageName;
}

float GRArcItemData::getOpacity() const                      //获取透明度
{
    return f_Opacity;
}

void GRArcItemData::setPenStyle(const QString &str)         //线条格式设置
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
}
