#ifndef TABLE_WIN_H
#define TABLE_WIN_H

#include <QString>
#include <QList>
#include <QStringList>
#include <QHash>
#include <QHBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QItemDelegate>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QListWidget>
#include <QMenu>
#include <QAction>

#include <ccxx/cxdescribe.h>

class TableWin;

class TableItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit TableItemDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    inline CxDescribeUnit* describeUnit() const { return _describeUnit; }
    inline void setDescribeUnit(CxDescribeUnit* value) { _describeUnit = value; }

private:
    CxDescribeUnit* _describeUnit;

    friend class TableWin;

};


class TableWin : public QObject
{
    Q_OBJECT

public:
    explicit TableWin();

    virtual ~TableWin();

    inline CxDescribeUnit * describeUnit() const { return _describeUnit; }
    void setDescribeUnit(CxDescribeUnit * value);

    void loadWin(QWidget* oParent, const std::string & sDataFilePath, const std::string & sUiFilePath = "", const std::string& sDataType = std::string(), const std::string& sDataParam = std::string());

    int saveWin(const std::string & sDataFilePath, const std::string& sDataType = std::string(), const std::string& sDataParam = std::string());

    inline const QHash<QString, QString> rowChangedNames () const { return _rowChangedNames; }

    inline QTableView* tableView() const { return _tableView; }

    void initUi(QWidget *oParent);

    void clearData();

    void loadData(const std::string &sDataFilePath, const std::string &sDataType, const std::string &sDataParam);

    void redefault();

protected:
    bool eventFilter(QObject *, QEvent *);

    void deleteTableSelectRows();

    void insertTableRow();

    void searchTableText();

    void searchListText();

    void insertTableRowsByList();

protected:
    CxDescribeUnit* _describeUnit;
    QWidget* _parent;

    QHBoxLayout* _layout;
    QTableView* _tableView;
    QStandardItemModel* _tableItemModel;
    TableItemDelegate* _tableItemDelegate;
    QMenu* _tableMenu;
    QAction* _setSameValueAction;
    QAction* _setIncreaseValueAction;
    QAction* _importCaptionsAction;
    QAction* _exportCaptionsAction;
    QAction* _batchAddRowsAction;

    QListWidget* _listWidget;

    QHash<QString, QString> _rowChangedNames;

    bool _isSetDescribeUnit;

private slots:
    void customContextMenu(const QPoint &pos);

    void setSameValueSlot();

    void setIncreaseValueSlot();

    void exportCaptionsSlot();

    void importCaptionsSlot();

    void batchAddRowsSlot();

};
#endif // TABLE_WIN_H
