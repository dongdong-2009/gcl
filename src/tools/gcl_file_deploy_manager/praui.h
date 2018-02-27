#ifndef PRAUI_H
#define PRAUI_H


#include <QtGui>
#include <ccxx/qtcommon.h>

#include "pra.h"


#define pra_win_state_none (0)
#define pra_win_state_view (2)
#define pra_win_state_edit (4)
#define pra_win_state_load (8)


class PraUi
{
public:
    static void praHeadInit(QTableWidget * oTableWidget);

    static void praFill(QTableWidget * oTableWidget, int iRowIndex, const Pra & pra);

    static void praInsert(QTableWidget * oTableWidget, const Pra & pra);

    static void praItemHeadInit(QTableWidget * oTableWidget);

    static void praItemInsert(QTableWidget * oTableWidget, const PraItem & praItem);

    static void prasRefresh(QTableWidget * oTableWidget, const std::vector<Pra> & pras);

    static void fdcItemHeadInit(QTableWidget * oTableWidget);

    static void fdcItemAdd(QTableWidget * oTableWidget, const FdcItem & fdcItem, const std::string & sMd5_r, const std::string & sMd5_l, const std::string & sState);

    static void fdcItemInsert(QTableWidget * oTableWidget, const FdcItem & fdcItem, const std::string & sMd5_r, const std::string & sMd5_l, const std::string & sState);

    static void fdcItemCreate(QTableWidget * oTableWidget, int iRowIndex, const FdcItem & fdcItem, const std::string & sMd5_r, const std::string & sMd5_l, const std::string & sState);

    static void fdcItemFill(QTableWidget * oTableWidget, const std::string & sRfilePath, const std::string & sMd5_r, const std::string & sMd5_l, const std::string & sState);

    static void fdcItemClearMd5_r(QTableWidget * oTableWidget);

    static void fdcItemClearState(QTableWidget * oTableWidget);

};


#endif // PRAUI_H
