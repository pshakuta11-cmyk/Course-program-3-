#pragma once
#include "Models.h"
#include "Repository.h"
#include <memory>

using namespace std;

class Database {
private:
    static Database* instance;
    Database() = default;

    Repository<shared_ptr<Client>> clients;
    Repository<shared_ptr<Property>> properties;
    Repository<shared_ptr<Deal>> deals;

public:
    static Database* getInstance();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    void loadData();
    void saveData();

    Repository<shared_ptr<Client>>& getClients() { return clients; }
    Repository<shared_ptr<Property>>& getProperties() { return properties; }
    Repository<shared_ptr<Deal>>& getDeals() { return deals; }

    int getNextClientId() { return static_cast<int>(clients.size()) + 1; }
    int getNextPropertyId() { return static_cast<int>(properties.size()) + 1; }
    int getNextDealId() { return static_cast<int>(deals.size()) + 1; }

    size_t getClientsCount() const { return clients.size(); }
    size_t getPropertiesCount() const { return properties.size(); }
    size_t getDealsCount() const { return deals.size(); }
};