#include "../include/WareHouse.h"
#include <iostream>

// BaseAction class
BaseAction::BaseAction() : errorMsg(""), status(ActionStatus::ERROR)  {}

ActionStatus BaseAction::getStatus() const { return status;}
string BaseAction::getStatusString() const {return status == ActionStatus::COMPLETED ? " COMPLETED" : " ERROR";}

void BaseAction::complete() { status = ActionStatus::COMPLETED;}

void BaseAction::error(string errorMsg) { status = ActionStatus::ERROR; this->errorMsg = "Error: " + errorMsg;}

string BaseAction::getErrorMsg() const { return errorMsg;}




// SimulateStep class
SimulateStep::SimulateStep(int numOfSteps) : numOfSteps(numOfSteps) {}
SimulateStep* SimulateStep::clone() const { return new SimulateStep(*this);}
string SimulateStep::toString() const { return "simulateStep " + std::to_string(numOfSteps) + getStatusString();}

void SimulateStep::act(WareHouse &wareHouse) {
    for(int i = numOfSteps; i>0; i-- ){
        wareHouse.step();
    }
    complete();
}





// AddOrder class
AddOrder::AddOrder(int id) : customerId(id) {}
AddOrder* AddOrder::clone() const { return new AddOrder(*this);}
string AddOrder::toString() const { return "order " + std::to_string(customerId) + getStatusString();}

void AddOrder::act(WareHouse &wareHouse) {
    if (!wareHouse.isCustomerExist(customerId)){
        error("Cannot place this order");
        std::cout << getErrorMsg() << std::endl;
    }else{
        Customer& customer = wareHouse.getCustomer(customerId);
        if ( !customer.canMakeOrder() ){
            error("Cannot place this order");
            std::cout << getErrorMsg() << std::endl;
        }
        else{
            int orderId = wareHouse.getOrderCounter();
            wareHouse.addOrder(new Order(orderId, customerId, customer.getCustomerDistance()));
            customer.addOrder(orderId);
            complete();
        }
    }
}






// AddCustomer class
AddCustomer::AddCustomer(const string &customerName, const string &customerType, int distance, int maxOrders)
    : customerName(customerName), customerType(StringToCustomerType.at(customerType)), distance(distance), maxOrders(maxOrders) {}
AddCustomer* AddCustomer::clone() const { return new AddCustomer(*this);}
string AddCustomer::toString() const { 
    string customerTypeString = customerType == CustomerType::Soldier ? "soldier" : "civilian";
    return "customer " + customerName + " " + customerTypeString + " " + std::to_string(distance) + " " + std::to_string(maxOrders) + " " + getStatusString();
}

void AddCustomer::act(WareHouse &wareHouse) {
    int customerId = wareHouse.getCustomerCounter();
    switch (customerType){
        case CustomerType::Civilian:
            wareHouse.addCustomer(new CivilianCustomer(customerId, customerName, distance, maxOrders));
        case CustomerType::Soldier:
            wareHouse.addCustomer(new SoldierCustomer(customerId, customerName, distance, maxOrders));
    }
    complete();
}





// PrintOrderStatus class
PrintOrderStatus::PrintOrderStatus(int id) : orderId(id) {}
PrintOrderStatus* PrintOrderStatus::clone() const { return new PrintOrderStatus(*this);}
string PrintOrderStatus::toString() const { return "orderStatus " + std::to_string(orderId) + getStatusString();}

void PrintOrderStatus::act(WareHouse &wareHouse) {
    if (!wareHouse.isOrderExist(orderId)){
        error("Order doesn't exist");
        std::cout << getErrorMsg() << std::endl;
    } 
    else { 
    std::cout << wareHouse.getOrder(orderId).toString() << std::endl;
    complete();
    }
}





// PrintCustomerStatus class
PrintCustomerStatus::PrintCustomerStatus(int customerId) : customerId(customerId) {}
PrintCustomerStatus* PrintCustomerStatus::clone() const { return new PrintCustomerStatus(*this);}
string PrintCustomerStatus::toString() const { return "customerStatus " + std::to_string(customerId) + getStatusString();}

void PrintCustomerStatus::act(WareHouse &wareHouse) {
    if (!wareHouse.isCustomerExist(customerId)){
        error("Customer doesn't exist");
        std::cout << getErrorMsg() << std::endl;
    } 
    else { 
        Customer& customer = wareHouse.getCustomer(customerId);
        std::cout << "CustomerID: " + std::to_string(customerId) << std::endl;
        for (int orderId : customer.getOrdersIds()) {
            std::cout << "OrderID: " + std::to_string(orderId) << std::endl;
            std::cout << "OrderStatus: " + OrderStatusToString.at(wareHouse.getOrder(orderId).getStatus()) << std::endl;//no need tho check existance of order because we know it exists
        }
        std::cout << "numOrdersLeft: " + std::to_string(customer.getMaxOrders()-customer.getNumOrders()) << std::endl;
        complete();
    }
}






// PrintVolunteerStatus class
PrintVolunteerStatus::PrintVolunteerStatus(int id) : volunteerId(id) {}
PrintVolunteerStatus* PrintVolunteerStatus::clone() const { return new PrintVolunteerStatus(*this);}
string PrintVolunteerStatus::toString() const { return "volunteerStatus " + std::to_string(volunteerId) + getStatusString();}

void PrintVolunteerStatus::act(WareHouse &wareHouse) {
    int VolunteerIndex = wareHouse.isVolunteerExist(volunteerId);
    if ( VolunteerIndex == -1 ){
        error("Volunteer doesn't exist");
        std::cout << getErrorMsg() << std::endl;
    }
    else{
        Volunteer& volunteer = wareHouse.getVolunteerByIndex(VolunteerIndex);
        std::cout << volunteer.toString() << std::endl;
        complete();
    }
}



// PrintActionsLog class
PrintActionsLog::PrintActionsLog() {}
PrintActionsLog* PrintActionsLog::clone() const { return new PrintActionsLog(*this);}
string PrintActionsLog::toString() const { return "log" + getStatusString();}

void PrintActionsLog::act(WareHouse &wareHouse) {
    std::vector<BaseAction*> actions = wareHouse.getActions();
    for (auto it = actions.begin(); it != actions.end(); ++it) {
        std::cout << (*it)->toString() << std::endl;
    }
    complete();
}


// Close class
Close::Close() {}
Close* Close::clone() const { return new Close(*this);}
string Close::toString() const { return "close" + getStatusString();}

void Close::act(WareHouse &wareHouse) {
    wareHouse.close();
    complete();
}



// BackupWareHouse class
BackupWareHouse::BackupWareHouse() {}
BackupWareHouse* BackupWareHouse::clone() const { return new BackupWareHouse(*this);}
string BackupWareHouse::toString() const { return "backup" + getStatusString();}

void BackupWareHouse::act(WareHouse &wareHouse) {
extern WareHouse* backup;
delete backup;
backup = new WareHouse(wareHouse);
complete();
}





// RestoreWareHouse class
RestoreWareHouse::RestoreWareHouse() {}
RestoreWareHouse* RestoreWareHouse::clone() const { return new RestoreWareHouse(*this);}
string RestoreWareHouse::toString() const { return "restore" + getStatusString();}

void RestoreWareHouse::act(WareHouse &wareHouse) {
    extern WareHouse* backup;    
    if (backup == nullptr) {
    error("No backup available");
    std::cout << getErrorMsg() << std::endl;
    }
    else {
    wareHouse = WareHouse(*backup);
    complete();
    }
}








