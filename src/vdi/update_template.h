#ifndef UPDATE_TEMPLATE_H
#define UPDATE_TEMPLATE_H

#include <ccxx/ccxx.h>

//静态模板
//类CT 必须有接口：
//static const std::string & getTableName();
//static void update(const std::map<std::string, std::string> &row);
template<class CT>
class UpdateTemplate
{
public:

//sFilePath is xml style :
/**
<?xml version="1.0" encoding="utf-8"?>
<xxx database_name=xxx>
    <bus table_name=bus>
        <filename 1field_value=filename>
            <realtime_data 2field_value=realtime_data>
                <Prop ssid="0" name="ticket" ttid="56"/>
            </rtdata>
        </filename>
    </bus>
</xxx>
 */
    static void update(const std::string &sFilePath)
    {
        std::vector<std::map<std::string, std::string> > rows;
        CxTinyXml::loadTable4Level(sFilePath, rows, "", CT::getTableName());
        update(rows);
    }

    static void update(const char *pData, int iLength)
    {
        std::vector<std::map<std::string, std::string> > rows;
        CxTinyXml::loadTable4Level(pData, iLength, rows, "", CT::getTableName());
        update(rows);
    }

    static void update(const std::vector<std::map<std::string, std::string> > &rows)
    {
        for (size_t i = 0; i < rows.size(); ++i)
        {
            const std::map<std::string, std::string> & row = rows.at(i);

            CT::update(row);
        }
    }

};

#endif // UPDATE_TEMPLATE_H
