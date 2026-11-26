#include <sstream>
#include <iomanip>
#include "Security.h"

using namespace std;

string Security::hashPassword(const string& password) {
    unsigned long long h = 1469598103934665603ULL;
    for (unsigned char c : password) {
        h ^= c;
        h *= 1099511628211ULL;
    }
    ostringstream ss;
    ss << hex << setw(16) << setfill('0') << h;
    return ss.str();
}

bool Security::verifyPassword(const string& password, const string& expectedHash) {
    return hashPassword(password) == expectedHash;
}