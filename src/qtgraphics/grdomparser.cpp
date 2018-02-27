
#include "grdomparser.h"
#include "grpage.h"
#include "gritemdatainterface.h"
#include "grdeviceitemdata.h"
#include "grconnectlineitemdata.h"
#include "grconnectpointitemdata.h"
#include "grbussitemdata.h"
#include "grrectangleitemdata.h"
#include "grarcitemdata.h"
#include "grbezieritemdata.h"
#include "grellipseitemdata.h"
#include "grlineitemdata.h"
#include "grpolylineitemdata.h"
#include "grpolygonitemdata.h"
#include "grtextitemdata.h"
#include "grcompositefigure.h"
#include <QDebug>
#include <QDomElement>
#include <QFile>
#include <QDomDocument>

GRDomParser::GRDomParser()
{
}

bool GRDomParser::readFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if(!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn))
    {
        return false;
    }

    QDomElement root = doc.documentElement();
    if(root.tagName() != "Pages")
    {
        return false;
    }

    this->parsePageslement(root);

    return true;
}

QVector<GRPage *> GRDomParser::getPageVector() const
{
    return m_pageVect;
}

QVector<GRItemDataInterface *> GRDomParser::getElementVector() const
{
    return m_itemDataVect;
}

void GRDomParser::parsePageslement(const QDomElement &element)
{
    QDomNode child = element.firstChild();
    int pageIndex = 0;
    while(!child.isNull())
    {
        if(child.toElement().tagName() == "Page")
        {
            this->parseSinglePageElement(child.toElement(), pageIndex++);
        }

        child = child.nextSibling();
    }
}

void GRDomParser::parseSinglePageElement(const QDomElement &element, int pageIndex)
{
    QString subName = element.attribute("SubstationName");
    QString pageName = element.attribute("PageName");
    QString fillColor = element.attribute("Fill");
    QString heiStr = element.attribute("Height");
    QString widStr = element.attribute("Width");
    int height = heiStr.toInt();
    int width = widStr.toInt();

    GRPage *subPage = new GRPage(subName, pageName, fillColor, height, width);
    m_pageVect.append(subPage);
    QDomNode child = element.firstChild();
    while(!child.isNull())
    {
/*        if(child.toElement().tagName() == "EquipmentTypes")
        {
            this->loadEquipmentTypes(child.toElement(), pageIndex);
        }
        else */
        if(child.toElement().tagName() == "Equipments")
        {
            this->loadEquipments(child.toElement(), pageIndex);
        }
        else if(child.toElement().tagName() == "ConnectLine")
        {
            this->loadConnectLine(child.toElement(), pageIndex);
        }
        else if(child.toElement().tagName() == "ConnectPoints")
        {
            this->loadConnectPoints(child.toElement(), pageIndex);
        }
        else if(child.toElement().tagName() == "OtherGraphics")
        {
            this->loadOtherGraphics(child.toElement(), pageIndex);
        }
        else if(child.toElement().tagName() == "Buss")
        {
            this->loadBuss(child.toElement(), pageIndex);
        }

        child = child.nextSibling();
    }
}

//void MyDomParser::loadEquipmentTypes(const QDomElement &element)
//{
//    QDomNode child = element.firstChild();
//    while(!child.isNull())
//    {
//        if(child.toElement().tagName() == "EquipmentType")
//        {
//            this->loadDeviceType(child.toElement());
//        }

//        child = child.nextSibling();
//    }
//}

void GRDomParser::loadEquipments(const QDomElement &element, int pageIndex)
{
    QDomNode child = element.firstChild();
    while(!child.isNull())
    {
        if(child.toElement().tagName() == "Equipment")
        {
            GRItemDataInterface *item = new GRDeviceItemData();
            item->setPageIndex(pageIndex);
            item->loadItemData(child.toElement());

            m_itemDataVect.append(item);
        }

        child = child.nextSibling();
    }
}

void GRDomParser::loadConnectLine(const QDomElement &element, int pageIndex)
{
    QDomNode child = element.firstChild();
    while(!child.isNull())
    {
        if(child.toElement().tagName() == "CL")
        {
            GRItemDataInterface *item = new GRConnectLineItemData();
            item->setPageIndex(pageIndex);
            item->loadItemData(child.toElement());

            m_itemDataVect.append(item);
        }

        child = child.nextSibling();
    }
}

void GRDomParser::loadConnectPoints(const QDomElement &element, int pageIndex)
{
    QDomNode child = element.firstChild();
    while(!child.isNull())
    {
        if(child.toElement().tagName() == "CP")
        {
            GRItemDataInterface *item = new GRConnectPointItemData();
            item->setPageIndex(pageIndex);
            item->loadItemData(child.toElement());

            m_itemDataVect.append(item);
        }

        child = child.nextSibling();
    }
}

void GRDomParser::loadOtherGraphics(const QDomElement &element, int pageIndex)
{
    QDomNode child = element.firstChild();
    while(!child.isNull())
    {
        if(child.toElement().tagName() == "Rect")
        {
            GRItemDataInterface *item = new GRRectangleItemData();
            item->setPageIndex(pageIndex);
            item->loadItemData(child.toElement());

            m_itemDataVect.append(item);
        }
        else if(child.toElement().tagName() == "Arc")
        {
            GRItemDataInterface *item = new GRArcItemData();
            item->setPageIndex(pageIndex);
            item->loadItemData(child.toElement());

            m_itemDataVect.append(item);
        }
        else if(child.toElement().tagName() == "Bezier")
        {
            GRItemDataInterface *item = new GRBezierItemData();
            item->setPageIndex(pageIndex);
            item->loadItemData(child.toElement());

            m_itemDataVect.append(item);
        }
        else if(child.toElement().tagName() == "Ellipse")
        {
            GRItemDataInterface *item = new GREllipseItemData();
            item->setPageIndex(pageIndex);
            item->loadItemData(child.toElement());

            m_itemDataVect.append(item);
        }
        else if(child.toElement().tagName() == "Line")
        {
            GRItemDataInterface *item = new GRLineItemData();
            item->setPageIndex(pageIndex);
            item->loadItemData(child.toElement());

            m_itemDataVect.append(item);
        }
        else if(child.toElement().tagName() == "PolyLine")
        {
            GRItemDataInterface *item = new GRPolyLineItemData();
            item->setPageIndex(pageIndex);
            item->loadItemData(child.toElement());

            m_itemDataVect.append(item);
        }
        else if(child.toElement().tagName() == "Polygon")
        {
            GRItemDataInterface *item = new GRPolygonItemData();
            item->setPageIndex(pageIndex);
            item->loadItemData(child.toElement());

            m_itemDataVect.append(item);
        }
        else if(child.toElement().tagName() == "Text")
        {
            GRItemDataInterface *item = new GRTextItemData();
            item->setPageIndex(pageIndex);
            item->loadItemData(child.toElement());

            m_itemDataVect.append(item);
        }
        else if(child.toElement().tagName() == "CompositeFigure")
        {
            GRItemDataInterface *item = new GRCompositeFigure();
            item->setPageIndex(pageIndex);
            item->loadItemData(child.toElement());

            m_itemDataVect.append(item);
        }

        child = child.nextSibling();
    }
}

void GRDomParser::loadBuss(const QDomElement &element, int pageIndex)
{
    QDomNode child = element.firstChild();
    while(!child.isNull())
    {
        if(child.toElement().tagName() == "Bus")
        {
            GRItemDataInterface *item = new GRBussItemData();
            item->setPageIndex(pageIndex);
            item->loadItemData(child.toElement());

            m_itemDataVect.append(item);
        }

        child = child.nextSibling();
    }
}


void GRDomParser::loadDeviceType(const QDomElement &element)
{
}
