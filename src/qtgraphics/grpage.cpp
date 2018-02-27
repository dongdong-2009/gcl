
#include "grpage.h"

GRPage::GRPage()
{
}

GRPage::GRPage(const QString &subName, const QString &paName, const QString &filCor,
           int hei, int wid) :
    m_substationName(subName),
    m_pageName(paName),
    m_fillColor(filCor),
    n_height(hei),
    n_width(wid)
{
}

GRPage::GRPage(const GRPage &otherPage) :
    m_substationName(otherPage.m_substationName),
    m_pageName(otherPage.m_pageName),
    m_fillColor(otherPage.m_fillColor),
    n_height(otherPage.n_height),
    n_width(otherPage.n_width)
{
}

const GRPage &GRPage::operator =(const GRPage &otherPage)
{
    if(this == &otherPage)
    {
        return *this;
    }

    m_substationName = otherPage.m_substationName;
    m_pageName = otherPage.m_pageName;
    m_fillColor = otherPage.m_fillColor;
    n_height = otherPage.n_height;
    n_width = otherPage.n_width;

    return *this;
}
