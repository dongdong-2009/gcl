
#include "grdeviceitemdata.h"

GRDeviceItemData::GRDeviceItemData() :
    n_curState(0)
{
}

void GRDeviceItemData::loadItemData(const QDomElement &element)
{
    this->f_height = element.attribute("Height").toFloat();
    this->f_width = element.attribute("Width").toFloat();
    this->f_y = element.attribute("Y").toFloat();
    this->f_x = element.attribute("X").toFloat();

    f_LabelY = element.attribute("LableY").toFloat();
    f_LabelX = element.attribute("LableX").toFloat();
    f_Rotation = element.attribute("Rotation").toFloat();
    f_Opacity = element.attribute("Opacity").toFloat();
    n_LableVisible = element.attribute("LableVisible").toInt();
    n_Zindex = element.attribute("Zindex").toInt();

    m_voltageClass = element.attribute("VoltageClass");
    m_DevicesName = element.attribute("DevicesName");
    m_DeviceType = element.attribute("DeviceType");
    m_DevicesNumber = element.attribute("DevicesNumber");
    m_ID = element.attribute("ID");
    this->m_cascadePageName = element.attribute("ConnectPageName");
    this->b_cascadePage = false;
    if(element.attribute("IsConnctToPage") == "True")
    {
        this->b_cascadePage = true;
    }

    QDomNode childNode = element.firstChild();
    while(!childNode.isNull())
    {
        QDomElement childElement = childNode.toElement();
        if(childElement.tagName() == "Text")
        {
            m_textFillColor = childElement.attribute("Fill");
            n_textFontSize = childElement.attribute("FontSize").toInt();
            m_textFontFamily = childElement.attribute("FontFamily");
        }
        else if(childElement.tagName() == "State")
        {
            GroupState *state = new GroupState();
            state->loadDataFromDom(childElement);

            m_groupVect.append(state);
        }

        childNode = childNode.nextSibling();
    }
}

void GRDeviceItemData::drawItemData(QPainter *painter)
{
    if(m_groupVect.count() == 0)
    {
        return;
    }

    foreach(GroupState *stateGroup, m_groupVect)
    {
        if(stateGroup->getGroupState() == n_curState)
        {
            if(this->b_commissioning)
            {
                stateGroup->setIsCommissioning(true);
            }
            else
            {
                stateGroup->setIsCommissioning(false);
            }
            stateGroup->drawGroupState(painter);
            break;
        }
    }
//    if(m_equipType != NULL)
//    {
//        m_equipType->drawEquipment(painter, n_curState);
//        qDebug() << "Draw device!!";
//    }
//    else
//    {
//        qDebug() << "NOT --- Draw device!!";
//    }
}

void GRDeviceItemData::changDeviceState(int toState)
{
    n_curState = toState;
}

GRGlobalParameter::GRItemDataType GRDeviceItemData::getItemType() const
{
    return GRGlobalParameter::GR_DeviceItemType;
}

QRectF GRDeviceItemData::getItemRect() const
{
    return QRectF(this->f_x,this->f_y,this->f_width,this->f_height);
}

bool GRDeviceItemData::isCascadePage() const                 //新增 是否级联到其他页面
{
    return this->b_cascadePage;
}

QString GRDeviceItemData::getCascadePageName() const         //新增 级联到的页面名
{
    return this->m_cascadePageName;
}

int GRDeviceItemData::getZIndex() const
{
    return n_Zindex;
}

float GRDeviceItemData::getRotation() const
{
    return f_Rotation;
}

float GRDeviceItemData::getOpacity() const
{
    return f_Opacity;
}

void GRDeviceItemData::drawUserText(int argc, QPainter *painter, const QString &text, const QString &color)
{
    if(m_groupVect.count() == 0)
    {
        return;
    }

    foreach(GroupState *stateGroup, m_groupVect)
    {
        stateGroup->drawUserText(argc, painter, text, color);
    }
}

void GRDeviceItemData::drawVarialRectangle(QPainter *painter, float value)
{
    if(m_groupVect.count() == 0)
    {
        return;
    }

    foreach(GroupState *stateGroup, m_groupVect)
    {
        stateGroup->drawVarialRectangle(painter, value);
    }
}
