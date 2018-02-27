
/*

#include <ccxx/ccxx.h>
#include "gcl.h"
#include "explain_efile.h"

using namespace std;

const char cc_flag_line = '\n';
const char cc_flag_tab = '\t';
const char cc_flag_space = ' ';

static map<string, string> f_eFileDeclare;
static vector<EFileElement> f_eFileEle;
static string f_eFC;
static map<string,vector<string> > f_mapTableToFiledName;

////////////////////////////////////////////////////////////////////////////////
EFileElement::EFileElement()
{
    clear();
}

EFileElement::~EFileElement()
{
   clear();
}

EFileElement &EFileElement::operator=(const EFileElement &o)
{
    _className     = o._className;
    _InstanceName  = o._InstanceName;
    _field         = o._field;
    _rec           = o._rec;
    _unit          = o._unit;
    _type          = o._type;
    return *this;
}

void EFileElement::clear()
{
    _className.clear();
    _InstanceName.clear();
    _field.clear();
    _rec.clear();
    _unit.clear();
    _type = ee_type_none;
}

vector<string> EFileElement::toEfileBuffer()
{
   vector<string> sLines;
   sLines.clear();
   switch(_type)
   {
   case ee_type_row: //目前只支持横表模式
   {
       string sLine;
       //类::实例名
       sLine = "<" + _className + "::" + _InstanceName + ">";
       sLines.push_back(sLine);
       //字段
       sLine = "@";
       if(_field.size()>0)
       {
            sLine.push_back(cc_flag_tab);
            sLine += CxString::join(_field,cc_flag_tab);
       }
       sLines.push_back(sLine);
       //记录
       for (size_t i = 0; i < _rec.size(); ++i)
       {
          vector<string> rec = _rec.at(i);
          sLine = "#";
          sLine.push_back(cc_flag_tab);
          sLine += CxString::join(rec,cc_flag_tab);
          sLines.push_back(sLine);
       }
       //结束
       sLine = "</" + _className + "::" + _InstanceName + ">";
       sLines.push_back(sLine);
   }
       break;
   default:
       break;
   }
   return sLines;

}

int EFileElement::fromEfileBuffer(const char *pData, int iLength)
{

}

vector<map<string, string> > EFileElement::getRec()
{
     vector<map<string, string> > vRecs;
     vRecs.clear();

//     cxPrompt()<<"field size" << _field.size();
//     cxPrompt()<<"_rec size" << _rec.size();

     if(_field.size()==0 || _rec.size()==0 ) return vRecs;

     for(size_t i=0;i<_rec.size();++i)
     {
         map<string, string> map1;
         vector<string> rec = _rec.at(i);

//         cxPrompt()<<"rec size" << rec.size();

         if(rec.size()!=_field.size())continue;

         for(size_t j=0;j<rec.size();++j)
         {
             map1[_field.at(j)] = rec.at(j);
         }

         vRecs.push_back(map1);
     }
     return vRecs;
}

void EFileElement::setRec(vector<map<string, string> > &vRec)
{
    if(_field.size()==0 )return;

    _rec.clear();

    for(size_t i=0;i<vRec.size();++i)
    {
        map<string, string> map1 = vRec.at(i);
        if(_field.size()!= map1.size())continue;

        vector<string> rec;

        for(size_t j=0;j<_field.size();++j)
        {
            rec.push_back(CxContainer::value(map1,_field.at(j)));
        }
        _rec.push_back(rec);
    }
}

void EFileElement::setRec2(vector<map<string, string> > &vRec)
{
    if(vRec.size()<1) return;

    _field.clear();

    map<string,string> map1 = vRec.at(0);

    for(map<string,string>::const_iterator it = map1.begin(); it != map1.end(); ++it)
    {
        //更新本地值
        _field.push_back(it->first);
    }

    if(_field.size()<1) return;

    _rec.clear();

    for(size_t i=0;i<vRec.size();++i)
    {
        map1 = vRec.at(i);

         if(_field.size()!= map1.size())continue;

        vector<string> rec;

        for(size_t j=0;j<_field.size();++j)
        {
            rec.push_back(CxContainer::value(map1,_field.at(j)));
        }
        _rec.push_back(rec);
    }
}


int EFileElement::saveTable(vector<int> &columnTypes)
{
    if(_InstanceName.length()>0 && _field.size()>0 && _rec.size()>0)
    {
        return CxDatabase::defaultDb()->saveTable(_InstanceName, _field, _rec, columnTypes, true);
    }
    return -1;
}

int EFileElement::loadTable(const string &sql)
{
    vector<map<string, string> > vRecs = CxDatabase::defaultDb()->queryToMapVector(sql);
    if(vRecs.size()>0)
    {
        setRec(vRecs);
    }
    return GCL_SUCCED;
}
int EFileElement::loadTable2(const string &sql)
{
    vector<map<string, string> > vRecs = CxDatabase::defaultDb()->queryToMapVector(sql);
    if(vRecs.size()>0)
    {
        setRec2(vRecs);
    }
    return GCL_SUCCED;
}
//////////////////////////////////////////////////////////////////////////////////////////


ExplainEfile::ExplainEfile()
{

}
//文件检查
bool ExplainEfile::check(const string &sFile, const string &sErr)
{
    //need to do
    return false;
}

int ExplainEfile::explain(const char *pData, int iLength, map<string, string> &heads, map<string, string> &majors, vector<map<string, string> > &details)
{

}
//解析E文件
int ExplainEfile::explain(const char *pData, int iLength, map<string, string> &declare, vector<EFileElement> &vObj)
{
    vector<string> sLines = CxString::split(string(pData, iLength), cc_flag_line);
    return explain(sLines,declare,vObj);

}
int ExplainEfile::explain(string &data, map<string, string> &declare, vector<EFileElement> &vObj)
{
    vector<string> sLines = CxString::split(data, cc_flag_line);
    return explain(sLines,declare,vObj);
}
int ExplainEfile::explain(const vector<string> &data, map<string, string> &declare, vector<EFileElement> &vObj)
{
    string sLine;
    EFileElement ee;
    int err = GCL_ERR_FAIL;

    for (size_t i = 0; i < data.size(); ++i)
    {
        sLine = CxString::trim(data.at(i));
        if (sLine.empty()) continue;

        switch (sLine.data()[0])
        {
        case '<'://头部
        {
            if(sLine.size()<2)break;
            if((sLine.data()[1])=='!')//声明
            {
                string s = CxString::unquote(sLine, '!', '!');
                s = CxString::trim(s);
                declare = CxString::splitToMap(s,'=',' ');
            }
            else if((sLine.data()[1])!='/')//类名实例名
            {
                string s = CxString::unquote(sLine, '<', '>');
                s = CxString::trim(s);
                string v = CxString::token(s, "::");
                ee.clear();
                ee.setName(v,s);
            }
            else
            {
                vObj.push_back(ee); //表结束
                err = GCL_SUCCED;
            }
        }
            break;
        case '@'://定义
        {
            if(sLine.size()<2)break;
            if((sLine.data()[1])=='@')//列表
            {

            }
            else if ((sLine.data()[1])=='#')//
            {

            }
            else //横表结构
            {
                string s = sLine.substr(1, sLine.size()-1);
                ee.seField(CxString::split(s, cc_flag_tab));
                ee.setType(EFileElement::ee_type_row);
            }
        }
            break;
        case '#': //记录
        {
            if (sLine.size() > 1)
            {
                string s = sLine.substr(1, sLine.size()-1);
                ee.addRec(CxString::split(s, cc_flag_tab));
//                ee.addRec(CxString::split(s, cc_flag_tab, true));

            }
        }
            break;
        case '/': //注释
        {
            if (sLine.size() > 2)
            {
                string sComment = sLine.substr(2, sLine.size()-2);
            }
        }
            break;
        case '%'://注释
        {
            if (sLine.size() > 2)
            {
                string s = sLine.substr(2, sLine.size()-2);
                ee.setUnit(CxString::split(s, cc_flag_tab));
            }
        }
            break;
        default:
            break;
        }
    }
    return err;
}


//生成E文件
vector<char> ExplainEfile::toEfileBuffer(map<string, string> &heads, map<string, string> &majors, vector<map<string, string> > &details)
{

}
//生成E文件
vector<string> ExplainEfile::toEfileBuffer(map<string, string> &declare, vector<EFileElement> &vObj)
{
    vector<string> sLines;
    string sLine;

    sLine = "<! ";
    sLine += CxString::join(declare, "=", " ");
    sLine += " !>";
    sLines.push_back(sLine);

    for (size_t i = 0; i < vObj.size(); ++i)
    {
        EFileElement ee = vObj.at(i);
        vector<string> es = ee.toEfileBuffer();
        if(es.size()>0)CxContainer::append(sLines,es);
    }
    return sLines;
}
//加载文件
int ExplainEfile::loadFromFile(const string & sFile, map<string, string> &declare, vector<EFileElement> &vObj)
{
    int err = GCL_ERR_FAIL;
    vector<string> vBuff;
    vBuff.clear();

    if(CxFile::load(sFile,vBuff,cc_flag_line)>0)
    {
        return explain(vBuff,declare,vObj);
    }
    return GCL_ERR_FAIL;
}
int ExplainEfile::loadFromString(const string &sBuffer, map<string, string> &declare, vector<EFileElement> &vObj)
{
    int err = GCL_ERR_FAIL;
    vector<string> vBuff = CxString::split(sBuffer,"\r\n");
    if(vBuff.size()>0)
    {
        err = explain(vBuff,declare,vObj);
    }
    return err;
}


//存文件
int ExplainEfile::saveToFile(const string & sFile, map<string, string> &declare, vector<EFileElement> &vObj)
{
    vector<string> sLines = toEfileBuffer(declare,vObj);
    int err = GCL_ERR_FAIL;
    if(sLines.size()>0)
    {
        if(CxFile::save(sFile,sLines))
        {
            err = GCL_SUCCED;
        }
    }
    return err;
}
///////////////////////////////////////////////////////////////////////////////
void ExplainEfile::clear()
{
    f_eFileDeclare.clear();
    f_eFileEle.clear();
}

void ExplainEfile::setFieldTemplate(map<string, vector<string> > &map1)
{
    f_mapTableToFiledName = map1;
}

bool ExplainEfile::createDeclare(map<string, string> &declare)
{
    f_eFileDeclare = declare;
}

bool ExplainEfile::createElement_sql(const string &sClassName, const string &sInstanceName, const string &sql,const string &sFieldList)
{
    if(sFieldList.size()<1) return false;

    EFileElement obj;
    obj.setName(sClassName,sInstanceName);
    obj.setType(EFileElement::ee_type_row);
    vector<string> vField = CxString::split(sFieldList,',');
    obj.seField(vField);
    obj.loadTable(sql);
    f_eFileEle.push_back(obj);

    return true;
}


bool ExplainEfile::createElement_sql(const string &sClassName, const string &sInstanceName,const string &sql)
{
    string sKey = sClassName;
    if(!CxContainer::contain(f_mapTableToFiledName,sKey)) return false;

    EFileElement obj;
    obj.setName(sClassName,sInstanceName);
    obj.setType(EFileElement::ee_type_row);
    obj.seField(CxContainer::value(f_mapTableToFiledName,sKey));

    obj.loadTable(sql);

    f_eFileEle.push_back(obj);

    return true;
}



bool ExplainEfile::createElement_row(const string &sClassName, const string &sInstanceName, const string &row)
{
    EFileElement obj;
    obj.setName(sClassName,sInstanceName);
    obj.setType(EFileElement::ee_type_row);

    vector<vector<string> > v = CxString::splitToLines(row,'=',';');

    vector<string> vField,vVale;

    for(int i=0;i<v.size();++i)
    {
        vector<string> vv = v.at(i);
        if(vv.size()!=2) continue;
        vField.push_back(vv.at(0));
        vVale.push_back(vv.at(1));
    }
    obj.seField(vField);
    obj.addRec(vVale);

    f_eFileEle.push_back(obj);

    return true;
}

bool ExplainEfile::createElement_rows(const string &sClassName, const string &sInstanceName, vector<string> &rows)
{
    if(rows.size()<1) return false;

    EFileElement obj;
    obj.setName(sClassName,sInstanceName);
    obj.setType(EFileElement::ee_type_row);

    string row = rows.at(0);

    vector<vector<string> > v = CxString::splitToLines(row,'=',';');

    vector<string> v0,v1;

    for(int i=0;v.size();++i)
    {
        vector<string> vv = v.at(i);
        if(vv.size()!=2) continue;
        v0.push_back(vv.at(0));
        v1.push_back(vv.at(1));
    }
    obj.seField(v0);
    obj.addRec(v1);

    for(int j=0;j<rows.size();++j)
    {
        v = CxString::splitToLines(rows.at(j),'=',';');
        v1.clear();
        for(int i=0;v.size();++i)
        {
            vector<string> vv = v.at(i);
            if(vv.size()!=2) continue;
            v1.push_back(vv.at(1));
        }
        obj.addRec(v1);
    }

    f_eFileEle.push_back(obj);

    return true;
}

int ExplainEfile::toFile(const string &sFile)
{
    vector<string> sLines = toEfileBuffer(f_eFileDeclare,f_eFileEle);
    int err = GCL_ERR_FAIL;
    if(sLines.size()>0)
    {
        if(CxFile::save(sFile,sLines,"\r\n",true))
        {
            err = GCL_SUCCED;
        }
    }
    return err;
}

string ExplainEfile::toString()
{
    string sBuf="";
    vector<string> sLines = toEfileBuffer(f_eFileDeclare,f_eFileEle);
    if(sLines.size()>0)
    {
        sBuf = CxString::join(sLines,"\r\n");
    }
    return sBuf;
}



*/

