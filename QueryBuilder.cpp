#include "pch.h"
#include "QueryBuilder.h"

QueryBuilder::QueryBuilder(const std::wstring& table) : tableName(table), limitValue(-1) {}

QueryBuilder& QueryBuilder::Select(const std::vector<std::wstring>& cols) {
    columns = cols;
    return *this;
}

QueryBuilder& QueryBuilder::Where(const std::wstring& condition) {
    whereClause = condition;
    return *this;
}

QueryBuilder& QueryBuilder::OrderBy(const std::wstring& column) {
    orderByClause = column;
    return *this;
}

QueryBuilder& QueryBuilder::Limit(int count) {
    limitValue = count;
    return *this;
}

std::wstring QueryBuilder::Build() const {
    std::wstring query = L"SELECT ";

    if (columns.empty()) {
        query += L"*";
    }
    else {
        for (size_t i = 0; i < columns.size(); ++i) {
            query += columns[i];
            if (i < columns.size() - 1) query += L", ";
        }
    }

    query += L" FROM " + tableName;

    if (!whereClause.empty()) query += L" WHERE " + whereClause;
    if (!orderByClause.empty()) query += L" ORDER BY " + orderByClause;
    if (limitValue > 0) query += L" FETCH FIRST " + std::to_wstring(limitValue) + L" ROWS ONLY";

    return query;
}
