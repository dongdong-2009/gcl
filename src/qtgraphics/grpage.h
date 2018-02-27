
#ifndef GRPAGE_H
#define GRPAGE_H
#include "graphics_global.h"
#include <QString>

class GCLGRAPHICSSHARED_EXPORT GRPage
{
public:
    GRPage();
    GRPage(const QString &subName, const QString &paName, const QString &filCor,
         int hei, int wid);
    GRPage(const GRPage &otherPage);
    const GRPage &operator=(const GRPage &otherPage);

public:
    QString getPageSubstationName() const
    {
        return m_substationName;
    }

    QString getPageName() const
    {
        return m_pageName;
    }

    QString getPageFillColor() const
    {
        return m_fillColor;
    }

    int getPageHeight() const
    {
        return n_height;
    }

    int getPageWidth() const
    {
        return n_width;
    }

private:
    QString m_substationName; //站名
    QString m_pageName;       //页名
    QString m_fillColor;      //背景填充色
    int n_height;             //高度
    int n_width;              //宽度
};

#endif // GRPAGE_H
