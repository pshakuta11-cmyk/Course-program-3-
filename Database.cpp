#define _CRT_SECURE_NO_WARNINGS
#include "Database.h"
#include "Security.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;

Database* Database::instance = nullptr;

Database* Database::getInstance() {
    if (!instance) instance = new Database();
    return instance;
}

static string trim(const string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

void Database::loadData() {
    ifstream fin("clients.txt");
    if (fin.is_open()) {
        string line;
        int skip = 2;
        while (getline(fin, line)) {
            if (skip > 0) { skip--; continue; }
            if (line.empty() || line[0] == '-') continue;

            try {
                int id = stoi(trim(line.substr(0, 4)));
                string name = trim(line.substr(5, 29));
                string phone = trim(line.substr(35, 17));
                string passport = trim(line.substr(53, 14));
                string role = trim(line.substr(68, 13));
                string login = trim(line.substr(82, 19));
                string hash = trim(line.substr(102));

                clients.add(make_shared<Client>(id, name, phone, passport, role, login, hash));
            }
            catch (...) { continue; }
        }
        fin.close();
    }

    ifstream fprop("properties.txt");
    if (fprop.is_open()) {
        string line;
        int skip = 2;
        while (getline(fprop, line)) {
            if (skip > 0) { skip--; continue; }
            if (line.empty() || line[0] == '-') continue;

            try {
                int id = stoi(trim(line.substr(0, 4)));
                int sellerId = stoi(trim(line.substr(5, 13)));
                string type = trim(line.substr(19, 13));
                double area = stod(trim(line.substr(33, 11)));
                double price = stod(trim(line.substr(45, 12)));
                string address = trim(line.substr(58, 23));
                int rooms = stoi(trim(line.substr(82, 7)));
                int floor = stoi(trim(line.substr(90, 13)));
                string district = trim(line.substr(104, 13));
                string status = trim(line.substr(118, 20));
                bool parking = trim(line.substr(139, 8)) == "Да";
                bool security = trim(line.substr(148, 8)) == "Да";

                auto prop = PropertyFactory::createProperty(type, id, sellerId, area, price, address, rooms, floor, district, parking, security);
                prop->setStatus(status);
                properties.add(prop);
            }
            catch (...) { continue; }
        }
        fprop.close();
    }

    ifstream fdeal("deals.txt");
    if (fdeal.is_open()) {
        string line;
        int skip = 2;
        while (getline(fdeal, line)) {
            if (skip > 0) { skip--; continue; }
            if (line.empty() || line[0] == '-') continue;

            try {
                int id = stoi(trim(line.substr(0, 4)));
                int buyerId = stoi(trim(line.substr(5, 13)));
                int sellerId = stoi(trim(line.substr(19, 13)));
                int propId = stoi(trim(line.substr(33, 13)));
                string date = trim(line.substr(47, 14));
                double price = stod(trim(line.substr(62)));

                auto buyer = clients.findById(buyerId);
                auto seller = clients.findById(sellerId);
                auto prop = properties.findById(propId);
                if (buyer && seller && prop) {
                    deals.add(make_shared<Deal>(id, buyer, seller, prop, date, price));
                }
            }
            catch (...) { continue; }
        }
        fdeal.close();
    }
}

void Database::saveData() {
    ofstream fout("clients.txt");
    if (!fout.is_open()) throw FileException("Не удалось открыть clients.txt");
    fout << left << setw(5) << "ID" << setw(30) << "ФИО" << setw(18) << "Телефон" << setw(15) << "Паспорт" << setw(14) << "Роль" << setw(20) << "Логин" << "Хеш\n";
    fout << string(120, '-') << "\n";
    for (const auto& c : clients.getAll()) {
        fout << left << setw(5) << c->getId() << setw(30) << c->getName() << setw(18) << c->getPhone() << setw(15) << c->getPassport() << setw(14) << c->getRole() << setw(20) << c->getLogin() << c->getPasswordHash() << "\n";
    }
    fout.close();
    if (fout.fail()) throw FileException("Ошибка записи clients.txt");

    ofstream fprop("properties.txt");
    if (!fprop.is_open()) throw FileException("Не удалось открыть properties.txt");
    fprop << left << setw(5) << "ID" << setw(14) << "ID_Продавца" << setw(14) << "Тип" << setw(12) << "Площадь" << setw(13) << "Цена" << setw(24) << "Адрес" << setw(8) << "Комнат" << setw(14) << "Этаж" << setw(14) << "Район" << setw(21) << "Статус" << setw(9) << "Парк." << "Охр.\n";
    fprop << string(150, '-') << "\n";
    for (const auto& p : properties.getAll()) {
        auto apt = dynamic_pointer_cast<Apartment>(p);
        auto elite = dynamic_pointer_cast<EliteApartment>(p);
        if (!apt) continue;
        fprop << left << setw(5) << p->getId() << setw(14) << p->getSellerId() << setw(14) << p->getType() << setw(12) << fixed << setprecision(1) << p->getArea() << setw(13) << fixed << setprecision(0) << p->getPrice() << setw(24) << p->getAddress() << setw(8) << apt->getRooms() << setw(14) << apt->getFloor() << setw(14) << apt->getDistrict() << setw(21) << p->getStatus() << setw(9) << (elite && elite->getParking() ? "Да" : "Нет") << (elite && elite->getSecurity() ? "Да" : "Нет") << "\n";
    }
    fprop.close();
    if (fprop.fail()) throw FileException("Ошибка записи properties.txt");

    ofstream fdeal("deals.txt");
    if (!fdeal.is_open()) throw FileException("Не удалось открыть deals.txt");

    fdeal << left << setw(5) << "ID"
        << setw(14) << "ID_Покуп."
        << setw(14) << "ID_Прод."
        << setw(14) << "ID_Кварт."
        << setw(15) << "Дата"
        << "Сумма($)\n";
    fdeal << string(80, '-') << "\n";

    for (const auto& d : deals.getAll()) {
        fdeal << left << setw(5) << d->getId()
            << setw(14) << d->getBuyerId()
            << setw(14) << d->getSellerId()
            << setw(14) << d->getProperty()->getId()
            << setw(15) << d->getDate()
            << fixed << setprecision(0) << d->getTotalPrice() << "\n";
    }
    fdeal.close();
    if (fdeal.fail()) throw FileException("Ошибка записи deals.txt");
}