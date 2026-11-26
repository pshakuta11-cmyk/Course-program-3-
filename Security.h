#pragma once
#include <string>

using namespace std;

namespace Security {
    string hashPassword(const string& password);
    bool verifyPassword(const string& password, const string& expectedHash);
}