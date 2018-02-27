
#include "grrectangleitemdata.h"
#include <QPainter>
#include <QDebug>

GRRectangleItemData::GRRectangleItemData() :
    fillstyle(NULL)
{
}

GRRectangleItemData::~GRRectangleItemData()
{
    if(fillstyle != NULL)
    {
        delete fillstyle;
    }
}

void GRRectangleItemData::loadItemData(const QDomElement &element)
{
    f_Rotation=element.attribute("Rotation").toFloat();
    m_Fill=element.attribute("Fill");
    m_hatchStyle=element.attribute("HatchStyle");
    m_startColor=element.attribute("StartColor");
    m_endColor=element.attribute("EndColor");
    this->f_height=element.attribute("Height").toFloat();         //所占矩形高度
    this->f_width=element.attribute("Width").toFloat();             //所占矩形宽度
    this->f_y=element.attribute("Top").toFloat();                //距离顶部高度
    this->f_x=element.attribute("Left").toFloat();              //距离左边距离
    m_Stroke=element.attribute("Stroke");  //线条颜色
    f_Opacity=element.attribute("Opacity").toFloat();//透明度
    f_StrokeThickness=element.attribute("StrokeThickness").toFloat();   //线条宽度
    this->m_cascadePageName = element.attribute("ConnectPageName");
    this->b_cascadePage = false;
    if(element.attribute("IsConnctToPage") == "True")
    {
         this->b_cascadePage = true;
    }

    n_StrokeLineJoin = element.attribute("StrokeLineJoinpsm").toInt();
    n_StrokeEndLineCap = element.attribute("LineCappsm").toInt();

    m_rectName = element.attribute("RectangleName");

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

void GRRectangleItemData::drawItemData(QPainter *painter)
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
            pen.setColor(Qt::darkGray);
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

    if(fillstyle!=NULL) //渐变填充
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
        if(!m_hatchStyle.isEmpty())
        {
            //影线填充，2014.4.3添加
            if(b_commissioning)
            {
                if(m_hatchStyle == "DiagonalCross")
                {
                    brush.setStyle(Qt::DiagCrossPattern);
                }
                else if(m_hatchStyle == "Vertical")
                {
                    brush.setStyle(Qt::VerPattern);
                }
                else if(m_hatchStyle == "Horizontal")
                {
                    brush.setStyle(Qt::HorPattern);
                }
                else if(m_hatchStyle == "LargeGrid")
                {
                    brush.setStyle(Qt::CrossPattern);
                }
                else if(m_hatchStyle == "BackwardDiagonal")
                {
                    brush.setStyle(Qt::BDiagPattern);
                }
                else if(m_hatchStyle == "ForwardDiagonal")
                {
                    brush.setStyle(Qt::FDiagPattern);
                }

                else if(m_hatchStyle == "Percent20")
                {
                    brush.setStyle(Qt::Dense6Pattern);
                }
                else if(m_hatchStyle == "Percent60")
                {
                    brush.setStyle(Qt::Dense3Pattern);
                }
                else if(m_hatchStyle == "Percent50")
                {
                    brush.setStyle(Qt::Dense4Pattern);
                }
                else if(m_hatchStyle == "Percent30")
                {
                    brush.setStyle(Qt::Dense5Pattern);
                }
                else if(m_hatchStyle == "Percent80")
                {
                    brush.setStyle(Qt::Dense1Pattern);
                }
                else if(m_hatchStyle == "Percent75")
                {
                    brush.setStyle(Qt::Dense2Pattern);
                }
                else
                {
                    brush.setStyle(Qt::SolidPattern);
                    brush.setColor(m_Fill);
                }
            }
            else
            {
                brush.setStyle(Qt::SolidPattern);
                brush.setColor(Qt::gray);
            }
            brush.setColor(m_startColor);
            painter->fillRect(q_MixXPoint,q_MixYPoint,this->f_width,this->f_height, QColor(m_endColor));
        }
        else
        {
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
        }

        painter->setBrush(brush);
    }

    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRect(q_MixXPoint,q_MixYPoint,this->f_width,this->f_height);


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

void GRRectangleItemData::changDeviceState(int toState)
{
     return;
}

bool GRRectangleItemData::isCascadePage() const
{
    return this->b_cascadePage;
}

QString GRRectangleItemData::getCascadePageName() const
{
    return this->m_cascadePageName;
}

GRGlobalParameter::GRItemDataType GRRectangleItemData::getItemType() const
{
    return GRGlobalParameter::GR_RectangleType;
}

QRectF GRRectangleItemData::getItemRect() const
{
    return QRectF(q_MixXPoint,q_MixYPoint,this->f_width,this->f_height);
}

float GRRectangleItemData::getOpacity() const
{
    return f_Opacity;
}

void GRRectangleItemData::drawVarialRectangle(QPainter *painter, float value)
{
    if(m_rectName != "$$variableRectangle")
    {
        drawItemData(painter);
    }
    else
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
                pen.setColor(Qt::darkGray);
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

        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        //设置颜色，0%-30%为绿色，30%-70%为黄色，70%-100%为红色
        //这里使用高度
        float vaTime = value / this->f_height;
        if(vaTime > 0.0f && vaTime < 0.31f)
        {
            brush.setColor(Qt::green);
        }
        else if(vaTime > 0.3f && vaTime < 0.71f)
        {
            brush.setColor(Qt::yellow);
        }
        else if(vaTime > 0.7f && vaTime < 1.0f)
        {
            brush.setColor(Qt::red);
        }
        else
        {
            return;
        }

        painter->setBrush(brush);
        painter->setRenderHint(QPainter::Antialiasing);


        painter->drawRect(q_MixXPoint, this->f_height - value + q_MixYPoint,this->f_width,/*this->f_height - */value);


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
}

void GRRectangleItemData::SetVp(const QString &str)
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
void GRRectangleItemData::setPenStyle(const QString &str)
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
