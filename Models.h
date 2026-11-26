#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <sstream>
#include <iomanip>
#include <cstring>
#include "Exceptions.h"

using namespace std;

class Property {
protected:
    int id;
    int sellerId;
    double area;
    double price;
    string address;
    string status;

public:
    Property(int id, int sellerId, double area, double price, string address, string status = "свободна");
    virtual ~Property() = default;

    virtual void printInfo() const = 0;
    virtual double calculateTax() const = 0;
    virtual string getType() const = 0;

    int getId() const { return id; }
    int getSellerId() const { return sellerId; }
    double getArea() const { return area; }
    double getPrice() const { return price; }
    string getAddress() const { return address; }
    string getStatus() const { return status; }

    void setId(int v) { id = v; }
    void setSellerId(int v) { sellerId = v; }
    void setArea(double v) { if (v <= 0) throw InvalidDataException("Площадь должна быть > 0"); area = v; }
    void setPrice(double v) { if (v < 0) throw InvalidDataException("Цена не может быть < 0"); price = v; }
    void setAddress(const string& v) { address = v; }
    void setStatus(const string& v) { status = v; }

    string getPriceString() const {
        stringstream ss;
        ss << fixed << setprecision(0) << price << "$";
        return ss.str();
    }
};

class Apartment : public Property {
protected:
    int roomsCount;
    int floor;
    string district;

public:
    Apartment(int id, int sellerId, double area, double price, string address, int rooms, int floor, string district);

    void printInfo() const override;
    double calculateTax() const override;
    string getType() const override { return "Apartment"; }

    int getRooms() const { return roomsCount; }
    int getFloor() const { return floor; }
    string getDistrict() const { return district; }

    void setRooms(int v) { if (v <= 0) throw InvalidDataException("Комнат должно быть > 0"); roomsCount = v; }
    void setFloor(int v) { floor = v; }
    void setDistrict(const string& v) { district = v; }
};

class EliteApartment : public Apartment {
private:
    bool hasParking;
    bool hasSecurity;

public:
    EliteApartment(int id, int sellerId, double area, double price, string address, int rooms, int floor, string district, bool parking, bool security);

    void printInfo() const override;
    double calculateTax() const override;
    string getType() const override { return "Elite"; }

    bool getParking() const { return hasParking; }
    bool getSecurity() const { return hasSecurity; }
    void setParking(bool v) { hasParking = v; }
    void setSecurity(bool v) { hasSecurity = v; }
};

class Client {
private:
    int clientId;
    char* fullName;
    string phoneNumber;
    string passport;
    string role;
    string login;
    string passwordHash;

public:
    Client(int id, const string& name, const string& phone, const string& passport, const string& role, const string& login = "", const string& hash = "");
    Client(const Client& other);
    Client& operator=(const Client& other);
    ~Client();

    int getId() const { return clientId; }
    string getName() const { return string(fullName); }
    string getPhone() const { return phoneNumber; }
    string getPassport() const { return passport; }
    string getRole() const { return role; }
    string getLogin() const { return login; }
    string getPasswordHash() const { return passwordHash; }

    void setId(int v) { clientId = v; }
    void setName(const string& n) {
        delete[] fullName;
        fullName = new char[n.length() + 1];
        strcpy(fullName, n.c_str());
    }
    void setPhone(const string& p) {
        if (p.size() < 9) throw InvalidDataException("Телефон слишком короткий");
        phoneNumber = p;
    }
    void setPassport(const string& p) {
        if (p.size() < 6) throw InvalidDataException("Паспорт слишком короткий");
        passport = p;
    }
    void setRole(const string& r) { role = r; }
    void setLogin(const string& l) { login = l; }
    void setPasswordHash(const string& h) { passwordHash = h; }

    bool operator<(const Client& other) const;
    friend ostream& operator<<(ostream& os, const Client& c);
    friend istream& operator>>(istream& is, Client& c);
};

class Deal {
private:
    int dealId;
    shared_ptr<Client> buyer;
    shared_ptr<Client> seller;
    shared_ptr<Property> property;
    string date;
    double totalPrice;
    string status;

public:
    Deal(int id, shared_ptr<Client> b, shared_ptr<Client> s, shared_ptr<Property> p, string date, double price);

    void printDeal() const;

    int getId() const { return dealId; }
    shared_ptr<Client> getBuyer() const { return buyer; }
    shared_ptr<Client> getSeller() const { return seller; }
    shared_ptr<Property> getProperty() const { return property; }
    string getDate() const { return date; }
    double getTotalPrice() const { return totalPrice; }
    string getStatus() const { return status; }
    int getBuyerId() const { return buyer ? buyer->getId() : 0; }
    int getSellerId() const { return seller ? seller->getId() : 0; }

    void setDate(const string& d) { date = d; }
    void setTotalPrice(double p) { if (p < 0) throw InvalidDataException("Сумма < 0"); totalPrice = p; }
    void setStatus(const string& s) { status = s; }

    friend ostream& operator<<(ostream& os, const Deal& d);
    friend istream& operator>>(istream& is, Deal& d);
};

class PropertyFactory {
public:
    static shared_ptr<Property> createProperty(const string& type, int id, int sellerId, double area, double price, const string& address, int rooms, int floor, const string& district, bool parking = false, bool security = false);
};