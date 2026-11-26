#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <iostream>
#include <iomanip>
#include <limits>
#include <memory>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <map>      
#include <windows.h>
#include "Models.h"
#include "Database.h"
#include "Security.h"
#include "Utils.h"

using namespace std;

static const int LINE_PROPS = 125;
static const int LINE_CLIENTS = 105;
static const int LINE_DEALS = 140;
static const int LINE_MENU = 60;

static void printPropertyHeader() {
    cout << left << setw(5) << "ID"
        << setw(14) << "Тип"
        << setw(12) << "Площадь, м2"
        << setw(13) << "Цена, $"
        << setw(24) << "Адрес"
        << setw(8) << "Комнат"
        << setw(14) << "Район"
        << "Статус" << endl;
    Utils::printLine('-', LINE_PROPS);
}

static void printClientHeader() {
    cout << left << setw(5) << "ID"
        << setw(30) << "ФИО"
        << setw(18) << "Телефон"
        << setw(15) << "Паспорт"
        << setw(14) << "Роль"
        << "Логин" << endl;
    Utils::printLine('-', LINE_CLIENTS);
}

static void printDealHeader() {
    cout << left << setw(5) << "ID"
        << setw(40) << "Покупатель (паспорт)"
        << setw(40) << "Продавец (паспорт)"
        << setw(24) << "Адрес"
        << setw(14) << "Дата"
        << "Сумма, $" << endl;
    Utils::printLine('-', LINE_DEALS);
}

static void showAllProperties(bool onlyFree = false, int sellerFilter = -1) {
    printPropertyHeader();
    for (auto& p : Database::getInstance()->getProperties().getAll()) {
        if (onlyFree && p->getStatus() != "свободна") continue;
        if (sellerFilter != -1 && p->getSellerId() != sellerFilter) continue;
        p->printInfo();
    }
    Utils::printLine('-', LINE_PROPS);
}

static void showAllClients(int excludeId = -1) {
    printClientHeader();
    for (auto& c : Database::getInstance()->getClients().getAll()) {
        if (c->getId() == excludeId) continue;
        cout << *c << endl;
    }
    Utils::printLine('-', LINE_CLIENTS);
}

static void showAllDeals() {
    printDealHeader();
    for (auto& d : Database::getInstance()->getDeals().getAll()) d->printDeal();
    Utils::printLine('-', LINE_DEALS);
}

static void searchByAddress() {
    string q = Utils::getLine("Фрагмент адреса: ");
    string qLower = Utils::searchKey(q);
    printPropertyHeader();
    bool found = false;
    for (auto& p : Database::getInstance()->getProperties().getAll()) {
        if (Utils::searchKey(p->getAddress()).find(qLower) != string::npos) {
            p->printInfo();
            found = true;
        }
    }
    if (!found) cout << "Не найдено.\n";
    Utils::printLine('-', LINE_PROPS);
}

static void searchClientByName() {
    string q = Utils::getLine("Фрагмент ФИО: ");
    string qLower = Utils::searchKey(q);
    printClientHeader();
    bool found = false;
    for (auto& c : Database::getInstance()->getClients().getAll()) {
        if (Utils::searchKey(c->getName()).find(qLower) != string::npos) {
            cout << *c << endl;
            found = true;
        }
    }
    if (!found) cout << "Не найдено.\n";
    Utils::printLine('-', LINE_CLIENTS);
}

static void filterByDistrict() {
    string district = Utils::getLine("Район: ");
    string distLower = Utils::searchKey(district);
    printPropertyHeader();
    bool found = false;
    for (auto& p : Database::getInstance()->getProperties().getAll()) {
        auto a = dynamic_pointer_cast<Apartment>(p);
        if (a && Utils::searchKey(a->getDistrict()).find(distLower) != string::npos) {
            p->printInfo();
            found = true;
        }
    }
    if (!found) cout << "Не найдено.\n";
    Utils::printLine('-', LINE_PROPS);
}

static void filterByPriceRange() {
    double lo = Utils::getDoubleInput("Цена от ($): ", 0, 1e12);
    double hi = Utils::getDoubleInput("Цена до ($): ", lo, 1e12);
    printPropertyHeader();
    bool found = false;
    for (auto& p : Database::getInstance()->getProperties().getAll()) {
        if (p->getPrice() >= lo && p->getPrice() <= hi) {
            p->printInfo();
            found = true;
        }
    }
    if (!found) cout << "Не найдено.\n";
    Utils::printLine('-', LINE_PROPS);
}

static void filterByArea() {
    double lo = Utils::getDoubleInput("Площадь от (м2): ", 0, 1e6);
    double hi = Utils::getDoubleInput("Площадь до (м2): ", lo, 1e6);
    printPropertyHeader();
    bool found = false;
    for (auto& p : Database::getInstance()->getProperties().getAll()) {
        if (p->getArea() >= lo && p->getArea() <= hi) {
            p->printInfo();
            found = true;
        }
    }
    if (!found) cout << "Не найдено.\n";
    Utils::printLine('-', LINE_PROPS);
}

static void filterByRooms() {
    int n = Utils::getIntInput("Комнат: ", 1, 20);
    printPropertyHeader();
    bool found = false;
    for (auto& p : Database::getInstance()->getProperties().getAll()) {
        auto a = dynamic_pointer_cast<Apartment>(p);
        if (a && a->getRooms() == n) {
            p->printInfo();
            found = true;
        }
    }
    if (!found) cout << "Не найдено.\n";
    Utils::printLine('-', LINE_PROPS);
}

static void filterByType() {
    int c = Utils::getIntInput("Тип (1-Обычная, 2-Элитная): ", 1, 2);
    printPropertyHeader();
    bool found = false;
    for (auto& p : Database::getInstance()->getProperties().getAll()) {
        bool elite = (dynamic_pointer_cast<EliteApartment>(p) != nullptr);
        if ((c == 2) ? elite : !elite) {
            p->printInfo();
            found = true;
        }
    }
    if (!found) cout << "Не найдено.\n";
    Utils::printLine('-', LINE_PROPS);
}

static void sortPropertiesByPrice() {
    Database::getInstance()->getProperties().sortBy([](shared_ptr<Property> p) { return p->getPrice(); });
    cout << "Отсортировано по цене.\n";
}

static void sortPropertiesByRooms() {
    Database::getInstance()->getProperties().sortBy([](shared_ptr<Property> p) {
        auto a = dynamic_pointer_cast<Apartment>(p);
        return a ? a->getRooms() : 0;
        });
    cout << "Отсортировано по комнатам.\n";
}

static void editProperty(int requesterId, bool isAdmin) {
    int id = Utils::getIntInput("ID квартиры: ", 1, INT_MAX);
    auto prop = Database::getInstance()->getProperties().findById(id);
    if (!prop) { cout << "Не найдено.\n"; return; }
    if (!isAdmin && prop->getSellerId() != requesterId) { cout << "Это не ваша квартира.\n"; return; }
    if (prop->getStatus() == "продана") { cout << "Проданную нельзя редактировать.\n"; return; }

    double price = prop->getPrice();
    if (Utils::readOptionalDouble("Новая цена, $ (Enter — не менять): ", price, 0, 1e12)) prop->setPrice(price);
    double area = prop->getArea();
    if (Utils::readOptionalDouble("Новая площадь, м2 (Enter — не менять): ", area, 0.1, 1e6)) prop->setArea(area);
    string addr = prop->getAddress();
    if (Utils::readOptionalString("Новый адрес (Enter — не менять): ", addr)) prop->setAddress(addr);

    auto apt = dynamic_pointer_cast<Apartment>(prop);
    if (apt) {
        string dist = apt->getDistrict();
        if (Utils::readOptionalString("Новый район (Enter — не менять): ", dist)) apt->setDistrict(dist);
        int r = apt->getRooms();
        if (Utils::readOptionalInt("Комнат (Enter — не менять): ", r, 1, 20)) apt->setRooms(r);
        int f = apt->getFloor();
        if (Utils::readOptionalInt("Этаж (Enter — не менять): ", f, -5, 200)) apt->setFloor(f);
    }
    cout << "Квартира обновлена.\n";
}

static void editClient() {
    int id = Utils::getIntInput("ID клиента: ", 1, INT_MAX);
    auto c = Database::getInstance()->getClients().findById(id);
    if (!c) { cout << "Не найдено.\n"; return; }
    string name = c->getName();
    if (Utils::readOptionalString("Новое ФИО (Enter — не менять): ", name)) c->setName(name);
    string phone = c->getPhone();
    if (Utils::readOptionalValidated("Новый телефон 37529XXXXXXX (Enter — не менять): ", phone, Utils::isValidPhone, "Телефон: ровно 12 цифр, начинается с 375.")) c->setPhone(phone);
    string pass = c->getPassport();
    if (Utils::readOptionalValidated("Новый паспорт ВМХХХХХХХХ (Enter — не менять): ", pass, Utils::isValidPassport, "Паспорт: 2 буквы и 7 цифр, например MP1234567.")) c->setPassport(Utils::toUpper(pass));
    cout << "Клиент обновлён.\n";
}

static void editDeal() {
    int id = Utils::getIntInput("ID сделки: ", 1, INT_MAX);
    auto d = Database::getInstance()->getDeals().findById(id);
    if (!d) { cout << "Не найдено.\n"; return; }
    string date = d->getDate();
    if (Utils::readOptionalString("Новая дата ДД.ММ.ГГГГ (Enter — не менять): ", date)) {
        int key;
        if (Utils::parseDate(date, key)) d->setDate(date);
        else cout << "Неверная дата, оставлена прежняя.\n";
    }
    double price = d->getTotalPrice();
    if (Utils::readOptionalDouble("Новая сумма, $ (Enter — не менять): ", price, 0, 1e12)) d->setTotalPrice(price);
    cout << "Сделка обновлена.\n";
}

static void deleteProperty(int requesterId, bool isAdmin) {
    int id = Utils::getIntInput("ID квартиры: ", 1, INT_MAX);
    auto prop = Database::getInstance()->getProperties().findById(id);
    if (!prop) { cout << "Не найдено.\n"; return; }
    if (!isAdmin && prop->getSellerId() != requesterId) { cout << "Не ваша квартира.\n"; return; }
    if (prop->getStatus() == "продана") { cout << "Нельзя удалить проданную.\n"; return; }
    Database::getInstance()->getProperties().removeById(id);
    cout << "Удалено.\n";
}

static void deleteClient() {
    int id = Utils::getIntInput("ID клиента: ", 1, INT_MAX);
    auto c = Database::getInstance()->getClients().findById(id);
    if (!c) { cout << "Не найдено.\n"; return; }
    for (auto& d : Database::getInstance()->getDeals().getAll())
        if (d->getBuyerId() == id || d->getSellerId() == id) {
            cout << "У клиента есть сделки — сначала удалите их.\n"; return;
        }
    Database::getInstance()->getClients().removeById(id);
    cout << "Клиент удалён.\n";
}

static void addProperty(int sellerId) {
    int id = Database::getInstance()->getNextPropertyId();
    int typeChoice = Utils::getIntInput("Тип (1-Обычная, 2-Элитная): ", 1, 2);
    double area = Utils::getDoubleInput("Площадь (м2): ", 0.1, 1e6);
    double price = Utils::getDoubleInput("Цена ($): ", 0, 1e12);
    string address = Utils::getLine("Адрес: ");
    int rooms = Utils::getIntInput("Комнат: ", 1, 20);
    int floor = Utils::getIntInput("Этаж: ", -5, 200);
    string district = Utils::getLine("Район: ");
    bool parking = false, security = false;
    if (typeChoice == 2) {
        parking = Utils::getIntInput("Парковка (1-Да, 0-Нет): ", 0, 1) == 1;
        security = Utils::getIntInput("Охрана (1-Да, 0-Нет): ", 0, 1) == 1;
    }
    try {
        auto prop = PropertyFactory::createProperty(typeChoice == 2 ? "Elite" : "Apartment", id, sellerId, area, price, address, rooms, floor, district, parking, security);
        Database::getInstance()->getProperties().add(prop);
        cout << "Квартира добавлена, ID = " << id << "\n";
    }
    catch (const InvalidDataException& e) {
        cout << "Ошибка: " << e.what() << "\n";
    }
}

static void createDeal() {
    int buyerId = Utils::getIntInput("ID покупателя: ", 1, INT_MAX);
    auto buyer = Database::getInstance()->getClients().findById(buyerId);
    if (!buyer) { cout << "Покупатель не найден.\n"; return; }
    int propId = Utils::getIntInput("ID квартиры: ", 1, INT_MAX);
    auto prop = Database::getInstance()->getProperties().findById(propId);
    if (!prop) { cout << "Квартира не найдена.\n"; return; }
    if (prop->getStatus() == "продана") { cout << "Уже продана.\n"; return; }
    auto seller = Database::getInstance()->getClients().findById(prop->getSellerId());
    if (!seller) { cout << "Продавец не найден.\n"; return; }
    string date = Utils::getLine("Дата ДД.ММ.ГГГГ: ");
    int key;
    if (!Utils::parseDate(date, key)) { cout << "Неверная дата.\n"; return; }
    int dealId = Database::getInstance()->getNextDealId();
    Database::getInstance()->getDeals().add(make_shared<Deal>(dealId, buyer, seller, prop, date, prop->getPrice()));
    prop->setStatus("продана");
    cout << "Сделка #" << dealId << " оформлена.\n";
}

static void reportDeals() {
    string d1 = Utils::getLine("Дата с ДД.ММ.ГГГГ: ");
    string d2 = Utils::getLine("Дата по ДД.ММ.ГГГГ: ");
    int k1, k2;
    if (!Utils::parseDate(d1, k1) || !Utils::parseDate(d2, k2) || k1 > k2) {
        cout << "Неверный диапазон.\n"; return;
    }

    ofstream out("report.txt");
    if (!out.is_open()) { cout << "Не удалось создать report.txt\n"; return; }
    out << "ID;Дата;Сумма_$;Налог_$\n";

    double total = 0, taxSum = 0;
    int count = 0;

    cout << left << setw(5) << "ID" << setw(14) << "Дата" << setw(15) << "Сумма, $" << setw(15) << "Налог, $" << "Адрес" << endl;
    Utils::printLine('-', LINE_PROPS);

    for (auto& d : Database::getInstance()->getDeals().getAll()) {
        int k;
        if (!Utils::parseDate(d->getDate(), k)) continue;
        if (k < k1 || k > k2) continue;
        double t = d->getProperty()->calculateTax();
        total += d->getTotalPrice();
        taxSum += t;
        count++;
        cout << left << setw(5) << d->getId() << setw(14) << d->getDate() << setw(15) << fixed << setprecision(0) << d->getTotalPrice() << setw(15) << t << d->getProperty()->getAddress() << "\n";
        out << d->getId() << ";" << d->getDate() << ";" << d->getTotalPrice() << ";" << t << "\n";
    }
    Utils::printLine('-', LINE_PROPS);
    cout << "Сделок: " << count << " | Сумма: " << total << " $ | Налог: " << taxSum << " $\n";
    out << "ИТОГО;" << count << ";" << total << ";" << taxSum << "\n";
    out.close();
    cout << "Отчёт сохранён в report.txt\n";
}

static void showPopularDistrict() {
    map<string, int> districtCounts;

    for (auto& d : Database::getInstance()->getDeals().getAll()) {
        auto apt = dynamic_pointer_cast<Apartment>(d->getProperty());
        if (apt) {
            districtCounts[apt->getDistrict()]++;
        }
    }

    if (districtCounts.empty()) {
        cout << "Сделок пока нет, невозможно определить популярный район.\n";
        return;
    }

    string popularDistrict;
    int maxCount = 0;

    for (auto& pair : districtCounts) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            popularDistrict = pair.first;
        }
    }

    cout << "========================================\n";
    cout << " Самый популярный район: " << popularDistrict << "\n";
    cout << " Количество сделок: " << maxCount << "\n";
    cout << "========================================\n";
}

static int readChoice() {
    string s = Utils::readRaw("");
    int v;
    return Utils::tryParseInt(s, v) ? v : -1;
}

static shared_ptr<Client> findByLogin(const string& login) {
    for (auto& c : Database::getInstance()->getClients().getAll())
        if (c->getLogin() == login) return c;
    return nullptr;
}

static shared_ptr<Client> loginExisting(const string& expectedRole) {
    string login = Utils::getLine("Логин: ");
    string pass = Utils::getLine("Пароль: ");
    auto user = findByLogin(login);
    if (!user) { cout << "Пользователь не найден.\n"; return nullptr; }

    if (!Security::verifyPassword(pass, user->getPasswordHash())) {
        cout << "Неверный пароль.\n"; return nullptr;
    }

    if (user->getRole() != expectedRole) {
        cout << "Этот логин принадлежит роли '" << user->getRole() << "', а вы пытаетесь войти как '" << expectedRole << "'.\n";
        return nullptr;
    }
    cout << "Добро пожаловать, " << user->getName() << "!\n";
    return user;
}

static shared_ptr<Client> registerNew(const string& role) {
    string login = Utils::getLine("Придумайте логин: ");
    if (findByLogin(login)) { cout << "Такой логин уже занят.\n"; return nullptr; }
    string pass = Utils::getLine("Придумайте пароль: ");
    string name = Utils::getLine("ФИО: ");
    string phone = Utils::getValidated("Телефон (формат 37529XXXXXXX): ", Utils::isValidPhone, "Телефон: ровно 12 цифр, начинается с 375.");
    string passport = Utils::toUpper(Utils::getValidated("Паспорт (формат ВМХХХХХХХХ): ", Utils::isValidPassport, "Паспорт: 2 буквы и 7 цифр, например MP1234567."));

    string hash = Security::hashPassword(pass);
    int id = Database::getInstance()->getNextClientId();
    auto c = make_shared<Client>(id, name, phone, passport, role, login, hash);
    Database::getInstance()->getClients().add(c);
    cout << "Зарегистрировано, ID = " << id << ". Вход выполнен.\n";
    return c;
}

static shared_ptr<Client> authClient(const string& role) {
    int choice;
    do {
        Utils::printLine('-', LINE_MENU);
        cout << "--- ВХОД: " << role << " ---\n"
            << "1. Уже есть аккаунт\n"
            << "2. Зарегистрироваться\n"
            << "0. Назад\n"
            << "Выбор: ";
        choice = readChoice();
        if (choice == 1) {
            auto u = loginExisting(role);
            if (u) return u;
        }
        else if (choice == 2) {
            auto u = registerNew(role);
            if (u) return u;
        }
        else if (choice == 0) return nullptr;
        else cout << "Неверный выбор.\n";
    } while (true);
}

static void adminMenu(int adminId) {
    int choice;
    do {
        Utils::printLine('-', LINE_MENU);
        cout << "--- АДМИНИСТРАТОР ---\n"
            << "1. Все квартиры\n"
            << "2. Все клиенты\n"
            << "3. Все сделки\n"
            << "4. Добавить квартиру\n"
            << "5. Редактировать квартиру\n"
            << "6. Удалить квартиру\n"
            << "7. Редактировать клиента\n"
            << "8. Удалить клиента\n"
            << "9. Редактировать сделку\n"
            << "10. Оформить сделку\n"
            << "11. Сортировать по цене\n"
            << "12. Фильтр по району\n"
            << "13. Отчёт по сделкам\n"
            << "14. Популярный район\n"
            << "15. Сохранить данные\n"
            << "0. Назад\n"
            << "Выбор: ";
        choice = readChoice();
        try {
            switch (choice) {
            case 1: showAllProperties(); break;
            case 2: showAllClients(adminId); break;
            case 3: showAllDeals(); break;
            case 4: { int sid = Utils::getIntInput("ID продавца: ", 1, INT_MAX); addProperty(sid); break; }
            case 5: editProperty(0, true); break;
            case 6: deleteProperty(0, true); break;
            case 7: editClient(); break;
            case 8: deleteClient(); break;
            case 9: editDeal(); break;
            case 10: createDeal(); break;
            case 11: sortPropertiesByPrice(); break;
            case 12: filterByDistrict(); break;
            case 13: reportDeals(); break;
            case 14: showPopularDistrict(); break;
            case 15: Database::getInstance()->saveData(); cout << "Сохранено.\n"; break;
            case 0: break;
            default: cout << "Неверный выбор.\n";
            }
        }
        catch (const exception& e) { cout << "Ошибка: " << e.what() << "\n"; }
    } while (choice != 0);
}

static void buyerMenu(int buyerId) {
    int choice;
    do {
        Utils::printLine('-', LINE_MENU);
        cout << "--- ПОКУПАТЕЛЬ ---\n"
            << "1. Свободные квартиры\n"
            << "2. Сортировать по цене\n"
            << "3. Сортировать по комнатам\n"
            << "4. Поиск по адресу\n"
            << "5. Фильтр по району\n"
            << "6. Фильтр по цене\n"
            << "7. Фильтр по комнатам\n"
            << "8. Фильтр по типу\n"
            << "9. Забронировать\n"
            << "0. Назад\n"
            << "Выбор: ";
        choice = readChoice();
        try {
            switch (choice) {
            case 1: showAllProperties(true); break;
            case 2: sortPropertiesByPrice(); break;
            case 3: sortPropertiesByRooms(); break;
            case 4: searchByAddress(); break;
            case 5: filterByDistrict(); break;
            case 6: filterByPriceRange(); break;
            case 7: filterByRooms(); break;
            case 8: filterByType(); break;
            case 9: {
                int id = Utils::getIntInput("ID квартиры: ", 1, INT_MAX);
                auto p = Database::getInstance()->getProperties().findById(id);
                if (!p) { cout << "Не найдено.\n"; break; }
                if (p->getStatus() != "свободна") { cout << "Занята.\n"; break; }
                p->setStatus("забронирована");
                cout << "Забронировано.\n";
                break;
            }
            case 0: break;
            default: cout << "Неверный выбор.\n";
            }
        }
        catch (const exception& e) { cout << "Ошибка: " << e.what() << "\n"; }
    } while (choice != 0);
}

static void sellerMenu(int sellerId) {
    int choice;
    do {
        Utils::printLine('-', LINE_MENU);
        cout << "--- ПРОДАВЕЦ ---\n"
            << "1. Добавить квартиру\n"
            << "2. Мои квартиры\n"
            << "3. Редактировать свою квартиру\n"
            << "4. Удалить свою квартиру\n"
            << "5. Сортировать по цене\n"
            << "6. Сортировать по комнатам\n"
            << "7. Поиск по адресу\n"
            << "8. Фильтр по району\n"
            << "9. Фильтр по цене\n"
            << "10. Мои входящие сделки\n"
            << "11. Сохранить данные\n"
            << "0. Назад\n"
            << "Выбор: ";
        choice = readChoice();
        try {
            switch (choice) {
            case 1: addProperty(sellerId); break;
            case 2: showAllProperties(false, sellerId); break;
            case 3: editProperty(sellerId, false); break;
            case 4: deleteProperty(sellerId, false); break;
            case 5: sortPropertiesByPrice(); break;
            case 6: sortPropertiesByRooms(); break;
            case 7: searchByAddress(); break;
            case 8: filterByDistrict(); break;
            case 9: filterByPriceRange(); break;
            case 10: {
                printDealHeader();
                bool any = false;
                for (auto& d : Database::getInstance()->getDeals().getAll())
                    if (d->getSellerId() == sellerId) { d->printDeal(); any = true; }
                if (!any) cout << "Сделок нет.\n";
                Utils::printLine('-', LINE_DEALS);
                break;
            }
            case 11: Database::getInstance()->saveData(); cout << "Сохранено.\n"; break;
            case 0: break;
            default: cout << "Неверный выбор.\n";
            }
        }
        catch (const exception& e) { cout << "Ошибка: " << e.what() << "\n"; }
    } while (choice != 0);
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    try {
        Database::getInstance()->loadData();
    }
    catch (const exception& e) {
        cerr << "Ошибка загрузки: " << e.what() << "\n";
    }

    {
        auto& clients = Database::getInstance()->getClients();
        bool hasAdmin = false;
        for (auto& c : clients.getAll())
            if (c->getRole() == "администратор") { hasAdmin = true; break; }
        if (!hasAdmin) {
            int id = Database::getInstance()->getNextClientId();
            auto admin = make_shared<Client>(id, "Администратор", "375290000000", "ADMIN0000", "администратор", "admin", Security::hashPassword("admin"));
            clients.add(admin);
        }
    }

    int choice;
    try {
        do {
            Utils::printLine('-', LINE_MENU);
            cout << "--- АГЕНТСТВО НЕДВИЖИМОСТИ ---\n"
                << "1. Вход администратора\n"
                << "2. Вход покупателя\n"
                << "3. Вход продавца\n"
                << "0. Выход\n"
                << "Выбор: ";
            choice = readChoice();

            if (choice == 1) {
                auto user = loginExisting("администратор");
                if (user) adminMenu(user->getId());
            }
            else if (choice == 2) {
                auto user = authClient("покупатель");
                if (user) buyerMenu(user->getId());
            }
            else if (choice == 3) {
                auto user = authClient("продавец");
                if (user) sellerMenu(user->getId());
            }
            else if (choice != 0) {
                cout << "Неверный выбор.\n";
            }
        } while (choice != 0);
    }
    catch (const InputClosedException&) {
        cout << "\nВвод закрыт.\n";
    }

    try {
        Database::getInstance()->saveData();
        cout << "Данные сохранены. Выход...\n";
    }
    catch (const exception& e) {
        cerr << "Ошибка сохранения: " << e.what() << "\n";
    }
    return 0;
}