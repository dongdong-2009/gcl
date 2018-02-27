#include "praui.h"

using namespace std;

//*pra--------------------------------------

void PraUi::praHeadInit(QTableWidget * oTableWidget)
{
    /*
    std::string md5;
    std::string fileName;
    std::string title;
    std::string moduleName;
    std::string desc;
    msepoch_t   modifyTime;
    std::string modifyContent;
    std::string modifier;
    */
    //量的相关数据
    QTableWidget * oGrid = oTableWidget;
    //首次刷新UI数据
    oGrid->clear();
    QStringList sHeaders;
    sHeaders.append(QString::fromUtf8("区域名称"));
    sHeaders.append(QString::fromUtf8("相对目录"));
    sHeaders.append(QString::fromUtf8("远程IP"));
    sHeaders.append(QString::fromUtf8("远程端口"));
    sHeaders.append(QString::fromUtf8("本地IP"));
    sHeaders.append(QString::fromUtf8("本地端口"));
    sHeaders.append(QString::fromUtf8("标识"));
    sHeaders.append(QString::fromUtf8("描述"));
    sHeaders.append(QString::fromUtf8("备注"));
    sHeaders.append(QString::fromUtf8("保留1"));
    int iColumnCount = sHeaders.count();
    oGrid->setColumnCount(iColumnCount);
    oGrid->setHorizontalHeaderLabels(sHeaders);
    oGrid->setColumnWidth(0, 100);
    oGrid->setColumnWidth(1, 100);
}

void PraUi::praFill(QTableWidget * oTableWidget, int iRowIndex, const Pra & pra)
{
    QTableWidget * oGrid = oTableWidget;
    QTableWidgetItem* oItem;
    //产品名称
    oItem = oGrid->item(iRowIndex, 0);
    QtCommonWidget::setItemValue(oItem, pra.name);
    //相对目录
    oItem = oGrid->item(iRowIndex, 1);
    QtCommonWidget::setItemValue(oItem, pra.rpath);
    //模块名
    oItem = oGrid->item(iRowIndex, 2);
    QtCommonWidget::setItemValue(oItem, pra.ip_r);
    //模块名
    oItem = oGrid->item(iRowIndex, 3);
    QtCommonWidget::setItemValue(oItem, pra.port_r);
    //修改人
    oItem = oGrid->item(iRowIndex, 4);
    QtCommonWidget::setItemValue(oItem, pra.ip_l);
    //修改时间
    oItem = oGrid->item(iRowIndex, 5);
    QtCommonWidget::setItemValue(oItem, pra.port_l);
    //标识
    oItem = oGrid->item(iRowIndex, 6);
    QtCommonWidget::setItemValue(oItem, pra.title);
    //描述
    oItem = oGrid->item(iRowIndex, 7);
    QtCommonWidget::setItemValue(oItem, pra.desc);
    //修改内容
    oItem = oGrid->item(iRowIndex, 8);
    QtCommonWidget::setItemValue(oItem, pra.remark);
    //配置命令
    oItem = oGrid->item(iRowIndex, 9);
    QtCommonWidget::setItemValue(oItem, pra.res1);
}

void PraUi::praInsert(QTableWidget * oTableWidget, const Pra & pra)
{
    int iRowIndex = 0;
    QTableWidget * oGrid = oTableWidget;
    oGrid->insertRow(iRowIndex);
    QTableWidgetItem* oItem;
    //产品名称
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 0, oItem);
    QtCommonWidget::setItemValue(oItem, pra.name);
    //相对目录
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 1, oItem);
    QtCommonWidget::setItemValue(oItem, pra.rpath);
    //模块名
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 2, oItem);
    QtCommonWidget::setItemValue(oItem, pra.ip_r);
    //模块名
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 3, oItem);
    QtCommonWidget::setItemValue(oItem, pra.port_r);
    //修改人
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 4, oItem);
    QtCommonWidget::setItemValue(oItem, pra.ip_l);
    //修改时间
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 5, oItem);
    QtCommonWidget::setItemValue(oItem, pra.port_l);
    //标识
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 6, oItem);
    QtCommonWidget::setItemValue(oItem, pra.title);
    //描述
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 7, oItem);
    QtCommonWidget::setItemValue(oItem, pra.desc);
    //修改内容
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 8, oItem);
    QtCommonWidget::setItemValue(oItem, pra.remark);
    //配置命令
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 9, oItem);
    QtCommonWidget::setItemValue(oItem, pra.res1);
}





//*praitem--------------------------------------
void PraUi::praItemHeadInit(QTableWidget * oTableWidget)
{
    /*
    std::string path;
    std::string pathType;
    std::string version;
    */
    //量的相关数据
    QTableWidget * oGrid = oTableWidget;
    //首次刷新UI数据
    oGrid->clear();
    QStringList sHeaders;
    sHeaders.append(QString::fromUtf8("产品名称"));
    sHeaders.append(QString::fromUtf8("相对目录"));
    sHeaders.append(QString::fromUtf8("描述"));
    int iColumnCount = sHeaders.count();
    oGrid->setColumnCount(iColumnCount);
    oGrid->setHorizontalHeaderLabels(sHeaders);
    oGrid->setColumnWidth(0, 100);
    oGrid->setColumnWidth(1, 100);
    oGrid->setColumnWidth(2, 100);
}

void PraUi::praItemInsert(QTableWidget * oTableWidget, const PraItem & praItem)
{
    int iRowIndex = 0;
    QTableWidget * oGrid = oTableWidget;
    oGrid->insertRow(iRowIndex);
    QTableWidgetItem* oItem;
    //相对目录
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 0, oItem);
    QtCommonWidget::setItemValue(oItem, praItem.fdc);
    //相对目录
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 1, oItem);
    QtCommonWidget::setItemValue(oItem, praItem.res1);
    //
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 2, oItem);
    QtCommonWidget::setItemValue(oItem, praItem.desc);
}

void PraUi::prasRefresh(QTableWidget * oTableWidget, const std::vector<Pra> & pras)
{
    /*
    std::string md5;
    std::string fileName;
    std::string title;
    std::string moduleName;
    std::string desc;
    msepoch_t   modifyTime;
    std::string modifyContent;
    std::string modifier;
    */
    //量的相关数据
    int iRowCount = pras.size();
    QTableWidget * oGrid = oTableWidget;
    int iRowIndex = 0;

    //首次刷新UI数据
    if (oGrid->rowCount() != iRowCount)
    {
        oGrid->setRowCount(0);
        for (size_t i = 0; i < pras.size(); ++i)
        {
            const Pra & pra = pras.at(i);

            PraUi::praInsert(oGrid, pra);
        }
    }
    else
    {
        if (pras.empty()) return;
        for (size_t i = pras.size()-1; i >= 0; --i)
        {
            const Pra & pra = pras.at(i);

            PraUi::praFill(oGrid, iRowIndex, pra);

            iRowIndex++;
        }
    }
}


void PraUi::fdcItemHeadInit(QTableWidget * oTableWidget)
{
    /*
    std::string path;
    std::string pathType;
    std::string version;
    */
    //量的相关数据
    QTableWidget * oGrid = oTableWidget;
    //首次刷新UI数据
    oGrid->clear();
    QStringList sHeaders;
    sHeaders.append(QString::fromUtf8("相对路径"));
    sHeaders.append(QString::fromUtf8("本地md5"));
    sHeaders.append(QString::fromUtf8("远程md5"));
    sHeaders.append(QString::fromUtf8("操作状态"));
    sHeaders.append(QString::fromUtf8("路径类型"));
    sHeaders.append(QString::fromUtf8("版本"));
    int iColumnCount = sHeaders.count();
    oGrid->setColumnCount(iColumnCount);
    oGrid->setHorizontalHeaderLabels(sHeaders);
    oGrid->setColumnWidth(0, 100);
    oGrid->setColumnWidth(1, 100);
    oGrid->setColumnWidth(2, 100);
}

void PraUi::fdcItemAdd(QTableWidget *oTableWidget, const FdcItem &fdcItem, const string &sMd5_r, const string &sMd5_l, const string &sState)
{
    QTableWidget * oGrid = oTableWidget;
    int iRowIndex = oGrid->rowCount();
    oGrid->setRowCount(iRowIndex+1);
    fdcItemCreate(oGrid, iRowIndex, fdcItem, sMd5_r, sMd5_l, sState);
}

void PraUi::fdcItemFill(QTableWidget * oTableWidget, const std::string &sRfilePath, const std::string &sMd5_r, const std::string &sMd5_l, const std::string &sState)
{
    QTableWidget * oGrid = oTableWidget;
    QTableWidgetItem * oItem;
    for (int i = 0; i < oGrid->rowCount(); ++i)
    {
        oItem = oGrid->item(i, 0);
        if (oItem)
        {
            string s = QtCommonWidget::getItemValue(oItem, string());
            if (sRfilePath == s)
            {
                if (sMd5_l.size()>0)
                {
                    oItem = oGrid->item(i, 1);
                    QtCommonWidget::setItemValue(oItem, sMd5_l);
                    oGrid->setCurrentCell(i, 1);
                }
                if (sMd5_r.size()>0)
                {
                    oItem = oGrid->item(i, 2);
                    QtCommonWidget::setItemValue(oItem, sMd5_r);
                    oGrid->setCurrentCell(i, 2);
                }
                if (sState.size()>0)
                {
                    oItem = oGrid->item(i, 3);
                    QtCommonWidget::setItemValue(oItem, sState);
                    oGrid->setCurrentCell(i, 3);
                }
                oGrid->update();
                break;
            }
        }
    }
}

void PraUi::fdcItemClearMd5_r(QTableWidget *oTableWidget)
{
    QTableWidget * oGrid = oTableWidget;
    QTableWidgetItem * oItem;
    for (int i = 0; i < oGrid->rowCount(); ++i)
    {
        oItem = oGrid->item(i, 2);
        QtCommonWidget::setItemValue(oItem, string());
    }
}

void PraUi::fdcItemClearState(QTableWidget *oTableWidget)
{
    QTableWidget * oGrid = oTableWidget;
    QTableWidgetItem * oItem;
    for (int i = 0; i < oGrid->rowCount(); ++i)
    {
        oItem = oGrid->item(i, 3);
        QtCommonWidget::setItemValue(oItem, string());
    }
}

void PraUi::fdcItemInsert(QTableWidget * oTableWidget, const FdcItem & fdcItem, const std::string & sMd5_r, const std::string & sMd5_l, const std::string &sState)
{
    int iRowIndex = 0;
    QTableWidget * oGrid = oTableWidget;
    oGrid->insertRow(iRowIndex);
    fdcItemCreate(oGrid, iRowIndex, fdcItem, sMd5_r, sMd5_l, sState);
}

void PraUi::fdcItemCreate(QTableWidget *oTableWidget, int iRowIndex, const FdcItem &fdcItem, const string &sMd5_r, const string &sMd5_l, const string &sState)
{
    QTableWidget * oGrid = oTableWidget;
    QTableWidgetItem* oItem;
    //相对目录
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 0, oItem);
    QtCommonWidget::setItemValue(oItem, fdcItem.rfilePath);
    //
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 1, oItem);
    QtCommonWidget::setItemValue(oItem, sMd5_l);
    //
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 2, oItem);
    QtCommonWidget::setItemValue(oItem, sMd5_r);
    //
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 3, oItem);
    QtCommonWidget::setItemValue(oItem, sState);
    //
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 4, oItem);
    QtCommonWidget::setItemValue(oItem, fdcItem.beupdate);
    //
    oItem = new QTableWidgetItem();
    oGrid->setItem(iRowIndex, 5, oItem);
    QtCommonWidget::setItemValue(oItem, fdcItem.version);
}


