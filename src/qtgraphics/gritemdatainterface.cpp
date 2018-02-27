

#include "gritemdatainterface.h"
#include <QPainter>

GRItemDataInterface::GRItemDataInterface() :
    f_height(0.0),
    f_width(0.0),
    f_x(0.0),
    f_y(0.0),
    n_pageIndex(0),
    b_cascadePage(false),
    b_commissioning(true)
{   
}

GRItemDataInterface::~GRItemDataInterface()
{
}

void GRItemDataInterface::setPageIndex(int index)
{
    n_pageIndex = index;
}

int GRItemDataInterface::getPageIndex() const
{
    return n_pageIndex;
}

void GRItemDataInterface::setCommissioning(bool enable)
{
    b_commissioning = enable;
}

void GRItemDataInterface::drawUserText(int argc, QPainter *painter, const QString &text, const QString &color)
{
}

void GRItemDataInterface::drawVarialRectangle(QPainter *painter, float value)
{
    drawItemData(painter);
}
