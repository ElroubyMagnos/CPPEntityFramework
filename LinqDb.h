#pragma once
#ifndef LINQ_DB_H
#define LINQ_DB_H

#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>

using namespace std;

template <typename T>
class LinqDb {
private:
    vector<T> data;

public:
    LinqDb(const vector<T>& initialData) : data(initialData) {}

    LinqDb<T> Select(function<T(T)> selector) const {
        vector<T> result;
        for (const auto& item : data) {
            result.push_back(selector(item));
        }
        return LinqDb<T>(result);
    }

    LinqDb<T> Where(std::function<bool(const T&)> predicate) const {
        std::vector<T> filtered;
        for (const auto& item : data) {
            if (predicate(item)) {  // التحقق من الشرط
                filtered.push_back(item);
            }
        }
        return LinqDb<T>(filtered);  // إرجاع كائن جديد من LinqDb يحتوي على العناصر المُصفاة
    }

    LinqDb<T> OrderBy(function<bool(const T&, const T&)> comparator) const {
        vector<T> result = data;
        sort(result.begin(), result.end(), comparator);
        return LinqDb<T>(result);
    }

    LinqDb<T> Limit(int count) const {
        vector<T> result(data.begin(), data.begin() + min(count, (int)data.size()));
        return LinqDb<T>(result);
    }

    void Print(function<void(const T&)> printer) const {
        for (const auto& item : data) {
            printer(item);
        }
    }

    vector<T> ToVector() const {
        return data;
    }
};

#endif // LINQ_DB_H
