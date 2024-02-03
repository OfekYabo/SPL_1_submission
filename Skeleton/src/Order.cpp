#include "../include/Order.h"

Order::Order(int id, int customerId, int distance) 
: id(id), customerId(customerId), distance(distance), status(OrderStatus::PENDING),
collectorId(NO_VOLUNTEER), driverId(NO_VOLUNTEER){}

int Order::getId() const 
{
    return id;
}

int Order::getCustomerId() const 
{
    return customerId;
}

void Order::setStatus(OrderStatus status) 
{
    this->status = status;
}

void Order::setCollectorId(int collectorId)
{
    this->collectorId = collectorId;
}

void Order::setDriverId(int driverId)
{
    this->driverId = driverId;
}

int Order::getCollectorId() const 
{
    return collectorId;
}

int Order::getDriverId() const 
{
    return driverId;
}

int Order::getDistance() const {
    return distance;
}
OrderStatus Order::getStatus() const
{
    return status;
}
const string Order::toString() const{
    string sCollectorId = (collectorId == NO_VOLUNTEER) ? "None" : std::to_string(collectorId);
    string sDriverId = (driverId == NO_VOLUNTEER) ? "None" : std::to_string(driverId);
    return  "OrderID: " + std::to_string(id) + "\n"
         +  "OrderStatus: " + OrderStatusToString.at(status) + "\n"
         +  "CustomerID: " + std::to_string(customerId) + "\n"
         +  "Collector: " + sCollectorId + "\n"
         +  "Driver: " + sDriverId;
} 
