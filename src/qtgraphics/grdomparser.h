
#ifndef GRDOMPARSER_H
#define GRDOMPARSER_H
#include "graphics_global.h"
#include <QVector>

class GRPage;
class GRItemDataInterface;
class QDomElement;
class GCLGRAPHICSSHARED_EXPORT GRDomParser
{
public:
    GRDomParser();

public:
    bool readFile(const QString &fileName);
    QVector<GRPage *> getPageVector() const;
    QVector<GRItemDataInterface *> getElementVector() const;

private:
    void parsePageslement(const QDomElement &element); //pages
    void parseSinglePageElement(const QDomElement &element, int pageIndex);  //single page

//    void loadEquipmentTypes(const QDomElement &element); //equipmentTypes
    void loadEquipments(const QDomElement &element, int pageIndex);    //equipments
    void loadConnectLine(const QDomElement &element, int pageIndex);    //
    void loadConnectPoints(const QDomElement &element, int pageIndex);    //
    void loadOtherGraphics(const QDomElement &element, int pageIndex);    //
    void loadBuss(const QDomElement &element, int pageIndex);    //

    void loadDeviceType(const QDomElement &element);    //从xml文件获取设备类型
//    void loadDevices(const QDomElement &element, int pageIndex);       //从xml文件获取设备

private:
    QVector<GRPage *> m_pageVect;                       //存放页面信息
    QVector<GRItemDataInterface *> m_itemDataVect;      //存放图元
};

#endif // GRDOMPARSER_H
