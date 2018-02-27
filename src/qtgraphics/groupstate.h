
#ifndef GROUPSTATE_H
#define GROUPSTATE_H
#include <QVector>
#include <QPainter>
#include <QDomElement>

class GRItemDataInterface;
class GroupState
{
public:
    GroupState();
    ~GroupState();

public:
    int getGroupState() const
    {
        return n_groupState;
    }
    void setIsCommissioning(bool enable)
    {
        b_commissioning = enable;
    }

public:
    void loadDataFromDom(const QDomElement &element);
    void drawGroupState(QPainter *painter);
    void drawUserText(int argc, QPainter *painter, const QString &text, const QString &color);
    void drawVarialRectangle(QPainter *painter, float value);

private:
    int n_groupState;                   //组合的状态
    bool b_commissioning;               //是否投运
    QVector<GRItemDataInterface *> m_itemVec;
};

#endif // GROUPSTATE_H
