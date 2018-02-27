#include "fillstyle.h"
#include <QtCore/qmath.h>
//#include <QtMath>

FillStyle::FillStyle()
{
}

FillStyle::FillStyle(const QDomElement &element)
{
    QDomElement thisxml=element.firstChild().toElement();
    if(thisxml.toElement().tagName()=="LinearGradientBrush")
    {
        m_StrBrush="LinearGradientBrush";
        if(thisxml.toElement().attribute("X1")=="0%")
        {
             n_X1=2;
        }
        else
        {
            n_X1=thisxml.toElement().attribute("X1").toInt();
        }
        n_Y1=thisxml.toElement().attribute("Y1").toInt();
        n_X2=thisxml.toElement().attribute("X2").toInt();
        n_Y2=thisxml.toElement().attribute("Y2").toInt();
        int index=1;
        QDomNode Linerchild = thisxml.firstChild();
        while(!Linerchild.isNull())
        {
            if(Linerchild.toElement().tagName()=="Stop")
            {
                if(index==1)
                {
                    f_Offset1=Linerchild.toElement().attribute("Offset").toFloat();
                    m_OffsetColor1=Linerchild.toElement().attribute("Color");
                }
                if(index==2)
                {
                    f_Offset2=Linerchild.toElement().attribute("Offset").toFloat();
                    m_OffsetColor2=Linerchild.toElement().attribute("Color");
                }
                if(index==3)
                {
                    f_Offset3=Linerchild.toElement().attribute("Offset").toFloat();
                    m_OffsetColor3=Linerchild.toElement().attribute("Color");
                }
            }
            Linerchild = Linerchild.nextSibling();
            index++;
        }
    }
    if(thisxml.toElement().tagName()=="RadialGradientBrush")
    {
        m_StrBrush="RadialGradientBrush";
        n_R=thisxml.toElement().attribute("r").toInt();
        n_Cy=thisxml.toElement().attribute("cy").toInt();
        n_Cx=thisxml.toElement().attribute("cx").toInt();
        int index=1;
        QDomNode Linerchild = thisxml.firstChild();
        while(!Linerchild.isNull())
        {
            if(Linerchild.toElement().tagName()=="Stop")
            {
                if(index==1)
                {
                    f_Offset1=Linerchild.toElement().attribute("Offset").toFloat();
                    m_OffsetColor1=Linerchild.toElement().attribute("Color");
                }
                if(index==2)
                {
                    f_Offset2=Linerchild.toElement().attribute("Offset").toFloat();
                    m_OffsetColor2=Linerchild.toElement().attribute("Color");
                }
                if(index==3)
                {
                    f_Offset3=Linerchild.toElement().attribute("Offset").toFloat();
                    m_OffsetColor3=Linerchild.toElement().attribute("Color");
                }
            }
            Linerchild = Linerchild.nextSibling();
            index++;
        }
    }
}
void FillStyle::SetData( int Height, int Width, qreal MaxXPoint,qreal MixXPoint,
                          qreal MaxYPoint,qreal MixYPoint )
{
    this->f_Height=Height;
    this->f_Width=Width;
    this->q_MaxXPoint=MaxXPoint;
    this->q_MaxYPoint=MaxYPoint;
    this->q_MixXPoint=MixXPoint;
    this->q_MixYPoint=MixYPoint;
}
QLinearGradient *FillStyle::LinerBrush()
{
    if(n_X1==0&&n_Y1==1&&n_X2==0&&n_Y2==0&&m_OffsetColor3=="")//底部到顶端
    {
        QLinearGradient *linearGradient = new QLinearGradient(0,q_MaxYPoint,0,q_MixYPoint);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
        return linearGradient;
    }
    if(n_X1==1&&n_Y1==0&&n_X2==0&&n_Y2==0&&m_OffsetColor3=="")//母线带电用
    {
        QLinearGradient *linearGradient=new QLinearGradient(0,q_MixYPoint,0,q_MaxYPoint);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==0&&n_X2==0&&n_Y2==1&&m_OffsetColor3=="")//顶端到底部
    {
        QLinearGradient *linearGradient=new QLinearGradient(0,q_MixYPoint,0,q_MaxYPoint);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==0&&n_X2==1&&n_Y2==0)//两边倒中间
    {
        QLinearGradient *linearGradient=new QLinearGradient(q_MixXPoint,0,q_MaxXPoint,0);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
        linearGradient->setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    if(n_X1==2&&n_Y1==0&&n_X2==0&&n_Y2==1&&m_OffsetColor3!="")//上到中间到下
    {
        QLinearGradient *linearGradient=new QLinearGradient(0,q_MixYPoint,0,q_MaxYPoint);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
        linearGradient->setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    if(n_X1==1&&n_Y1==0&&n_X2==0&&n_Y2==1)//从左到右
    {
        QLinearGradient *linearGradient=new QLinearGradient(q_MixXPoint,0,q_MaxXPoint,0);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
        return linearGradient;
    }
    if(n_X1==1&&n_Y1==1&&n_X2==0&&n_Y2==0)//从右上到左下
    {
        QLinearGradient *linearGradient=new QLinearGradient(q_MaxXPoint,q_MaxYPoint,q_MixXPoint,q_MixYPoint);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==0&&n_X2==0&&n_Y2==1&&m_OffsetColor3!="")//中到上到下
    {
        QLinearGradient *linearGradient=new QLinearGradient(0,q_MixYPoint,0,(q_MaxYPoint-q_MixYPoint)/2);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
        linearGradient->setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==0&&n_X2==1&&n_Y2==1&&m_OffsetColor3=="")//中到左上到右下
    {
        QLinearGradient *linearGradient=new QLinearGradient(q_MixXPoint,q_MixYPoint,q_MaxXPoint,q_MaxYPoint);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
        return linearGradient;
    }
    if(n_X1==2&&n_Y1==1&&n_X2==1&&n_Y2==0)//中到左下到右上
    {
        QLinearGradient *linearGradient=new QLinearGradient(q_MixXPoint,q_MaxYPoint,q_MaxXPoint,q_MixYPoint);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
        linearGradient->setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==0&&n_X2==1&&n_Y2==1&&m_OffsetColor3!="")//左上到中到右下
    {
        QLinearGradient *linearGradient=new QLinearGradient(q_MixXPoint,q_MixYPoint,q_MaxXPoint,q_MaxYPoint);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
         linearGradient->setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==1&&n_X2==1&&n_Y2==0&&m_OffsetColor3=="")//中到上到下
    {
        QLinearGradient *linearGradient=new QLinearGradient(q_MixXPoint,q_MaxYPoint,q_MaxXPoint,q_MixYPoint);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
        return linearGradient;
    }
    if(n_X1==2&&n_Y1==2&&n_X2==2&&n_Y2==2)//2014.8.6修改，从上到中再到下，添加此渐变
    {
        QLinearGradient *linearGradient=new QLinearGradient(0,q_MixYPoint,0,q_MaxYPoint);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
        linearGradient->setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==1&&n_X2==1&&n_Y2==0&&m_OffsetColor3!="")//中到上到下
    {
        QLinearGradient *linearGradient=new QLinearGradient(q_MixXPoint,q_MaxYPoint,q_MaxXPoint,q_MixYPoint);
        linearGradient->setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient->setColorAt(f_Offset2, m_OffsetColor2);
        linearGradient->setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    else
    {
        QLinearGradient *linearGradient=new QLinearGradient(0, 0, 100, 100);
        linearGradient->setColorAt(0.0, Qt::white);
        linearGradient->setColorAt(0.2, Qt::green);
        return linearGradient;
    }
}
QLinearGradient FillStyle::InitLinerBrush()
{
    if(n_X1==0&&n_Y1==1&&n_X2==0&&n_Y2==0&&m_OffsetColor3=="")//底部到顶端
    {
        QLinearGradient linearGradient(0,q_MaxYPoint,0,q_MixYPoint);
        linearGradient.setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient.setColorAt(f_Offset2, m_OffsetColor2);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==0&&n_X2==0&&n_Y2==1&&m_OffsetColor3=="")//顶端到底部
    {
        QLinearGradient linearGradient(0,q_MixYPoint,0,q_MaxYPoint);
        linearGradient.setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient.setColorAt(f_Offset2, m_OffsetColor2);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==0&&n_X2==1&&n_Y2==0)//两边倒中间
    {
        QLinearGradient linearGradient(q_MixXPoint,0,q_MaxXPoint,0);
        linearGradient.setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient.setColorAt(f_Offset2, m_OffsetColor2);
        linearGradient.setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    if(n_X1==2&&n_Y1==0&&n_X2==0&&n_Y2==1&&m_OffsetColor3!="")//上到中间到下
    {
        QLinearGradient linearGradient(0,q_MixYPoint,0,q_MaxYPoint);
        linearGradient.setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient.setColorAt(f_Offset2, m_OffsetColor2);
        linearGradient.setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    if(n_X1==1&&n_Y1==0&&n_X2==0&&n_Y2==1)//从左到右
    {
        QLinearGradient linearGradient(q_MixXPoint,0,q_MaxXPoint,0);
        linearGradient.setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient.setColorAt(f_Offset2, m_OffsetColor2);
        return linearGradient;
    }
    if(n_X1==1&&n_Y1==1&&n_X2==0&&n_Y2==0)//从右上到左下
    {
        QLinearGradient linearGradient(q_MaxXPoint,q_MaxYPoint,q_MixXPoint,q_MixYPoint);
        linearGradient.setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient.setColorAt(f_Offset2, m_OffsetColor2);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==0&&n_X2==0&&n_Y2==1&&m_OffsetColor3!="")//中到上到下
    {
        QLinearGradient linearGradient(0,q_MixYPoint,0,(q_MaxYPoint-q_MixYPoint)/2);
        linearGradient.setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient.setColorAt(f_Offset2, m_OffsetColor2);
        linearGradient.setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==0&&n_X2==1&&n_Y2==1&&m_OffsetColor3=="")//中到左上到右下
    {
        QLinearGradient linearGradient(q_MixXPoint,q_MixYPoint,q_MaxXPoint,q_MaxYPoint);
        linearGradient.setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient.setColorAt(f_Offset2, m_OffsetColor2);
        return linearGradient;
    }
    if(n_X1==2&&n_Y1==1&&n_X2==1&&n_Y2==0)//中到左下到右上
    {
        QLinearGradient linearGradient(q_MixXPoint,q_MaxYPoint,q_MaxXPoint,q_MixYPoint);
        linearGradient.setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient.setColorAt(f_Offset2, m_OffsetColor2);
        linearGradient.setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==0&&n_X2==1&&n_Y2==1&&m_OffsetColor3!="")//左上到中到右下
    {
        QLinearGradient linearGradient(q_MixXPoint,q_MixYPoint,q_MaxXPoint,q_MaxYPoint);
        linearGradient.setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient.setColorAt(f_Offset2, m_OffsetColor2);
         linearGradient.setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==1&&n_X2==1&&n_Y2==0&&m_OffsetColor3=="")//中到上到下
    {
        QLinearGradient linearGradient(q_MixXPoint,q_MaxYPoint,q_MaxXPoint,q_MixYPoint);
        linearGradient.setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient.setColorAt(f_Offset2, m_OffsetColor2);
        return linearGradient;
    }
    if(n_X1==2&&n_Y1==2&&n_X2==2&&n_Y2==2)//2014.8.6修改，从上到中再到下，添加此渐变
    {
        QLinearGradient linearGradient(0,q_MixYPoint,0,q_MaxYPoint);
        linearGradient.setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient.setColorAt(f_Offset2, m_OffsetColor2);
        linearGradient.setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    if(n_X1==0&&n_Y1==1&&n_X2==1&&n_Y2==0&&m_OffsetColor3!="")//中到上到下
    {
        QLinearGradient linearGradient(q_MixXPoint,q_MaxYPoint,q_MaxXPoint,q_MixYPoint);
        linearGradient.setColorAt(f_Offset1, m_OffsetColor1);
        linearGradient.setColorAt(f_Offset2, m_OffsetColor2);
        linearGradient.setColorAt(f_Offset3, m_OffsetColor3);
        return linearGradient;
    }
    else
    {
        QLinearGradient linearGradient(0, 0, 100, 100);
        linearGradient.setColorAt(0.0, Qt::white);
        linearGradient.setColorAt(0.2, Qt::green);
        return linearGradient;
    }
}
QRadialGradient FillStyle::InitRadialBrush()
{
    if(n_Cx==1&&n_Cy==0&&n_R==1)//右下到左上3
    {
        qreal qfx=(q_MixXPoint+q_MaxXPoint)/2;
        qreal qfy=(q_MixYPoint+q_MaxYPoint)/2;
        qreal qfxy=(q_MaxXPoint-q_MixXPoint)*(q_MaxXPoint-q_MixXPoint)
                +(q_MaxYPoint-q_MixYPoint)*(q_MaxYPoint-q_MixYPoint);
        qreal qr=sqrt(qfxy);
        QRadialGradient radialGradient(qfx,qfy,qr/2,q_MaxXPoint,q_MixYPoint);

        radialGradient.setColorAt(f_Offset1, m_OffsetColor1);
        radialGradient.setColorAt(f_Offset2+0.17, m_OffsetColor2);

        return radialGradient;
    }
    if(n_Cx==1&&n_Cy==1&&n_R==1)//左上到右下4
    {
        qreal qfx=(q_MixXPoint+q_MaxXPoint)/2;
        qreal qfy=(q_MixYPoint+q_MaxYPoint)/2;
        qreal qfxy=(q_MaxXPoint-q_MixXPoint)*(q_MaxXPoint-q_MixXPoint)
                +(q_MaxYPoint-q_MixYPoint)*(q_MaxYPoint-q_MixYPoint);
        qreal qr=sqrt(qfxy);
        QRadialGradient radialGradient(qfx,qfy,qr/2,q_MaxXPoint,q_MaxYPoint);

        radialGradient.setColorAt(f_Offset1, m_OffsetColor1);
        radialGradient.setColorAt(f_Offset2+0.17, m_OffsetColor2);

        return radialGradient;
    }
    if(n_Cx==0&&n_Cy==0&&n_R==1)//右上到左下5
    {
        qreal qfx=(q_MixXPoint+q_MaxXPoint)/2;
        qreal qfy=(q_MixYPoint+q_MaxYPoint)/2;
        qreal qfxy=(q_MaxXPoint-q_MixXPoint)*(q_MaxXPoint-q_MixXPoint)
                +(q_MaxYPoint-q_MixYPoint)*(q_MaxYPoint-q_MixYPoint);
        qreal qr=sqrt(qfxy);
        QRadialGradient radialGradient(qfx,qfy,qr/2,q_MixXPoint,q_MixYPoint);

        radialGradient.setColorAt(f_Offset1, m_OffsetColor1);
        radialGradient.setColorAt(f_Offset2+0.17, m_OffsetColor2);

        return radialGradient;
    }
    if(n_Cx==0&&n_Cy==1&&n_R==1)//左下到右上6
    {
        qreal qfx=(q_MixXPoint+q_MaxXPoint)/2;
        qreal qfy=(q_MixYPoint+q_MaxYPoint)/2;
        qreal qfxy=(q_MaxXPoint-q_MixXPoint)*(q_MaxXPoint-q_MixXPoint)
                +(q_MaxYPoint-q_MixYPoint)*(q_MaxYPoint-q_MixYPoint);
        qreal qr=sqrt(qfxy);
        QRadialGradient radialGradient(qfx,qfy,qr/2,q_MixXPoint,q_MaxYPoint);

        radialGradient.setColorAt(f_Offset1, m_OffsetColor1);
        radialGradient.setColorAt(f_Offset2+0.17, m_OffsetColor2);

        return radialGradient;
    }
    if(n_Cx==1&&n_Cy==0&&n_R==1)//左下到右上
    {
        qreal qfx=(q_MixXPoint+q_MaxXPoint)/2;
        qreal qfy=(q_MixYPoint+q_MaxYPoint)/2;
        qreal qfxy=(q_MaxXPoint-q_MixXPoint)*(q_MaxXPoint-q_MixXPoint)
                +(q_MaxYPoint-q_MixYPoint)*(q_MaxYPoint-q_MixYPoint);
        qreal qr=sqrt(qfxy);
        QRadialGradient radialGradient(qfx,qfy,qr/2,q_MixXPoint,q_MaxYPoint);

        radialGradient.setColorAt(f_Offset1, m_OffsetColor1);
        radialGradient.setColorAt(f_Offset2, m_OffsetColor2);

        return radialGradient;
    }
    else//圆心到中间，中间到圆心
    {
        qreal qfx=(q_MixXPoint+q_MaxXPoint)/2;
        qreal qfy=(q_MixYPoint+q_MaxYPoint)/2;
        qreal qfxy=(q_MaxXPoint-q_MixXPoint)*(q_MaxXPoint-q_MixXPoint)
                +(q_MaxYPoint-q_MixYPoint)*(q_MaxYPoint-q_MixYPoint);
        qreal qr=sqrt(qfxy);
        QRadialGradient radialGradient(qfx,qfy,qr/2,qfx,qfy);

        radialGradient.setColorAt(f_Offset1, m_OffsetColor1);
        radialGradient.setColorAt(f_Offset2, m_OffsetColor2);

        return radialGradient;
    }

}
