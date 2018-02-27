
#ifndef FILLSTYLE_H
#define FILLSTYLE_H
#include <QVector>
#include <QSet>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QDomElement>

class FillStyle
{
public:
    FillStyle();
    FillStyle(const QDomElement &element);

public:
    QLinearGradient InitLinerBrush();
    QRadialGradient InitRadialBrush();
    QLinearGradient *LinerBrush();
    void SetData( int Height, int Width, qreal MaxXPoint,qreal MixXPoint,
                  qreal MaxYPoint,qreal MixYPoint );
    QString m_StrBrush;//以下为填充
private:
    QVector<int> m_Vp;          //多边形坐标点
    int f_Height;            //所占矩形高度
    int f_Width;              //所占矩 形宽度
    qreal q_MaxXPoint;//最右边的x坐标
    qreal q_MixXPoint;//最左边的x坐标
    qreal q_MaxYPoint;//最下面的y坐标
    qreal q_MixYPoint;//最上面的y坐标

    int n_X1;
    int n_Y1;
    int n_X2;
    int n_Y2;
    int n_R;
    int n_Cy;
    int n_Cx;
    float f_Offset1;
    float f_Offset2;
    float f_Offset3;
    QString m_OffsetColor1;
    QString m_OffsetColor2;
    QString m_OffsetColor3;
};

#endif // FILLSTYLE_H
