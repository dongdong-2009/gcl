
#ifndef GRTEXTITEMDATA_H
#define GRTEXTITEMDATA_H
#include "gritemdatainterface.h"
#include <QFont>

class GRTextItemData : public GRItemDataInterface
{
public:
    GRTextItemData();

public:
    virtual void loadItemData(const QDomElement &element);   //读取数据
    virtual void drawItemData(QPainter *painter);        //画图元
    virtual void changDeviceState(int toState);    //改变设备状态。只有五防设备才操作

    virtual GRGlobalParameter::GRItemDataType getItemType() const;
    virtual QRectF getItemRect() const;                 //返回所占的矩形空间

    virtual bool isCascadePage() const;                 //新增 是否级联到其他页面
    virtual QString getCascadePageName() const;         //新增 级联到的页面名
    virtual float getOpacity() const;
    virtual void drawUserText(int argc, QPainter *painter, const QString &text, const QString &color);

    QString getReportContent() const
    {
        return m_reportContent;
    }

    QString getTextContent() const
    {
        return TextContent;
    }

    void setTextContent(const QString &content)
    {
        TextContent = content;
    }

    QFont getTextItemFont() const   //获取字体
    {
        QFont font(FontFamily, fontsize);
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

        return font;
    }
    float getItemPosX() const
    {
        return this->f_x;
    }
    float getItemPosY() const
    {
        return this->f_y;
    }

private:
    QString FontFamily;         //字体
    QString TextContent;        //文本内容
    int f_Rotation;             //旋转角度
    QString m_Stroke;           //线条颜色
    int f_StrokeThickness;      //线宽
    float fontsize;             //字体大小
    QString FontFamilyStyle;    //文本字体格式
    QString Alignment;          //文本水平对其方式
    QString m_reportContent;    //报表值 2012-8-29添加
};

#endif // GRTEXTITEMDATA_H
