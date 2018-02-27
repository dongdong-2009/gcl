#ifndef SHAREMEMORYVIEWER_WIN_H
#define SHAREMEMORYVIEWER_WIN_H

#include <QMainWindow>


#include <vdi/monsb_sharememory.h>
#include <vdi/ycadd_sharememory.h>
#include <vdi/straw_sharememory.h>
#include <vdi/fileinfo_sharememory.h>

#include <ccxx/qtcommon.h>


namespace Ui {
class ShareMemoryViewer_win;
}

class ShareMemoryViewer_win : public QMainWindow , public ICxTimerNotify
{
    Q_OBJECT

public:
    explicit ShareMemoryViewer_win(QWidget *parent = 0);
    ~ShareMemoryViewer_win();

protected:
    void timer_timeOut(const CxTimer* oTimer);

private slots:
    void on_yxFilterEd1_valueChanged(int arg1);

    void on_yxFilterEd2_valueChanged(int arg1);

    void on_yxFilterEd1_2_valueChanged(int arg1);

    void on_yxFilterEd2_2_valueChanged(int arg1);

    void on_ycFilterEd1_valueChanged(int arg1);

    void on_ycFilterEd2_valueChanged(int arg1);

    void on_ycFilterEd1_2_valueChanged(int arg1);

    void on_ycFilterEd2_2_valueChanged(int arg1);

    void on_ywFilterEd1_valueChanged(int arg1);

    void on_ywFilterEd2_valueChanged(int arg1);

    void on_ywFilterEd1_2_valueChanged(int arg1);

    void on_ywFilterEd2_2_valueChanged(int arg1);

    void on_yxFilterEd5_editTextChanged(const QString &arg1);

    void on_yxFilterEd5_currentTextChanged(const QString &arg1);

    void on_yxFilterEd6_currentTextChanged(const QString &arg1);

private:
    void outInfo(const std::string & sInfo);

    void outInfo(const QString & sInfo);

    void refreshMonsbs();

    void refreshYcadds();

    void refreshStraws();

    void refreshFileInfos();

private:
    Ui::ShareMemoryViewer_win *ui;
    CxTimer _timer;
    int _timerTimes;

};

#endif // SHAREMEMORYVIEWER_WIN_H
