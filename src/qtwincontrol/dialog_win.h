#ifndef DIALOG_WIN_H
#define DIALOG_WIN_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>

class DialogWin
{
public:
    template<class WidgetSon>
    static QDialog* createDialog(QWidget* oParent, const QString& sTitle, WidgetSon* oWidget) {
        //dialog
        QDialog* oDialog = new QDialog(oParent);
        oDialog->resize(800, 600);
        QVBoxLayout* oLayou0 = new QVBoxLayout(oDialog);
        QHBoxLayout* oLayou01 = new QHBoxLayout();
        oLayou0->addLayout(oLayou01);
        QHBoxLayout* oLayou02 = new QHBoxLayout();
        oLayou0->addLayout(oLayou02);
        //body
        oWidget = new WidgetSon(oDialog);
        QGroupBox* oBodyParent = new QGroupBox(oDialog);
        QHBoxLayout* oLayou011 = new QHBoxLayout(oBodyParent);
        oLayou011->addWidget(oWidget);
        oBodyParent->setTitle(sTitle);
        oLayou01->addWidget(oBodyParent);
        //button
        QSpacerItem* oSpacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        oLayou02->addItem(oSpacerItem);
        QPushButton* oCloseBn = new QPushButton(oDialog);
        oLayou02->addWidget(oCloseBn);
        oCloseBn->setText("close");
        oCloseBn->resize(80, 30);
        QObject::connect(oCloseBn, SIGNAL(clicked()), oDialog, SLOT(reject()));
        return oDialog;
    }

};

#endif // DIALOG_WIN_H
