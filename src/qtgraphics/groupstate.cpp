
#include "groupstate.h"
#include "gritemdatainterface.h"
#include "grarcitemdata.h"
#include "grbezieritemdata.h"
#include "grbussitemdata.h"
#include "grconnectlineitemdata.h"
#include "grconnectpointitemdata.h"
#include "grellipseitemdata.h"
#include "grlineitemdata.h"
#include "grpolylineitemdata.h"
#include "grpolygonitemdata.h"
#include "grrectangleitemdata.h"
#include "grtextitemdata.h"
#include "grcompositefigure.h"

GroupState::GroupState() :
    n_groupState(-1),
    b_commissioning(true)
{
}

GroupState::~GroupState()
{
    foreach(GRItemDataInterface *item, m_itemVec)
    {
        delete item;
    }
}

void GroupState::loadDataFromDom(const QDomElement &element)
{
    n_groupState = element.attribute("StateID").toInt();

    QDomNode child = element.firstChild();
    while(!child.isNull())
    {
        if(child.toElement().tagName() == "BaseElement")
        {
            QDomElement childElement = child.toElement();
            QString eleType = childElement.attribute("Type");
            if(eleType == "Rect")
            {
                GRItemDataInterface *item = new GRRectangleItemData();
                item->loadItemData(childElement);
                m_itemVec.append(item);
            }
            else if(eleType == "Arc")
            {
                GRItemDataInterface *item = new GRArcItemData();
                item->loadItemData(childElement);
                m_itemVec.append(item);
            }
            else if(eleType == "Bezier")
            {
                GRItemDataInterface *item = new GRBezierItemData();
                item->loadItemData(childElement);
                m_itemVec.append(item);
            }
            else if(eleType == "Ellipse")
            {
                GRItemDataInterface *item = new GREllipseItemData();
                item->loadItemData(childElement);
                m_itemVec.append(item);
            }
            else if(eleType == "Line")
            {
                GRItemDataInterface *item = new GRLineItemData();
                item->loadItemData(childElement);
                m_itemVec.append(item);
            }
            else if(eleType == "PolyLine")
            {
                GRItemDataInterface *item = new GRPolyLineItemData();
                item->loadItemData(childElement);
                m_itemVec.append(item);
            }
            else if(eleType == "Polygon")
            {
                GRItemDataInterface *item = new GRPolygonItemData();
                item->loadItemData(childElement);
                m_itemVec.append(item);
            }
            else if(eleType == "Text")
            {
                GRItemDataInterface *item = new GRTextItemData();
                item->loadItemData(childElement);
                m_itemVec.append(item);
            }
            else if(eleType == "CompositeFigure")
            {
                GRItemDataInterface *item = new GRCompositeFigure();
                item->loadItemData(childElement);
                m_itemVec.append(item);
            }
        }

        child = child.nextSibling();
    }
}

void GroupState::drawGroupState(QPainter *painter)
{
    foreach(GRItemDataInterface *item, m_itemVec)
    {
        if(b_commissioning)
        {
            item->setCommissioning(true);
        }
        else
        {
            item->setCommissioning(false);
        }
        item->drawItemData(painter);
    }
}

void GroupState::drawUserText(int argc, QPainter *painter, const QString &text, const QString &color)
{
    foreach(GRItemDataInterface *item, m_itemVec)
    {
        if(b_commissioning)
        {
            item->setCommissioning(true);
        }
        else
        {
            item->setCommissioning(false);
        }
        item->drawUserText(argc, painter, text, color);
    }
}

void GroupState::drawVarialRectangle(QPainter *painter, float value)
{
    foreach(GRItemDataInterface *item, m_itemVec)
    {
        if(b_commissioning)
        {
            item->setCommissioning(true);
        }
        else
        {
            item->setCommissioning(false);
        }
        item->drawVarialRectangle(painter, value);
    }
}
