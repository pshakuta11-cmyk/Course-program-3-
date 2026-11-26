#pragma once
#include <limits>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <climits>
#include <ctime>
#include <cstdio>
#include "Exceptions.h"

namespace Utils {

    inline std::u32string decode(const std::string& s) {
        std::u32string out;
        for (size_t i = 0; i < s.size();) {
            unsigned char c = static_cast<unsigned char>(s[i]);
            char32_t cp = c;
            int len = 1;
            if (c >= 0xF0) { cp = c & 0x07; len = 4; }
            else if (c >= 0xE0) { cp = c & 0x0F; len = 3; }
            else if (c >= 0xC0) { cp = c & 0x1F; len = 2; }
            for (int k = 1; k < len && i + k < s.size(); ++k)
                cp = (cp << 6) | (static_cast<unsigned char>(s[i + k]) & 0x3F);
            out.push_back(cp);
            i += len;
        }
        return out;
    }

    inline std::string encode(const std::u32string& s) {
        std::string out;
        for (char32_t cp : s) {
            if (cp < 0x80) out += static_cast<char>(cp);
            else if (cp < 0x800) {
                out += static_cast<char>(0xC0 | (cp >> 6));
                out += static_cast<char>(0x80 | (cp & 0x3F));
            }
            else if (cp < 0x10000) {
                out += static_cast<char>(0xE0 | (cp >> 12));
                out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                out += static_cast<char>(0x80 | (cp & 0x3F));
            }
            else {
                out += static_cast<char>(0xF0 | (cp >> 18));
                out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
                out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                out += static_cast<char>(0x80 | (cp & 0x3F));
            }
        }
        return out;
    }

    inline std::string toLower(const std::string& s) {
        std::string r = s;
        for (char& ch : r) {
            unsigned char c = static_cast<unsigned char>(ch);
            if (c >= 'A' && c <= 'Z') c += 32;
            else if (c >= 0xC0 && c <= 0xDF) c += 32;
            else if (c == 0xA8) c = 0xB8;
            ch = static_cast<char>(c);
        }
        return r;
    }

    inline std::string searchKey(const std::string& s) {
        std::string r;
        for (char ch : toLower(s)) {
            unsigned char c = static_cast<unsigned char>(ch);
            if (c == ' ' || c == '\t') continue;
            if (c == 0xB8) c = 0xE5;
            r += static_cast<char>(c);
        }
        return r;
    }

    inline std::string trim(const std::string& s) {
        const char* ws = " \t\r\n";
        size_t b = s.find_first_not_of(ws);
        if (b == std::string::npos) return "";
        size_t e = s.find_last_not_of(ws);
        return s.substr(b, e - b + 1);
    }

    inline std::string sanitize(std::string s) {
        for (char& c : s) if (c == ';' || c == '\n' || c == '\r' || c == '\t') c = ',';
        return s;
    }

    inline bool containsText(const std::string& haystack, const std::string& needle) {
        return toLower(haystack).find(toLower(needle)) != std::string::npos;
    }


    inline bool isLetter(unsigned char c) {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c >= 0xC0 || c == 0xA8 || c == 0xB8;
    }

    inline bool isDigit(unsigned char c) {
        return c >= '0' && c <= '9';
    }

    inline std::string toUpper(const std::string& s) {
        std::string r = s;
        for (char& ch : r) {
            unsigned char c = static_cast<unsigned char>(ch);
            if (c >= 'a' && c <= 'z') c -= 32;
            else if (c >= 0xE0) c -= 32;
            else if (c == 0xB8) c = 0xA8;
            ch = static_cast<char>(c);
        }
        return r;
    }

    inline bool isValidPhone(const std::string& s) {
        if (s.size() != 12 || s.compare(0, 3, "375") != 0) return false;
        for (unsigned char c : s) if (!isDigit(c)) return false;
        return true;
    }

    inline bool isValidPassport(const std::string& s) {
        if (s.size() != 9) return false;
        if (!isLetter(s[0]) || !isLetter(s[1])) return false;
        for (size_t i = 2; i < 9; ++i) if (!isDigit(s[i])) return false;
        return true;
    }

    inline std::string pad(const std::string& s, size_t width) {
        std::u32string u = decode(s);
        if (u.size() > width) {
            u.resize(width > 1 ? width - 1 : 0);
            u.push_back(U'.');
        }
        std::string out = encode(u);
        out.append(width - u.size(), ' ');
        return out;
    }

    inline std::string fmt(double v, int precision = 2) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(precision) << v;
        return ss.str();
    }

    inline std::vector<std::string> split(const std::string& line, char delim = ';') {
        std::vector<std::string> parts;
        std::string cur;
        for (char c : line) {
            if (c == delim) { parts.push_back(cur); cur.clear(); }
            else cur += c;
        }
        parts.push_back(cur);
        return parts;
    }

    inline void printLine(char c = '-', int length = 100, std::ostream& os = std::cout) {
        os << std::string(length, c) << "\n";
    }

    inline bool tryParseInt(const std::string& s, int& v) {
        try { size_t p = 0; v = std::stoi(s, &p); return p == s.size(); }
        catch (...) { return false; }
    }

    inline bool tryParseDouble(std::string s, double& v) {
        for (char& c : s) if (c == ',') c = '.';
        try { size_t p = 0; v = std::stod(s, &p); return p == s.size(); }
        catch (...) { return false; }
    }

    inline bool parseDate(const std::string& s, int& key, std::string* normalized = nullptr) {
        size_t p1 = s.find('.');
        size_t p2 = (p1 == std::string::npos) ? std::string::npos : s.find('.', p1 + 1);
        if (p1 == std::string::npos || p2 == std::string::npos) return false;
        int d = 0, m = 0, y = 0;
        std::string ds = s.substr(0, p1), ms = s.substr(p1 + 1, p2 - p1 - 1), ys = s.substr(p2 + 1);
        auto digitsOnly = [](const std::string& t, size_t maxLen) {
            if (t.empty() || t.size() > maxLen) return false;
            for (char c : t) if (c < '0' || c > '9') return false;
            return true;
            };
        if (!digitsOnly(ds, 2) || !digitsOnly(ms, 2) || ys.size() != 4 || !digitsOnly(ys, 4)) return false;
        d = std::stoi(ds); m = std::stoi(ms); y = std::stoi(ys);
        if (y < 1900 || y > 2100 || m < 1 || m > 12 || d < 1) return false;
        static const int dim[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
        int maxD = dim[m - 1];
        bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
        if (m == 2 && leap) maxD = 29;
        if (d > maxD) return false;
        key = y * 10000 + m * 100 + d;
        if (normalized) {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%02d.%02d.%04d", d, m, y);
            *normalized = buf;
        }
        return true;
    }

    inline std::string today() {
        std::time_t t = std::time(nullptr);
        std::tm tmv{};
#ifdef _WIN32
        localtime_s(&tmv, &t);
#else
        localtime_r(&t, &tmv);
#endif
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%02d.%02d.%04d",
            tmv.tm_mday, tmv.tm_mon + 1, tmv.tm_year + 1900);
        return buf;
    }

    inline std::string readRaw(const std::string& prompt) {
        std::cout << prompt << std::flush;
        std::string line;
        if (!std::getline(std::cin, line)) throw InputClosedException();
        return trim(line);
    }

    inline std::string getLine(const std::string& prompt, bool allowEmpty = false) {
        while (true) {
            std::string s = readRaw(prompt);
            if (!s.empty() || allowEmpty) return sanitize(s);
            std::cout << "Поле не может быть пустым.\n";
        }
    }

    inline int getIntInput(const std::string& prompt, int lo = INT_MIN, int hi = INT_MAX) {
        while (true) {
            std::string s = readRaw(prompt);
            int v;
            if (tryParseInt(s, v) && v >= lo && v <= hi) return v;
            if (lo == INT_MIN && hi == INT_MAX) std::cout << "Ошибка! Нужно ввести целое число.\n";
            else std::cout << "Ошибка! Введите целое число от " << lo << " до " << hi << ".\n";
        }
    }

    inline double getDoubleInput(const std::string& prompt, double lo, double hi) {
        while (true) {
            std::string s = readRaw(prompt);
            double v;
            if (tryParseDouble(s, v) && v >= lo && v <= hi) return v;
            std::cout << "Ошибка! Введите число от " << lo << " до " << hi << ".\n";
        }
    }

    inline bool readOptionalInt(const std::string& prompt, int& v, int lo, int hi) {
        while (true) {
            std::string s = readRaw(prompt);
            if (s.empty()) return false;
            int x;
            if (tryParseInt(s, x) && x >= lo && x <= hi) { v = x; return true; }
            std::cout << "Ошибка! Введите целое число от " << lo << " до " << hi << " или Enter.\n";
        }
    }

    inline bool readOptionalDouble(const std::string& prompt, double& v, double lo, double hi) {
        while (true) {
            std::string s = readRaw(prompt);
            if (s.empty()) return false;
            double x;
            if (tryParseDouble(s, x) && x >= lo && x <= hi) { v = x; return true; }
            std::cout << "Ошибка! Введите число от " << lo << " до " << hi << " или Enter.\n";
        }
    }

    inline bool readOptionalString(const std::string& prompt, std::string& v) {
        std::string s = readRaw(prompt);
        if (s.empty()) return false;
        v = sanitize(s);
        return true;
    }
    template <typename Check>
    inline std::string getValidated(const std::string& prompt, Check check, const std::string& message) {
        while (true) {
            std::string s = getLine(prompt);
            if (check(s)) return s;
            std::cout << message << "\n";
        }
    }

    template <typename Check>
    inline bool readOptionalValidated(const std::string& prompt, std::string& v, Check check, const std::string& message) {
        while (true) {
            std::string s = sanitize(readRaw(prompt));
            if (s.empty()) return false;
            if (check(s)) { v = s; return true; }
            std::cout << message << "\n";
        }
    }

    inline void clearInput() {
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
}