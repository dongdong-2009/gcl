
#include "grbussitemdata.h"
#include "grpolylineitemdata.h"
#include "fillstyle.h"
#include <QDomElement>

GRBussItemData::GRBussItemData() :
    m_polyLine(NULL),
    m_curGradient(NULL),
    m_linearGradient_0(NULL),
    m_linearGradient_1(NULL)
{
}

GRBussItemData::~GRBussItemData()
{
    if(m_polyLine != NULL)
    {
        delete m_polyLine;
    }
    if(m_linearGradient_0 != NULL)
    {
        delete m_linearGradient_0;
    }
    if(m_linearGradient_1 != NULL)
    {
        delete m_linearGradient_1;
    }
}

void GRBussItemData::loadItemData(const QDomElement &element)
{
    this->f_height = element.attribute("Height").toFloat();
    this->f_width = element.attribute("Width").toFloat();
    this->f_y = element.attribute("Y").toFloat();
    this->f_x = element.attribute("X").toFloat();

    m_deviceMainNumber = element.attribute("DevicesNumber");
    m_ID = element.attribute("ID");
    m_voltage = element.attribute("Voltage");

    b_IsElectrification = false;
    if(element.attribute("IsElectrification") == "True")
    {
        b_IsElectrification = true;
    }

    this->m_cascadePageName = element.attribute("ConnectPageName");
    this->b_cascadePage = false;
    if(element.attribute("IsConnctToPage") == "True")
    {
        this->b_cascadePage = true;
    }

    int stateCount = 0;
    QDomNode stateChild = element.firstChild();
    while(!stateChild.isNull())
    {
        QDomElement stateElement = stateChild.toElement();
        if(stateElement.tagName() == "State")
        {
            stateCount++;
            if(stateCount > 2)  //母线只有两个状态
            {
                break;
            }

            int state = stateElement.attribute("StateID").toInt();
            if(state == 0)
            {
                QDomElement baseElement = stateElement.firstChild().toElement();
                m_polyLine = new GRPolyLineItemData();
                m_polyLine->loadItemData(baseElement);

                QDomElement strokeElement = baseElement.firstChild().toElement();
                if(strokeElement.tagName() == "Stroke")
                {
                    this->loadGradient(strokeElement, 0);
                }
            }
            else
            {
                QDomElement baseElement = stateElement.firstChild().toElement();
                QDomElement strokeElement = baseElement.firstChild().toElement();
                if(strokeElement.tagName() == "Stroke")
                {
                    this->loadGradient(strokeElement, 1);
                }
            }
        }

        stateChild = stateChild.nextSibling();
    }

    m_curGradient = m_linearGradient_0;
}

void GRBussItemData::drawItemData(QPainter *painter)
{
    if(m_polyLine != NULL)
    {
//        m_polyLine->setRadient(m_curGradient);
        if(!this->b_commissioning)
        {
            m_polyLine->setCommissioning(false);
        }
        m_polyLine->drawItemData(painter);
    }
}

void GRBussItemData::changDeviceState(int toState)
{
    if(toState == 0)
    {
        m_curGradient = m_linearGradient_0;
    }
    else
    {
        m_curGradient = m_linearGradient_1;
    }
}

GRGlobalParameter::GRItemDataType GRBussItemData::getItemType() const
{
    return GRGlobalParameter::GR_BusType;
}

float GRBussItemData::getOpacity() const
{
    return 1;
}

QRectF GRBussItemData::getItemRect() const
{
    return QRectF(this->f_x,this->f_y,this->f_width,this->f_height);
}

bool GRBussItemData::isCascadePage() const                 //新增 是否级联到其他页面
{
    return this->b_cascadePage;
}

QString GRBussItemData::getCascadePageName() const         //新增 级联到的页面名
{
    return this->m_cascadePageName;
}

void GRBussItemData::loadGradient(const QDomElement &element, int index)
{
    FillStyle style(element);
    style.SetData(this->f_height, this->f_width, this->f_x + this->f_width,
                  this->f_x, this->f_y + this->f_height, this->f_y);

    if(index == 0)
    {
        m_linearGradient_0 = style.LinerBrush();
    }
    else
    {
        m_linearGradient_1 = style.LinerBrush();
    }
}
