
#include "../include/WareHouse.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

WareHouse::WareHouse(const string &configFilePath) 
: isOpen(true), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(),
completedOrders(), customers(), customerCounter(0), volunteerCounter(0), orderCounter(0){
    std::ifstream file(configFilePath);
    std::string line;
    while (std::getline(file, line)) 
    {
        if (line[0] != '#' && !line.empty()) 
        {
            std::istringstream iss(line);
            std::string type;
            iss >> type;
            if (type == "customer") 
            {
                Customer* customer;
                std::string name, customerType;
                int distance, maxOrders;
                iss >> name >> customerType >> distance >> maxOrders;
                if (customerType == "soldier") {
                    customer = new SoldierCustomer(customerCounter, name, distance, maxOrders);
                } else{
                    customer = new CivilianCustomer(customerCounter, name, distance, maxOrders);
                }
                addCustomer(customer);
            } 
            else if (type == "volunteer") 
            {
                Volunteer* volunteer;
                std::string name, role;
                int coolDown_maxDistance, maxOrders, distancePerStep;
                iss >> name >> role >> coolDown_maxDistance;
                if (role.find("driver") != std::string::npos)
                {
                    iss >> distancePerStep;
                    if (role.find("limited") != std::string::npos)
                    {
                        iss >> maxOrders;
                        volunteer = new LimitedDriverVolunteer(volunteerCounter, name, coolDown_maxDistance, distancePerStep, maxOrders);
                    }
                    else
                    {
                        volunteer = new DriverVolunteer(volunteerCounter, name, coolDown_maxDistance, distancePerStep);
                    }
                } else {
                    if (role.find("limited") != std::string::npos)
                    {
                        iss >> maxOrders;
                        volunteer = new LimitedCollectorVolunteer(volunteerCounter, name, coolDown_maxDistance, maxOrders);
                    }
                    else
                    {
                        volunteer = new CollectorVolunteer(volunteerCounter, name, coolDown_maxDistance);
                    }
                }
                addVolunteer(volunteer);
            }
        }
    }
}

void WareHouse::start() {
    isOpen = true;
    std::cout << "WareHouse is open!" << std::endl;
    std::string line;

    while (isOpen && std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        BaseAction* action;
        if (command == "step") {
            int numOfSteps;
            iss >> numOfSteps;
            action = new SimulateStep(numOfSteps);
        } else if (command == "order") {
            int customerId;
            iss >> customerId;
            action = new AddOrder(customerId);
        } else if (command == "customer") {
            std::string customerName, customerType;
            int distance, maxOrders;
            iss >> customerName >> customerType >> distance >> maxOrders;
            action = new AddCustomer(customerName, customerType, distance, maxOrders);
        } else if (command == "orderStatus") {
            int orderId;
            iss >> orderId;
            action = new PrintOrderStatus(orderId);
        } else if (command == "customerStatus") {
            int customerId;
            iss >> customerId;
            action = new PrintCustomerStatus(customerId);
        } else if (command == "volunteerStatus") {
            int volunteerId;
            iss >> volunteerId;
            action = new PrintVolunteerStatus(volunteerId);
        } else if (command == "log") {
            action = new PrintActionsLog();
        } else if (command == "close") {
            action = new Close();
            action->act(*this);
            delete (action);
            continue;//start the loop again
        } else if (command == "backup") {
            action = new BackupWareHouse();
        } else if (command == "restore") {
            action = new RestoreWareHouse();
        } else {
            std::cout << "Illegal Input- Try Again"<< std::endl;
            continue;//start the loop again
        }
        action->act(*this);
        addAction(action);
    }
}
void WareHouse::step() {
    auto it = pendingOrders.begin();
    while (it != pendingOrders.end()) {
        bool orderProcessed = false;
        for (Volunteer* volunteer : volunteers) {
            if (volunteer->canTakeOrder(**it)) {
                volunteer->acceptOrder(**it);
                if ((**it).getStatus() == OrderStatus::PENDING) {
                    (**it).setStatus(OrderStatus::COLLECTING);
                    (**it).setCollectorId(volunteer->getId());
                } else if ((**it).getStatus() == OrderStatus::COLLECTING) {
                    (**it).setStatus(OrderStatus::DELIVERING);
                    (**it).setDriverId(volunteer->getId());
                }
                inProcessOrders.push_back(*it);
                it = pendingOrders.erase(it); // Move iterator to the next valid position after erasing
                orderProcessed = true;
                break;
            }
        }
        if (!orderProcessed) {
            ++it; // Move to the next order only if it was not processed
        }
    }
    for (Volunteer* volunteer : volunteers){
        if (volunteer->isBusy()){
            volunteer->step();
            if (!volunteer->isBusy()){
                Order& order = getOrder(volunteer->getCompletedOrderId());//orders not delete so no need to check exist
                std::vector<Order*>::iterator it = std::find(inProcessOrders.begin(), inProcessOrders.end(), &order);
                switch(order.getStatus()){
                    case OrderStatus::COLLECTING:
                        if (it != inProcessOrders.end()) {
                            pendingOrders.push_back(&order);
                            inProcessOrders.erase(it);
                        }
                        break;
                    case OrderStatus::DELIVERING:
                        order.setStatus(OrderStatus::COMPLETED);
                        if (it != inProcessOrders.end()) {
                            completedOrders.push_back(&order);
                            inProcessOrders.erase(it);
                        }
                        break;
                    case OrderStatus::PENDING:
                        break;
                    case OrderStatus::COMPLETED:
                        break;
                }
                if (!volunteer->hasOrdersLeft()){
                    deleteVolunteer(volunteer);
                }
            }
        }
    }
}



void WareHouse::addOrder(Order* order) {
    orderCounter++;
    pendingOrders.push_back(order);
}

void WareHouse::addVolunteer(Volunteer* volunteer) {
    volunteerCounter++;
    volunteers.push_back(volunteer);
}

void WareHouse::addCustomer(Customer* customer) {
    customerCounter++;
    customers.push_back(customer);
}

void WareHouse::addAction(BaseAction* action) {
    actionsLog.push_back(action);
}


 bool WareHouse::deleteVolunteer(Volunteer* volunteer) {
    for (auto it = volunteers.begin(); it != volunteers.end(); ++it) {
        if (*it == volunteer) {
            delete *it;
            volunteers.erase(it);
            return true;
        }
    }
    return false;
}

//use before using getVolunteerByIndex
//-1 if not exist, else return index
int WareHouse::isVolunteerExist(int volunteerId) const {
    if(volunteerCounter<=volunteerId || volunteerId<0) {return -1;}
    for (std::vector<Volunteer*>::size_type i = 0; i < volunteers.size(); i++) {
        if (volunteers[i]->getId() == volunteerId) {
            return i;
        }
    }
    return -1;
}

//use before using getCustomer
bool WareHouse::isOrderExist(int orderId) const{
    if(orderCounter<=orderId || orderId<0){
        return false;
    }
    return true;
}

//use before using getCustomer
bool WareHouse::isCustomerExist(int customerId) const{
    if(customerCounter<=customerId || customerId<0) {
        return false;
    }
    return true;
}

//use isCustomerExist before using this function
Customer& WareHouse::getCustomer(int customerId) const {
    return *customers[customerId];
}

Volunteer& WareHouse::getVolunteer(int volunteerId) const {
    int index = isVolunteerExist(volunteerId);
    if (index == -1) {
        throw std::invalid_argument("Volunteer doesn't exist");
    }
    return *volunteers[index];
}

//use isVolunteerExist before using this function
Volunteer& WareHouse::getVolunteerByIndex(int index) const {
    return *volunteers[index];
}

//use isOrderExist before using this function
Order& WareHouse::getOrder(int orderId) const {
    for (Order* order : pendingOrders) {
        if (order->getId()==orderId) {
            return *order;
        }
    }
    for (Order* order : inProcessOrders) {
        if (order->getId()==orderId) {
            return *order;
        }
    }
    for (Order* order : completedOrders) {
        if (order->getId()==orderId) {
            return *order;
        }
    }
    throw std::invalid_argument("Order doesn't exist");
}

const vector<BaseAction *>& WareHouse::getActions() const { return actionsLog; }

int WareHouse::getOrderCounter() const {return orderCounter;}

int WareHouse::getVolunteerCounter() const {return volunteerCounter;}

int WareHouse::getCustomerCounter() const {return customerCounter;}

void WareHouse::printClose(const Order& order){
    std::cout << "OrderID: " << std::to_string(order.getId()) 
    << ", CustomerID: " << std::to_string(order.getCustomerId())
    << ", OrderStatus: " << OrderStatusToString.at(order.getStatus()) << std::endl;
}
void WareHouse::close() {
    for (Order* order : pendingOrders) {
        printClose(*order);
    }
    for (Order* order : inProcessOrders) {
        printClose(*order);
    }
    for (Order* order : completedOrders) {
        printClose(*order);
    }
    isOpen = false;
}

void WareHouse::open() {isOpen = true;}

//
//rule of five
//

void WareHouse::freeResources(){
// Free all resources and clear vectors
    for (auto action : actionsLog) delete action;
    actionsLog.clear();
    for (auto volunteer : volunteers) delete volunteer;
    volunteers.clear();
    for (auto order : pendingOrders) delete order;
    pendingOrders.clear();
    for (auto order : inProcessOrders) delete order;
    inProcessOrders.clear();
    for (auto order : completedOrders) delete order;
    completedOrders.clear();
    for (auto customer : customers) delete customer;
    customers.clear();
}

WareHouse::~WareHouse() {
    freeResources();
}

WareHouse::WareHouse(const WareHouse& other) 
    : isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(),
    customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter){
        // Deep copy of pointers in vectors
        for (BaseAction* action : other.actionsLog) actionsLog.push_back(action->clone());
        for (Volunteer* volunteer : other.volunteers) volunteers.push_back(volunteer->clone());
        for (Order* order : other.pendingOrders) pendingOrders.push_back(new Order(*order));
        for (Order* order : other.inProcessOrders) inProcessOrders.push_back(new Order(*order));
        for (Order* order : other.completedOrders) completedOrders.push_back(new Order(*order));
        for (Customer* customer : other.customers) customers.push_back(customer->clone());
}

WareHouse& WareHouse::operator=(const WareHouse& other) {
    if (this != &other) {
        // Free current resources
        freeResources();
        
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;

        // Deep copy of pointers in vectors
        for (BaseAction* action : other.actionsLog) actionsLog.push_back(action->clone());
        for (Volunteer* volunteer : other.volunteers) volunteers.push_back(volunteer->clone());
        for (Order* order : other.pendingOrders) pendingOrders.push_back(new Order(*order));
        for (Order* order : other.inProcessOrders) inProcessOrders.push_back(new Order(*order));
        for (Order* order : other.completedOrders) completedOrders.push_back(new Order(*order));
        for (Customer* customer : other.customers) customers.push_back(customer->clone());
    }
    return *this;
}

WareHouse::WareHouse(WareHouse&& other) noexcept 
    : isOpen(other.isOpen), actionsLog(std::move(other.actionsLog)), volunteers(std::move(other.volunteers)),
    pendingOrders(std::move(other.pendingOrders)), inProcessOrders(std::move(other.inProcessOrders)),
    completedOrders(std::move(other.completedOrders)), customers(std::move(other.customers)),
    customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter),orderCounter(other.orderCounter) {
        other.customerCounter = 0;
        other.volunteerCounter = 0;
        other.orderCounter = 0;
        other.isOpen = false;
}

WareHouse& WareHouse::operator=(WareHouse&& other) noexcept {
    if (this!=&other) {
        freeResources();
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        other.isOpen = false;
        other.customerCounter = 0;
        other.volunteerCounter = 0;
        other.orderCounter = 0;
        volunteers = std::move(other.volunteers);
        actionsLog = std::move(other.actionsLog);
        pendingOrders = std::move(other.pendingOrders);
        inProcessOrders = std::move(other.inProcessOrders);
        completedOrders = std::move(other.completedOrders);
        customers = std::move(other.customers);
    }
    return *this;
}



