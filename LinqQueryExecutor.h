#ifndef LINQQUERYEXECUTOR_H
#define LINQQUERYEXECUTOR_H

#include <vector>
#include <string>
#include <sstream>
#include "ODBCFramework.h"
#include "QueryBuilder.h"
#include "LinqDb.h"

using namespace std;

// دالة لتحويل wstring إلى string
class StringConverter {
public:
    static string WStringToString(const wstring& wstr);
};

// كلاس LinqQueryExecutor لتنفيذ الاستعلامات وتحويل النتائج
template <typename T>
class LinqQueryExecutor {
private:
    ODBCFramework& db;
    template <typename T>
    T Convert(const wstring& value);

    template <>
    int Convert<int>(const wstring& value) {
        return stoi(value);
    }

    template <>
    string Convert<string>(const wstring& value) {
        return StringConverter::WStringToString(value);
    }


public:
    LinqQueryExecutor(ODBCFramework& database) : db(database) {}

    template <typename T>
    T MapRowToEntity(const vector<wstring>& row) {
        T entity;
        auto tuple_ref = entity.as_tuple(); // الحصول على الحقول تلقائياً

        if (tuple_size<decltype(tuple_ref)>::value != row.size()) {
            throw runtime_error("عدد الأعمدة لا يطابق عدد حقول الكائن!");
        }

        apply([&](auto&... args) {
            size_t index = 0;
            ((args = Convert<typename remove_reference<decltype(args)>::type>(row[index++])), ...);
            }, tuple_ref);

        return entity;
    }


    // تنفيذ الاستعلام واسترجاع البيانات ككائنات T
    LinqDb<T> ExecuteQuery(const wstring& tableName) {
        QueryBuilder qb(tableName);
        qb.Select({ L"*" });  // اختيار كل الأعمدة

        auto x = qb.Build();

        vector<vector<wstring>> results = db.fetchResults(qb.Build());
        vector<T> entities;

        for (const auto& row : results) {
            entities.push_back(MapRowToEntity<T>(row));
        }

        return LinqDb<T>(entities);
    }
};

#endif // LINQQUERYEXECUTOR_H
#ifndef LINQQUERYEXECUTOR_H
#define LINQQUERYEXECUTOR_H

#include <vector>
#include <string>
#include <sstream>
#include "ODBCFramework.h"
#include "QueryBuilder.h"
#include "LinqDb.h"

using namespace std;

// دالة لتحويل wstring إلى string
class StringConverter {
public:
    static string WstringToString(const wstring& wstr);
};

// كلاس LinqQueryExecutor لتنفيذ الاستعلامات وتحويل النتائج
template <typename T>
class LinqQueryExecutor {
private:
    ODBCFramework& db;

public:
    LinqQueryExecutor(ODBCFramework& database) : db(database) {}

    // تحويل صف البيانات إلى كائن من النوع T
    T MapRowToEntity(const vector<wstring>& row) {
        T entity;
        istringstream stream(StringConverter::WstringToString(row[0])); // تحويل أول عمود إلى string
        stream >> entity;
        return entity;
    }

    // تنفيذ الاستعلام واسترجاع البيانات ككائنات T
    LinqDb<T> ExecuteQuery(const wstring& tableName) {
        QueryBuilder qb(tableName);
        qb.Select(L"*");  // اختيار كل الأعمدة

        vector<vector<wstring>> results = db.fetchResults(qb.Build());
        vector<T> entities;

        for (const auto& row : results) {
            entities.push_back(MapRowToEntity(row));
        }

        return LinqDb<T>(entities);
    }
};

#endif // LINQQUERYEXECUTOR_H
