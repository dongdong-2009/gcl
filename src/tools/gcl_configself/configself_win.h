#ifndef SETUP_WIN_H
#define SETUP_WIN_H


#include <QMainWindow>


#include <ccxx/ccxx.h>
#include <ccxx/qtcommon.h>


#define ConfigselfProjectState_beforeLoad (1)
#define ConfigselfProjectState_afterLoad (2)
#define ConfigselfProjectState_beforeRefresh (3)
#define ConfigselfProjectState_afterRefresh (4)

namespace Ui {
class ConfigselfWin;
}

class UnitUi
{
public:
    std::string unitCaption;
    QWidget * tab;
    QWidget * unitContainer;
    QWidget * unitAttrContainer;

    UnitUi() :
        unitCaption(),
        tab(NULL),
        unitContainer(NULL),
        unitAttrContainer(NULL)
    {
    }

};


class ConfigselfPluginBase;
class ConfigselfPluginProxy;

//DescribePath：描述文件的路径（界面控件是用描述加载、创建、布局出来的，界面控件内容是数据文件加载出来的）
//DataPath：数据文件路径（界面控件是用描述加载、创建、布局出来的，界面控件内容是数据文件加载出来的）

//单个 describe 是一个配置单元
//多个 describe 的表是 vector<describe> 被包含在 unit
//unit 中还包含 自身属性 ( unitName , unitTitle ...... )
//多个 unit 的表是 vector<unit> 被包含在全局静态类 CxDescribeManager 中
//catalog 指 描述文件 与 数据文件 的关系 , 个体是 unitAttr
//project 包含多个 catalog

//attr 包括 oUnitAttr->dataType dataPath dataSection dataName dataParam 是否保存单元数据的属性（数据文件路径，数据参数……）

class ConfigselfWin : public QMainWindow , public CxInterinfoOut_I
{
    Q_OBJECT

public:
    explicit ConfigselfWin(QWidget *parent = 0);
    ~ConfigselfWin();

    void uiMemuClear();

    void uiToolBarClear();

    QMenu * uiProjectMenu() const { return _uiProjectMenu; }
    QMenu * uiCatalogMenu() const { return _uiCatalogMenu; }
    QMenu * uiUnitMenu   () const { return _uiUnitMenu;    }
    QMenu * uiOtherMenu    () const { return _uiOtherMenu;     }
    QMenu * uiHelpMenu     () const { return _uiHelpMenu;      }
    QToolBar * uiWinsToolBar      () const { return _uiWinsToolBar;      }
    QAction  * uiProjectLocadAct() const { return _uiProjectLocadAct;}
    QAction  * uiProjectSaveAct () const { return _uiProjectSaveAct; }
    QAction  * uiSaveCatalogAct () const { return _uiSaveCatalogAct; }
    QAction  * uiUnitSaveAct    () const { return _uiUnitSaveAct;    }


    inline const std::string & projectFilePath() { return _projectFilePath; }

    inline CxDescribeProject & describeProject() { return _describeProject; }

    //same to deployPath
    inline const std::string & dataPath() const { return _dataPath; }
    inline void setDataPath(const std::string & value) { _dataPath = value; }

    inline const std::string & describePath() const { return _describePath; }

    inline const std::map<std::string, std::vector<UnitUi> > & catalogUis() const { return _catalogUis; }

    //* ui -> row [catalog]
    inline int currentRow() const { return _currentRow; }

    inline const std::string & currentCatalogTitle() const { return _currentCatalogTitle; }

    inline CxCatalogAttr * currentCatalogAttr() const { return _currentCatalogAttr; }

    inline CxDescribeCatalog * currentCatalog() const { return _currentCatalogAttr ? _currentCatalogAttr->catalog : NULL; }

    const std::vector<CxUnitAttr*> & currentUnitAttrs() const;

    inline const std::vector<UnitUi> & currentUnitUis() const { return _currentUnitUis; }

    inline const std::string & currentUnitCaption() const { return _currentUnitCaption; }

    //* ui -> row -> tab [catalog->unit]
    inline CxUnitAttr * currentUnitAttr() const { return _currentUnitAttr; }

    inline CxDescribeUnit * currentUnit() const { return _currentUnitAttr ? _currentUnitAttr->unit : NULL; }

    inline std::string currentUnitName() const { return _currentUnitAttr && _currentUnitAttr->unit ? _currentUnitAttr->unit->unitName() : std::string(); }

    inline int currentTab() const { return _currentTab; }

    inline const UnitUi & currentUnitUi() const { return _currentUnitUi; }

    //*
    CxCatalogAttr *findCatalogAttr(const std::string &sCatalogTitle, int & iRow, std::vector<UnitUi> & unitUis);

    CxUnitAttr *findUnitAttr(const std::string &sCatalogTitle, const std::string &sUnitCaption, int &iTab, UnitUi &unitUi);

    QTabWidget * getTableWidgetLeft();

    QTabWidget * getTableWidgetRight();

    //*
    void loadProject();

    void loadProject(const std::string &sFilePath);

    void saveProject();

    void addCatalogWin(const std::string & sCatalogTitle);

    CxCatalogAttr *removeCatalogWin(const std::string & sCatalogTitle);

    //sCatalogTitle : if sCatalogTitle empty then sCatalogTitle=_currentCatalogTitle
    void reloadCatalogData(const std::string & sCatalogTitle = std::string(), const std::string &sDataName = std::string());

    //bUiViewIn: need save attr
    void saveCatalogData( bool bUiViewin = true, const std::string & sCatalogTitle = std::string());

    //save currentCatalog data
    inline void saveCurrentCatalogData( ) { saveCatalogData(false); }

    //save currentCatalog data and attr
    inline void saveCurrentCatalog( ) { saveCatalogData(true); }

    //save data and attr
    void saveCatalog(const std::string & sCatalogTitle = std::string());

    //save single unitData
    bool saveUnitData(CxUnitAttr *oUnitAttr, const UnitUi & unitUi, const std::string &sDataPath, bool bUiViewin = true);

    //has data
    bool loadUnitData(CxUnitAttr * oUnitAttr, const UnitUi &unitUi, const std::string &sDataFilePath);

    //if unit is table then clear table, if unit single object then viewOut from decribe content.
    bool initUnitData(CxUnitAttr * oUnitAttr, const UnitUi &unitUi, bool bToDefault);

    void saveCurrentUnit();

    void saveCurrentUnitData();

protected:
    void interinfo_out( const std::string& sInfo, const std::string& sTitle, int type = 0, int reason = 0, int source = 0, int target = 0, int iTag = 0 );

    inline CxInterinfo::PlatformEnum platformValue() { return CxInterinfo::Platform_Desktop; }

private slots:
    void on_saveAll_clicked();

    void on_about_clicked();

    void on_exit_clicked();

    void on_loadProjectBn_clicked();

    void on_listWidget_currentRowChanged(int currentRow);

    void on_tabWidget_currentChanged(int index);

    void on_saveProjectBn_clicked();

    void on_saveCatalogBn_clicked();

    void on_saveCatalogDataBn_clicked();

    void on_reloadDataNameBn_clicked();

    void on_saveDataNameBn_clicked();

    void on_redefaultDataNamBn_clicked();

private:
    void initCurrent();

    void loadPlugins();

    void createActions();

    void createMenus();

    void createMenus2();

    void createToolBars();

    //create ui
    void refreshProjectWin();

    //create ui : Catalogs Win
    void refreshCatalogsWin();

    //1,create ui : Catalog Win;
    //2,loadUnitData
    void refreshCatalogWin(const std::string & sCatalogTitle, QTabWidget * oTabWidget);

    void setCurrentUnit();

    void clearProjectWin();

    UnitUi createUnitUi(QWidget * oTabConfig, QWidget * oTabData);

    QTabWidget *addCatalogTabWidget(CxCatalogAttr *oCatalogAttr);

    void setCurrentRow(int iCurrentRow, bool bSave);

    void refreshAttribute();

private:
    CxDescribeProject _describeProject;
    std::string _dataPath;
    std::string _describePath;
    std::vector<fn_void_t> fnCatalogChangeds;
    std::vector<fn_void_t> fnUnitChangeds;

    std::string _projectFilePath;

    int _currentRow;
    std::string _currentCatalogTitle;
    CxCatalogAttr * _currentCatalogAttr;
    std::vector<UnitUi> _currentUnitUis;
    std::string _currentUnitCaption;
    CxUnitAttr * _currentUnitAttr;
    int _currentTab;
    UnitUi _currentUnitUi;

    ConfigselfPluginProxy * _configselfPluginProxy;

private:
    Ui::ConfigselfWin *ui;

    QMenu * _uiProjectMenu;
    QMenu * _uiCatalogMenu;
    QMenu * _uiUnitMenu;
    QMenu * _uiOtherMenu;
    QMenu * _uiHelpMenu;

    QToolBar * _uiWinsToolBar;

    QAction * _uiProjectLocadAct;
    QAction * _uiProjectSaveAct;

    QAction * _uiSaveCatalogAct;
    QAction * _uiUnitSaveAct;

    QAction * _saveAct;
    QAction * _exitAct;
    QAction * _aboutAct;

    std::map<std::string, std::vector<UnitUi> > _catalogUis;

    bool _isDeleting;

};


class ConfigselfPluginBase
{
public:
    ConfigselfPluginBase() {}

    virtual ~ConfigselfPluginBase() {}

    static std::vector<CxFactoryTemplate<ConfigselfPluginBase>*>* factoriesContainer();

    static void factoriesCreateAndRegister() {}

protected:
    virtual void initImpl(ConfigselfWin * oConfigselfWin) = 0;

    virtual void uninitImpl() {}

    virtual bool projectChangeImpl(const ConfigselfWin* oConfigselfWin, int iState) { return true; }

    virtual bool catalogBeforeChangeImpl(const ConfigselfWin* oConfigselfWin, const std::string & sOldCatalogTitle, const std::string & sNewCatalogTitle) { return true; }

    virtual void catalogChangedImpl(const ConfigselfWin* oConfigselfWin) {}

    virtual bool unitBeforeChangeImpl(const ConfigselfWin* oConfigselfWin, const std::string & sOldUnitCaption, const std::string & sNewUnitCaption) { return true; }

    virtual void unitChangedImpl(const ConfigselfWin* oConfigselfWin) {}

private:
    friend class ConfigselfPluginProxy;

};

class ConfigselfPluginFactoryBase : public CxFactoryTemplate<ConfigselfPluginBase>
{
};

class ConfigselfPluginFactoryManager : public CxFactoryManagerTemplate<ConfigselfPluginBase>
{
};


class ConfigselfPluginProxy
{
public:
    ConfigselfPluginProxy();

    ~ConfigselfPluginProxy();

    inline const std::vector<ConfigselfPluginBase*> & subjects() const { return _plugins; }

    void add(const std::string &sPluginName, const std::string &sPluginParam);

    void deleteAndClear();

    void init(ConfigselfWin * oConfigselfWin);

    void uninit();

    bool projectChange(const ConfigselfWin* oConfigselfWin, int iState);

    bool catalogBeforeChange(const ConfigselfWin* oConfigselfWin, const std::string & sOldCatalogTitle, const std::string & sNewCatalogTitle);

    void catalogChanged(const ConfigselfWin* oConfigselfWin);

    bool unitBeforeChange(const ConfigselfWin* oConfigselfWin, const std::string & sOldUnitCaption, const std::string & sNewUnitCaption);

    void unitChanged(const ConfigselfWin* oConfigselfWin);

private:
    std::vector<ConfigselfPluginBase*> _plugins;

};


#endif // SETUP_WIN_H
