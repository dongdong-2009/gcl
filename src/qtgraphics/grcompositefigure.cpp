
#include "grcompositefigure.h"
#include "groupstate.h"
#include <QDomElement>

GRCompositeFigure::GRCompositeFigure() :
    f_Rotation(0),
    m_groupState(NULL)
{
}

GRCompositeFigure::~GRCompositeFigure()
{
    if(m_groupState != NULL)
    {
        delete m_groupState;
        m_groupState = NULL;
    }
}

void GRCompositeFigure::loadItemData(const QDomElement &element)   //读取数据
{
    this->f_height = element.attribute("Height").toFloat();
    this->f_width = element.attribute("Width").toFloat();
    this->f_y = element.attribute("Y").toFloat();
    this->f_x = element.attribute("X").toFloat();

    f_Rotation = element.attribute("Rotation").toFloat();
    this->m_cascadePageName = element.attribute("ConnectPageName");
    this->b_cascadePage = false;
    if(element.attribute("IsConnctToPage") == "True")
    {
        this->b_cascadePage = true;
    }

    QDomNode child = element.firstChild();
    while(!child.isNull())
    {
        if(child.toElement().tagName() == "State")
        {
            if(m_groupState != NULL)
            {
                delete m_groupState;
                m_groupState = NULL;
            }
            m_groupState = new GroupState();
            m_groupState->loadDataFromDom(child.toElement());
        }

        child = child.nextSibling();
    }
}

void GRCompositeFigure::drawItemData(QPainter *painter)        //画图元
{
    if(m_groupState != NULL)
    {
        if(this->b_commissioning)
        {
            m_groupState->setIsCommissioning(true);
        }
        else
        {
            m_groupState->setIsCommissioning(false);
        }
        m_groupState->drawGroupState(painter);
    }
}

void GRCompositeFigure::changDeviceState(int toState)    //改变设备状态。只有五防设备才操作
{
}

GRGlobalParameter::GRItemDataType GRCompositeFigure::getItemType() const
{
    return GRGlobalParameter::GR_CompositeFigureType;
}

QRectF GRCompositeFigure::getItemRect() const
{
    return QRectF(this->f_x, this->f_y, this->f_width, this->f_height);
}

bool GRCompositeFigure::isCascadePage() const                 //新增 是否级联到其他页面
{
    return this->b_cascadePage;
}

QString GRCompositeFigure::getCascadePageName() const         //新增 级联到的页面名
{
    return this->m_cascadePageName;
}

float GRCompositeFigure::getRotation() const
{
    return f_Rotation;
}

float GRCompositeFigure::getOpacity() const
{
    return f_Opacity;
}
