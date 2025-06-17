#include "pch.h"
#include "ODBCFramework.h"
#include <iostream>
#include <locale>
#include <codecvt>
#include <string>
#include <Windows.h>

vector<vector<wstring>> ODBCFramework::executeQueryBuilder(const QueryBuilder& qb) {
    return fetchResults(qb.Build());
}


std::string ODBCFramework::WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed - 1, 0);  // -1 لتجاهل null terminator
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
    return str;
}

ODBCFramework::ODBCFramework() : isConnected(false), hEnv(SQL_NULL_HENV), hDbc(SQL_NULL_HDBC) {
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
}

ODBCFramework::~ODBCFramework() {
    disconnect();
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

bool ODBCFramework::connect(const wstring& dsn, const wstring& user, const wstring& password) {
    SQLRETURN ret = SQLConnect(
        hDbc,
        (SQLWCHAR*)dsn.c_str(), SQL_NTS,     // اسم مصدر البيانات (DSN)
        (SQLWCHAR*)user.c_str(), SQL_NTS,    // اسم المستخدم
        (SQLWCHAR*)password.c_str(), SQL_NTS // كلمة المرور
    );

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        showError(hDbc, SQL_HANDLE_DBC);
        return false;
    }

    isConnected = true;
    return true;
}

bool ODBCFramework::connect(const wstring& connectionString) {
    SQLWCHAR outConnStr[1024];
    SQLSMALLINT outConnStrLen;

    SQLRETURN ret = SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connectionString.c_str(), SQL_NTS,
        outConnStr, sizeof(outConnStr) / sizeof(SQLWCHAR), &outConnStrLen, SQL_DRIVER_NOPROMPT);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        showError(hDbc, SQL_HANDLE_DBC);
        return false;
    }
    isConnected = true;
    return true;
}

void ODBCFramework::disconnect() {
    if (isConnected) {
        SQLDisconnect(hDbc);
        isConnected = false;
    }
}

bool ODBCFramework::executeQuery(const wstring& query) {
    SQLHANDLE hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLRETURN ret = SQLExecDirect(hStmt, const_cast<SQLWCHAR*>(query.c_str()), SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        showError(hStmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return false;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return true;
}

vector<vector<wstring>> ODBCFramework::fetchResults(const wstring& query) 
{
    SQLHANDLE hStmt = SQL_NULL_HSTMT;
    vector<vector<wstring>> results;

    if (hDbc == SQL_NULL_HDBC) {
        std::wcerr << L"⚠ مقبض الاتصال بقاعدة البيانات غير صالح!" << std::endl;
        return results;
    }

    // ✅ تخصيص مقبض البيان (Statement Handle)
    SQLRETURN retAlloc = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    if (retAlloc != SQL_SUCCESS && retAlloc != SQL_SUCCESS_WITH_INFO) {
        std::wcerr << L"❌ فشل في تخصيص مقبض البيان!" << std::endl;
        showError(hDbc, SQL_HANDLE_DBC);
        return results;
    }

    // ✅ تنفيذ الاستعلام
    SQLRETURN retExec = SQLExecDirect(hStmt, reinterpret_cast<SQLWCHAR*>(const_cast<wchar_t*>(query.c_str())), SQL_NTS);
    if (retExec == SQL_NO_DATA) {
        std::wcerr << L"⚠ الاستعلام لم يرجع أي بيانات!" << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return results;
    }
    else if (retExec != SQL_SUCCESS && retExec != SQL_SUCCESS_WITH_INFO) {
        std::wcerr << L"❌ خطأ في تنفيذ الاستعلام!" << std::endl;
        showError(hStmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return results;
    }

    // ✅ الحصول على عدد الأعمدة
    SQLSMALLINT numCols = 0;
    SQLNumResultCols(hStmt, &numCols);
    if (numCols == 0) {
        std::wcerr << L"⚠ لم يتم إرجاع أي أعمدة!" << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return results;
    }

    vector<SQLWCHAR> buffer(256);
    SQLLEN indicator;
    SQLRETURN fetchRet = SQLFetch(hStmt);

    if (fetchRet == SQL_NO_DATA) {
        std::wcerr << L"⚠ لم يتم العثور على أي بيانات!" << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return results;
    }
    else if (fetchRet != SQL_SUCCESS && fetchRet != SQL_SUCCESS_WITH_INFO) {
        std::wcerr << L"❌ خطأ أثناء جلب البيانات!" << std::endl;
        showError(hStmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return results;
    }

    // ✅ جلب البيانات من الجدول
    do {
        vector<wstring> row;
        for (SQLSMALLINT i = 1; i <= numCols; ++i) {
            memset(buffer.data(), 0, buffer.size() * sizeof(SQLWCHAR));
            SQLRETURN retData = SQLGetData(hStmt, i, SQL_C_WCHAR, buffer.data(), buffer.size() * sizeof(SQLWCHAR), &indicator);

            if (retData == SQL_SUCCESS || retData == SQL_SUCCESS_WITH_INFO) {
                if (indicator == SQL_NULL_DATA) {
                    row.push_back(L"NULL");
                    std::wcout << L"العمود " << i << L": NULL" << std::endl;
                }
                else {
                    std::wcout << L"العمود " << i << L": " << buffer.data() << std::endl;
                    row.push_back(wstring(buffer.data()));
                }
            }
            else {
                std::wcout << L"⚠ خطأ في قراءة العمود " << i << std::endl;
                row.push_back(L"ERROR");
            }
        }
        results.push_back(row);
    } while (SQLFetch(hStmt) == SQL_SUCCESS);

    // ✅ تحرير الموارد
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return results;
}


void ODBCFramework::showError(SQLHANDLE handle, SQLSMALLINT type) {
    SQLWCHAR sqlState[1024];
    SQLWCHAR message[1024];
    if (SQLGetDiagRec(type, handle, 1, sqlState, NULL, message, 1024, NULL) == SQL_SUCCESS) {
        cout << L"SQL Error: " << message << L" (SQL State: " << sqlState << L")" << endl;
    }
}
