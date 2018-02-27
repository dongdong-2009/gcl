#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include"sqlite3.h"
#include<string.h>

using namespace std;

#pragma comment(lib, "sqlite3.lib")

int f_argc;
char ** f_argv;

int testQuery1(int argc,char **argv)
{
    int rc,i,ncols;
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *sql;
    const char*tail;
    // 打开数据
    rc=sqlite3_open("foods.db",&db);
    if(rc){
        fprintf(stderr,"Can'topendatabase:%sn",sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sql="select * from episodes";
    // 预处理
    rc=sqlite3_prepare(db,sql,(int)strlen(sql),&stmt,&tail);
    if(rc!=SQLITE_OK){
        fprintf(stderr,"SQLerror:%sn",sqlite3_errmsg(db));
    }
    rc=sqlite3_step(stmt);
    ncols=sqlite3_column_count(stmt);
    while(rc==SQLITE_ROW){
        for(i=0;i<ncols;i++){
            fprintf(stderr,"'%s'",sqlite3_column_text(stmt,i));
        }
        fprintf(stderr,"\n");
        rc=sqlite3_step(stmt);
    }
    // 释放 statement
    sqlite3_finalize(stmt);
    // 关闭数据库
    sqlite3_close(db);

    printf("\n");

    return(0);
}



int callback(void* data, int ncols, char** values, char** headers);
int testExec1(int argc, char **argv)
{
    sqlite3 *db;
    int rc;
    char *sql;
    char *zErr;
    char* data;
    rc = sqlite3_open("test.db", &db);
    if(rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    data = "Callback function called";
    sql = "insert into episodes (name, cid) values ('Mackinaw Peaches', 1);"
            "select * from episodes;";
    rc = sqlite3_exec(db, sql, callback, data, &zErr);
    if(rc != SQLITE_OK) {
        if (zErr != NULL) {
            fprintf(stderr, "SQL error: %s\n", zErr);
            sqlite3_free(zErr);
        }
    }
    sqlite3_close(db);
    return 0;
}
int callback(void* data, int ncols, char** values, char** headers)
{
    int i;
    fprintf(stderr, "%s: ", (const char*)data);
    for(i=0; i < ncols; i++) {
        fprintf(stderr, "%s=%s ", headers[i], values[i]);
    }
    fprintf(stderr, "\n");
    return 0;
}


int testCreateTable1()
{
    sqlite3 *db;
    int rc;
    char *sql;
    char *zErr;
    char* data;
    rc = sqlite3_open("test.db", &db);
    if(rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sql =   "CREATE TABLE [t3] ("
            "[i] BIGINT ,"
            "[dt] UNSIGNED BIGINT,"
            ")"
            ;
    rc = sqlite3_exec(db, sql, NULL, NULL, &zErr);
    if(rc != SQLITE_OK) {
        if (zErr != NULL) {
            fprintf(stderr, "SQL error: %s\n", zErr);
            sqlite3_free(zErr);
        }
    }
    sqlite3_close(db);
    return 0;
}

int testInsert1()
{
    sqlite3 *db;
    int rc;
    char *sql;
    char *zErr;
    char* data;
    rc = sqlite3_open("test.db", &db);
    if(rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    data = "Callback function called";
    sql = "insert into episodes (name, cid) values ('Mackinaw Peaches', 1);"
            "select * from episodes;";
    rc = sqlite3_exec(db, sql, callback, data, &zErr);
    if(rc != SQLITE_OK) {
        if (zErr != NULL) {
            fprintf(stderr, "SQL error: %s\n", zErr);
            sqlite3_free(zErr);
        }
    }
    sqlite3_close(db);
    return 0;
}


int main(int argc,char **argv)
{
    cout << "Hello World!" << endl;

    f_argc = argc;
    f_argv = argv;

    testCreateTable1();

    string s;
    cin >> s;
    return 0;
}


