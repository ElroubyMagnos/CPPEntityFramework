#ifndef ODBC_FRAMEWORK_H
#define ODBC_FRAMEWORK_H

#include <windows.h>
#include <sqlext.h>
#include <string>
#include <vector>
#include "QueryBuilder.h"
using namespace std;

class ODBCFramework {
private:
    SQLHANDLE hEnv;
    SQLHANDLE hDbc;
    bool isConnected;

public:
    string WStringToString(const wstring& wstr);
    ODBCFramework();
    ~ODBCFramework();
    bool connect(const wstring& dsn, const wstring& user, const wstring& password);
    bool connect(const wstring& connectionString);
    void disconnect();
    bool executeQuery(const wstring& query);
    vector<vector<wstring>> fetchResults(const wstring& query);
    void showError(SQLHANDLE handle, SQLSMALLINT type);

    vector<vector<wstring>> executeQueryBuilder(const QueryBuilder& qb);
};

#endif
