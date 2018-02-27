#ifndef SETUP_WIN_H
#define SETUP_WIN_H

#include <QMainWindow>

#include <ccxx/ccxx.h>
#include <ccxx/qtcommon.h>

namespace Ui {
class SetupWin;
}

class UnitUi
{
public:
    QWidget * tab;
    QWidget * unitContainer;
    QWidget * unitAttrContainer;

};

class SetupWin : public QMainWindow ,public CxInterinfoOut_I
{
    Q_OBJECT

public:
    explicit SetupWin(QWidget *parent = 0);
    ~SetupWin();

protected:
    void interinfo_out( const std::string& sInfo, const std::string& sTitle, int type = 0, int reason = 0, int source = 0, int target = 0, int iTag = 0 );

    inline CxInterinfo::PlatformEnum platformValue() { return CxInterinfo::Platform_Desktop; }

private slots:
    void on_loadProjectBn_clicked();

    void on_listWidget_currentRowChanged(int currentRow);

    void on_saveProjectBn_clicked();

    void on_saveCollectionBn_clicked();

    void on_reloadDataNameBn_clicked();

    void on_saveDataNameBn_clicked();

    void on_redefaultDataNamBn_clicked();

private:
    void loadProject();

    void refreshProjectWin();

    void refreshCollectionsWin();

    void refreshCollectionWin(const QString & sCollectionTitle, QTabWidget * oTabWidget);

    void clearProjectWin();

    CxUnitAttr * getCurrentUnit(int & iRow, int & iTab, UnitUi & unitUi);

    void saveProject();

    void saveCollectionWin(const QString & sCollectionTitle, const std::vector<UnitUi> & unitUis);

    bool saveUnit(CxUnitAttr *oUnitAttr, UnitUi unitUi, const std::string &sProjectPath);

    UnitUi createUnitUi(QWidget * oTabConfig, QWidget * oTabData);

private:
    CxDescribeProject _describeProject;
    std::string _describePath;

private:
    Ui::SetupWin *ui;
    std::vector<std::vector<UnitUi> > collectionUis;

};

#endif // SETUP_WIN_H
