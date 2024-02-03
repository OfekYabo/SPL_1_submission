#pragma once
#include <string>
#include <vector>
#include "Order.h"
#include "Customer.h"
#include "../include/Volunteer.h"
#include "../include/Action.h"

class BaseAction;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.


class WareHouse {

    public:
        WareHouse(const string &configFilePath);
        void start();
        void step();
        void addOrder(Order* order);
        void addAction(BaseAction* action);
        void addVolunteer(Volunteer* volunteer);
        void addCustomer(Customer* customer);
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        Volunteer &getVolunteerByIndex(int index) const;
        Order &getOrder(int orderId) const;
        bool isOrderExist(int orderId) const;
        bool isCustomerExist(int customerId) const;
        int isVolunteerExist(int volunteerId) const;
        const vector<BaseAction*> &getActions() const;
        int getOrderCounter() const;
        int getVolunteerCounter() const;
        int getCustomerCounter() const;
        void close();
        void open();
        // Rule of Five:
        ~WareHouse(); // Destructor
        WareHouse(const WareHouse& other); // Copy Constructor
        WareHouse& operator=(const WareHouse& other); // Copy Assignment Operator
        WareHouse(WareHouse&& other) noexcept; // Move Constructor
        WareHouse& operator=(WareHouse&& other) noexcept; // Move Assignment Operator

    private:
        bool isOpen;
        vector<BaseAction*> actionsLog;
        vector<Volunteer*> volunteers;
        vector<Order*> pendingOrders;
        vector<Order*> inProcessOrders;
        vector<Order*> completedOrders;
        vector<Customer*> customers;
        int customerCounter; //For assigning unique customer IDs
        int volunteerCounter; //For assigning unique volunteer IDs
        int orderCounter; //For assigning unique order IDs
        bool deleteVolunteer(Volunteer* volunteer);
        void printClose(const Order& order);
        void freeResources();
};