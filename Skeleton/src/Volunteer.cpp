
#include "../include/Volunteer.h"



/////////////////////////////////////////////////////////
//////////////////////// Volunteer //////////////////////
/////////////////////////////////////////////////////////

Volunteer::Volunteer(int id, const string &name) 
: completedOrderId(NO_ORDER), activeOrderId(NO_ORDER), id(id), name(name) {}

int Volunteer::getId() const
{
    return id;
}

const string& Volunteer::getName() const 
{
    return name;
}

int Volunteer::getActiveOrderId() const 
{
    return activeOrderId;
}

int Volunteer::getCompletedOrderId() const 
{
    return completedOrderId;
}

bool Volunteer::isBusy() const
{
    return (activeOrderId != NO_ORDER);
}


/////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Collector Volunteer ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


CollectorVolunteer::CollectorVolunteer(int id, const string &name, const int coolDown) : Volunteer(id, name) ,coolDown(coolDown), timeLeft(0){}
CollectorVolunteer* CollectorVolunteer::clone() const 
{
     return new CollectorVolunteer(*this);
}
void CollectorVolunteer::step() // need to check if it's correct
{
    if(decreaseCoolDown())
    {
        completedOrderId = activeOrderId;
        activeOrderId = NO_ORDER;
    }
}
int CollectorVolunteer::getCoolDown() const
{
    return coolDown;
}

int CollectorVolunteer::getTimeLeft() const 
{
    return timeLeft;
}
void CollectorVolunteer::setTimeLeft(int timeLeft)
{
    this->timeLeft = timeLeft;
}
bool CollectorVolunteer::decreaseCoolDown() //Decrease timeLeft by 1,return true if timeLeft=0,false otherwise
{
    if(timeLeft > 0){
        timeLeft = timeLeft - 1;
    }
    return (timeLeft == 0);
}

bool CollectorVolunteer::hasOrdersLeft() const  // Signal whether the volunteer didn't reach orders limit,Always true for CollectorVolunteer and DriverVolunteer
{
    return true;
} 

bool CollectorVolunteer::canTakeOrder(const Order &order) const 
{
    return (order.getStatus() == OrderStatus::PENDING && !Volunteer::isBusy());
}

void CollectorVolunteer::acceptOrder(const Order &order) 
{
    if(canTakeOrder(order)){
        activeOrderId = order.getId();
        timeLeft = coolDown;
    }
}
string CollectorVolunteer::toString() const 
{
    string sActiveOrderId = (activeOrderId == -1) ? "None" : std::to_string(activeOrderId);
    string sTimeLeft = (getTimeLeft() <= 0) ? "None" : std::to_string(getTimeLeft());
    string sIsbusy = (Volunteer::isBusy()) ? "True" : "False";
    return "VolunteerID: " + std::to_string(getId()) + "\n"
         + "isBusy:" + sIsbusy + "\n"
         + "OrderID: " + sActiveOrderId + "\n"
         + "TimeLeft: " + sTimeLeft + "\n" 
         + "OrdersLeft: No Limit";
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// LimitedCollector Volunteer //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, const string &name, int coolDown ,int maxOrders) : CollectorVolunteer(id, name, coolDown), maxOrders(maxOrders), ordersLeft(maxOrders) {}
LimitedCollectorVolunteer* LimitedCollectorVolunteer::clone() const 
{
    return new LimitedCollectorVolunteer(*this);
}

bool LimitedCollectorVolunteer::hasOrdersLeft() const 
{
    return (ordersLeft > 0);
}

bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const 
{
    return (order.getStatus() == OrderStatus::PENDING && !CollectorVolunteer::isBusy() && hasOrdersLeft());
}

void LimitedCollectorVolunteer::acceptOrder(const Order &order) 
{
    if(canTakeOrder(order)){
        ordersLeft--;
        activeOrderId = order.getId();
        setTimeLeft(this->getCoolDown());   
    }
}

int LimitedCollectorVolunteer::getMaxOrders() const 
{
    return maxOrders;
}
int LimitedCollectorVolunteer::getNumOrdersLeft() const
{
    return ordersLeft;
}
string LimitedCollectorVolunteer::toString() const 
{
    string sActiveOrderId = (activeOrderId == -1) ? "None" : std::to_string(activeOrderId);
    string sTimeLeft = (getTimeLeft() <= 0) ? "None" : std::to_string(getTimeLeft());
    string sIsbusy = (Volunteer::isBusy()) ? "True" : "False";
    return "VolunteerID: " + std::to_string(getId()) + "\n"
         + "isBusy:" + sIsbusy + "\n" 
         + "OrderID: " + sActiveOrderId + "\n"
         + "TimeLeft: " + sTimeLeft + "\n"
         + "OrdersLeft: " + std::to_string(ordersLeft);
}

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Driver Collector ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

DriverVolunteer::DriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep) : Volunteer(id,name), maxDistance(maxDistance), distancePerStep(distancePerStep), distanceLeft(0) {}
DriverVolunteer* DriverVolunteer::clone() const 
{
    return new DriverVolunteer(*this);
}

int DriverVolunteer::getDistanceLeft() const
{
    return distanceLeft;
}
void DriverVolunteer::setDistanceLeft(int distanceLeft)
{
    this->distanceLeft = distanceLeft;
}

int DriverVolunteer::getMaxDistance() const
{
    return maxDistance;
}
int DriverVolunteer::getDistancePerStep() const
{
    return distancePerStep;
} 
bool DriverVolunteer::decreaseDistanceLeft() //Decrease distanceLeft by distancePerStep,return true if distanceLeft<=0,false otherwise
{
    if(distanceLeft > 0){
        distanceLeft = distanceLeft - distancePerStep;
    }
    return (distanceLeft <= 0);
} 
bool DriverVolunteer::hasOrdersLeft() const 
{
    return true;
}
bool DriverVolunteer::canTakeOrder(const Order &order) const 
{
    return (order.getStatus() == OrderStatus::COLLECTING && !Volunteer::isBusy() && order.getDistance() <= maxDistance); // Signal if the volunteer is not busy and the order is within the maxDistance
} 

void DriverVolunteer::acceptOrder(const Order &order)  
{
    if(canTakeOrder(order)){
        distanceLeft = order.getDistance(); // Assign distanceLeft to order's distance
        activeOrderId = order.getId();
    }
} 

void DriverVolunteer::step() 
{ 
    if(decreaseDistanceLeft()){
        completedOrderId = activeOrderId;
        activeOrderId = NO_ORDER;
    }
} 

string DriverVolunteer::toString() const {
    string sActiveOrderId = (activeOrderId == -1) ? "None" : std::to_string(activeOrderId);
    string sDistanceLeft = (getDistanceLeft() <= 0) ? "None" : std::to_string(getDistanceLeft());
    string sIsbusy = (Volunteer::isBusy()) ? "True" : "False";
    return "VolunteerID: " + std::to_string(getId()) + "\n"
         + "isBusy:" + sIsbusy + "\n"
         + "OrderID: " + sActiveOrderId + "\n"
         + "DistanceLeft: " + sDistanceLeft + "\n"
         + "OrdersLeft: No Limit";
}

////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Limited Driver Collector ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep,int maxOrders) : DriverVolunteer(id, name, maxDistance, distancePerStep) , maxOrders(maxOrders), ordersLeft(maxOrders) {}
LimitedDriverVolunteer* LimitedDriverVolunteer::clone() const
{
    return new LimitedDriverVolunteer(*this);
}

int LimitedDriverVolunteer::getMaxOrders() const 
{
    return maxOrders;
}

int LimitedDriverVolunteer::getNumOrdersLeft() const 
{
    return ordersLeft;
}

bool LimitedDriverVolunteer::hasOrdersLeft() const
{
    return (ordersLeft > 0);
}

bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const {
    return (hasOrdersLeft() && order.getStatus() == OrderStatus::COLLECTING && !Volunteer::isBusy() && order.getDistance() <= DriverVolunteer::getMaxDistance());
} // Signal if the volunteer is not busy, the order is within the maxDistance.

void LimitedDriverVolunteer::acceptOrder(const Order &order) {
    if(canTakeOrder(order))
    {
        ordersLeft--;
        activeOrderId = order.getId();
        setDistanceLeft(order.getDistance());
    }
} // Assign distanceLeft to order's distance and decrease ordersLeft.
string LimitedDriverVolunteer::toString() const 
{
    string sActiveOrderId = (activeOrderId == -1) ? "None" : std::to_string(activeOrderId);
    string sDistanceLeft = (getDistanceLeft() <= 0) ? "None" : std::to_string(getDistanceLeft());
    string sIsbusy = (Volunteer::isBusy()) ? "True" : "False";
    return "VolunteerID: " + std::to_string(getId()) + "\n"
         + "isBusy:" + sIsbusy + "\n"
         + "OrderID: " + sActiveOrderId + "\n"
         + "DistanceLeft: " + sDistanceLeft + "\n"
         + "OrdersLeft: " + std::to_string(ordersLeft);
}
