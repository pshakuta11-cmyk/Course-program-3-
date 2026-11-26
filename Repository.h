#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include "Exceptions.h"

using namespace std;

template <typename T>
class Repository {
private:
    vector<T> items;

public:
    void add(const T& item) { items.push_back(item); }

    void removeById(int id) {
        items.erase(remove_if(items.begin(), items.end(), [id](const T& item) { return item->getId() == id; }), items.end());
    }

    vector<T>& getAll() { return items; }
    const vector<T>& getAll() const { return items; }
    size_t size() const { return items.size(); }

    T search(int id) {
        for (auto& item : items)
            if (item->getId() == id) return item;
        throw NotFoundException("Запись с ID " + to_string(id) + " не найдена");
    }

    template <typename Key>
    vector<T> search(const string& query, Key key) const {
        vector<T> res;
        string q = toLowerAscii(query);
        for (auto& item : items) {
            string field = toLowerAscii(key(item));
            if (field.find(q) != string::npos) res.push_back(item);
        }
        return res;
    }

    template <typename Pred>
    vector<T> filter(Pred pred) const {
        vector<T> res;
        for (auto& x : items) if (pred(x)) res.push_back(x);
        return res;
    }

    template <typename Key>
    void sortBy(Key key) {
        sort(items.begin(), items.end(), [&](const T& a, const T& b) { return key(a) < key(b); });
    }

    T findById(int id) {
        for (auto& item : items)
            if (item->getId() == id) return item;
        return nullptr;
    }

private:
    static string toLowerAscii(const string& s) {
        string r = s;
        for (char& c : r) if (c >= 'A' && c <= 'Z') c += 32;
        return r;
    }
};