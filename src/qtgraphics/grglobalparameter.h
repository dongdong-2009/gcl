
#ifndef GRGLOBALPARAMETER_H
#define GRGLOBALPARAMETER_H
#include "graphics_global.h"

class GCLGRAPHICSSHARED_EXPORT GRGlobalParameter
{
public:
    GRGlobalParameter();

public:
    enum GRItemDataType
    {
        GR_DeviceItemType,      //设备
        GR_ConnectLineType,     //连接线
        GR_ConnectPointType,    //连接点
        GR_RectangleType,       //矩形
        GR_ArcType,             //圆弧
        GR_BezierType,          //贝塞尔曲线
        GR_EllipseType,         //椭圆
        GR_LineType,            //直线
        GR_PolylineType,        //折线
        GR_PolygonType,         //多边形
        GR_TextType,            //文本
        GR_BusType,             //母线
        GR_CompositeFigureType, //组合符号
        GR_PictureType,         //图片，预留
        GR_CurvatureType        //弧线，预留
    };
};

#endif // GRGLOBALPARAMETER_H
