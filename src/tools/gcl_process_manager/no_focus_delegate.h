#ifndef NO_FOCUS_DELEGATE_H
#define NO_FOCUS_DELEGATE_H

#include <QStyledItemDelegate>

class NoFocusDelegate : public QStyledItemDelegate   //消除选中时的虚框
{
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


void NoFocusDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem itemOption(option);
        if (itemOption.state & QStyle::State_HasFocus)
            itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
        QStyledItemDelegate::paint(painter, itemOption, index);
}
#endif // NO_FOCUS_DELEGATE_H
