#pragma once
#include <stdexcept>
#include <string>

using namespace std;

class InvalidDataException : public runtime_error {
public:
    explicit InvalidDataException(const string& msg) : runtime_error(msg) {}
};

class FileException : public runtime_error {
public:
    explicit FileException(const string& msg) : runtime_error(msg) {}
};

class NotFoundException : public runtime_error {
public:
    explicit NotFoundException(const string& msg) : runtime_error(msg) {}
};

class AuthException : public runtime_error {
public:
    explicit AuthException(const string& msg) : runtime_error(msg) {}
};

class InputClosedException : public runtime_error {
public:
    InputClosedException() : runtime_error("¬вод закрыт") {}
};