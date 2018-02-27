
#include "grtextitemdata.h"
#include <QDomElement>
#include <QColor>
#include <QPainter>

GRTextItemData::GRTextItemData()
{
}

void GRTextItemData::loadItemData(const QDomElement &element)
{
    FontFamily=element.attribute("FontFamily");//字体
    TextContent=element.attribute("TextContent");//文本内容
    f_Rotation=element.attribute("Rotation").toFloat();//旋转角度
    m_Stroke=element.attribute("Fill"); //线条颜色

    this->f_width=element.attribute("Width").toFloat();
    this->f_height=element.attribute("Height").toFloat();//高度

    this->f_x =element.attribute("LableX").toFloat();
    this->f_y=element.attribute("LableY").toFloat();
    fontsize=element.attribute("qsize").toFloat();
    FontFamilyStyle=element.attribute("FontFamilyStyle");

    this->m_cascadePageName = element.attribute("ConnectPageName");
    this->b_cascadePage = false;
    if(element.attribute("IsConnctToPage") == "True")
    {
        this->b_cascadePage = true;
    }
    this->m_reportContent = element.attribute("ReportContext");
}

void GRTextItemData::drawItemData(QPainter *painter)
{
    QColor color(m_Stroke);
    QFont font(FontFamily,fontsize);
    font.setBold(false);
    font.setUnderline(false);
    font.setItalic(false);
    if(FontFamilyStyle=="Bold")//粗体
    {
        font.setBold(true);
    }
    if(FontFamilyStyle=="Italic")//倾斜
    {
        font.setItalic(true);
    }
    if(FontFamilyStyle=="Underline")//带下划线
    {
        font.setUnderline(true);
    }
    if(FontFamilyStyle=="Strikeout")//中间有直线通过的文本
    {
        font.setStrikeOut(true);
    }

    if(f_Rotation!=0)
    {
        float cenX=(this->f_x+this->f_x+this->f_width)/2;
        float cenY=(this->f_y+this->f_y+this->f_height)/2;
        painter->translate(cenX,cenY);
        painter->rotate(f_Rotation);
        painter->translate(-cenX, -cenY);
    }
    painter->setFont(font);
    /*
    Alignment文本水平对其方式
            Center水平剧中
            Far水平居右
            Near水平居左
    FormatFlags文本布局*/
    painter->setPen(QColor(color));
    painter->drawText(QRectF(f_x,f_y, f_width, f_height),Qt::AlignCenter, TextContent);
    if(f_Rotation!=0)
    {
        float cenX=(this->f_x+this->f_x+this->f_width)/2;
        float cenY=(this->f_y+this->f_y+this->f_height)/2;
        painter->translate(cenX,cenY);
        painter->rotate(-f_Rotation);
        painter->translate(-cenX, -cenY);
    }
}

void GRTextItemData::changDeviceState(int toState)
{
    return;
}

bool GRTextItemData::isCascadePage() const
{
    return this->b_cascadePage;
}

QString GRTextItemData::getCascadePageName() const
{
    return this->m_cascadePageName;
}

GRGlobalParameter::GRItemDataType GRTextItemData::getItemType() const
{
    return GRGlobalParameter::GR_TextType;
}

float GRTextItemData::getOpacity() const
{
    return 1;
}

void GRTextItemData::drawUserText(int argc, QPainter *painter, const QString &text, const QString &color)
{
    if(argc == 4)
    {
        TextContent = text;
        m_Stroke = color;
    }
    else
    {
        TextContent = text;
    }
    this->drawItemData(painter);
}

QRectF GRTextItemData::getItemRect() const
{
    return QRectF(this->f_x,this->f_y,this->f_width,this->f_height);
}
