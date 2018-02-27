
#include <ccxx/ccxx.h>

using namespace std;


int testCreate1()
{
    std::string sTableName = "table1";
    std::map<std::string, int> columns;
    columns["int1"] = CxDatabase::column_type_int;
    columns["string64"] = CxDatabase::column_type_varchar64;
    CxDatabase::defaultDb()->createTable(sTableName, columns);
    return 0;
}

int testInsert1()
{
    std::string sTableName = "table1";
    std::vector<std::string> columnNames;
    columnNames.push_back("int1");
    columnNames.push_back("string64");
    std::vector<std::vector<std::string> > rows;
    for (size_t i = 0; i < 10; ++i)
    {
        vector<string> row;
        for (size_t j = 0; j < columnNames.size(); ++j)
        {
            row.push_back(CxString::toString(i * 100 + j));
        }
        rows.push_back(row);
    }

    return CxDatabase::defaultDb()->saveTable(sTableName, columnNames, rows);
}

int testSelect1()
{
    std::string sTableName = "table1";
    std::vector<std::string> columnNames;
//    columnNames.push_back("int1");
//    columnNames.push_back("string64");
    std::vector<std::vector<std::string> > rows;

    int iCount = CxDatabase::defaultDb()->loadTable(sTableName, rows, & columnNames);

    for (size_t i = 0; i < columnNames.size(); ++i)
    {
        cout << "column name : " << columnNames.at(i) << endl;
    }

    for (size_t i = 0; i < rows.size(); ++i)
    {
        const vector<string> & row = rows.at(i);
        string sLine;
        for (size_t j = 0; j < row.size(); ++j)
        {
            sLine += row.at(j) + "  -  ";
        }
        cout << "row " << i << ": " << sLine << endl;
    }

    return iCount;
}


int main(int argc, char * argv[])
{
    cout << "begin test database : " << endl;

    CxApplication::init(argc, argv);

    CxDatabase::defaultDb()->openDatabase(":memory:");
//    CxDatabase::defaultDb()->openDatabase("db1.db");

    testCreate1();

    testInsert1();

    testSelect1();

    cout << "end test database !!! " << endl;

    return 0;
}



