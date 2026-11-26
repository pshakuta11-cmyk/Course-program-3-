#define _CRT_SECURE_NO_WARNINGS
#include "Models.h"
#include <sstream>
#include <iomanip>
#include <cstring>

using namespace std;

Property::Property(int id, int sellerId, double area, double price, string address, string status)
    : id(id), sellerId(sellerId), area(area), price(price), address(address), status(status) {
    if (price < 0)  throw InvalidDataException("Цена не может быть отрицательной!");
    if (area <= 0)  throw InvalidDataException("Площадь должна быть положительной!");
}

Apartment::Apartment(int id, int sellerId, double area, double price, string address, int rooms, int floor, string district)
    : Property(id, sellerId, area, price, address), roomsCount(rooms), floor(floor), district(district) {
    if (rooms <= 0) throw InvalidDataException("Количество комнат должно быть > 0!");
}

void Apartment::printInfo() const {
    cout << left << setw(5) << id << setw(14) << "Квартира" << setw(12) << fixed << setprecision(1) << area << setw(13) << fixed << setprecision(0) << price << setw(24) << address << setw(8) << roomsCount << setw(14) << district << status << endl;
}

double Apartment::calculateTax() const {
    return price * 0.01;
}

EliteApartment::EliteApartment(int id, int sellerId, double area, double price, string address, int rooms, int floor, string district, bool parking, bool security)
    : Apartment(id, sellerId, area, price, address, rooms, floor, district), hasParking(parking), hasSecurity(security) {
}

void EliteApartment::printInfo() const {
    cout << left << setw(5) << id << setw(14) << "Элитная кв." << setw(12) << fixed << setprecision(1) << area << setw(13) << fixed << setprecision(0) << price << setw(24) << address << setw(8) << roomsCount << setw(14) << district << status << endl;
}

double EliteApartment::calculateTax() const {
    return price * 0.02;
}

Client::Client(int id, const string& name, const string& phone, const string& passport, const string& role, const string& login, const string& hash)
    : clientId(id), phoneNumber(phone), passport(passport), role(role), login(login), passwordHash(hash) {
    fullName = new char[name.length() + 1];
    strcpy(fullName, name.c_str());
}

Client::Client(const Client& other)
    : clientId(other.clientId), phoneNumber(other.phoneNumber), passport(other.passport), role(other.role), login(other.login), passwordHash(other.passwordHash) {
    fullName = new char[strlen(other.fullName) + 1];
    strcpy(fullName, other.fullName);
}

Client& Client::operator=(const Client& other) {
    if (this != &other) {
        delete[] fullName;
        clientId = other.clientId;
        phoneNumber = other.phoneNumber;
        passport = other.passport;
        role = other.role;
        login = other.login;
        passwordHash = other.passwordHash;
        fullName = new char[strlen(other.fullName) + 1];
        strcpy(fullName, other.fullName);
    }
    return *this;
}

Client::~Client() {
    delete[] fullName;
}

bool Client::operator<(const Client& other) const {
    return string(fullName) < string(other.fullName);
}

ostream& operator<<(ostream& os, const Client& c) {
    os << left << setw(5) << c.clientId << setw(30) << c.fullName << setw(18) << c.phoneNumber << setw(15) << c.passport << setw(14) << c.role << setw(20) << c.login;
    return os;
}

istream& operator>>(istream& is, Client& c) {
    string line;
    if (!getline(is, line)) return is;

    stringstream ss(line);
    string sid, name, phone, passport, role, login, hash;
    bool ok = getline(ss, sid, ';') && getline(ss, name, ';') && getline(ss, phone, ';') && getline(ss, passport, ';') && getline(ss, role, ';') && getline(ss, login, ';') && getline(ss, hash, ';');
    if (!ok) { is.setstate(ios::failbit); return is; }

    try {
        c = Client(stoi(sid), name, phone, passport, role, login, hash);
    }
    catch (...) {
        is.setstate(ios::failbit);
    }
    return is;
}

Deal::Deal(int id, shared_ptr<Client> b, shared_ptr<Client> s, shared_ptr<Property> p, string date, double price)
    : dealId(id), buyer(b), seller(s), property(p), date(date), totalPrice(price), status("активна") {
}

void Deal::printDeal() const {
    string buyerInfo = buyer ? (buyer->getName() + " (" + buyer->getPassport() + ")") : "?";
    string sellerInfo = seller ? (seller->getName() + " (" + seller->getPassport() + ")") : "?";

    cout << left << setw(5) << dealId << setw(40) << buyerInfo << setw(40) << sellerInfo << setw(24) << (property ? property->getAddress() : "?") << setw(14) << date << setw(13) << fixed << setprecision(0) << totalPrice << endl;
}

ostream& operator<<(ostream& os, const Deal& d) {
    os << left << setw(5) << d.dealId << setw(20) << (d.property ? d.property->getAddress() : "None") << setw(15) << d.date << setw(10) << d.totalPrice;
    return os;
}

istream& operator>>(istream& is, Deal& d) {
    string line;
    getline(is, line);
    return is;
}

shared_ptr<Property> PropertyFactory::createProperty(const string& type, int id, int sellerId, double area, double price, const string& address, int rooms, int floor, const string& district, bool parking, bool security) {
    if (type == "Elite")
        return make_shared<EliteApartment>(id, sellerId, area, price, address, rooms, floor, district, parking, security);
    return make_shared<Apartment>(id, sellerId, area, price, address, rooms, floor, district);
}