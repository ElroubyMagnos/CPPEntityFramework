#ifndef QUERY_BUILDER_H
#define QUERY_BUILDER_H
using namespace std;
#include <string>
#include <vector>

class QueryBuilder {
private:
    wstring tableName;
    vector<wstring> columns;
    wstring whereClause;
    wstring orderByClause;
    int limitValue;

public:
    QueryBuilder(const wstring& table);
    QueryBuilder& Select(const vector<wstring>& cols);
    QueryBuilder& Where(const wstring& condition);
    QueryBuilder& OrderBy(const wstring& column);
    QueryBuilder& Limit(int count);
    wstring Build() const;
};

#endif // QUERY_BUILDER_H
