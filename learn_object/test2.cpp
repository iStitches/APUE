#include <iostream>
#include <string.h>

using namespace std;

class Building;

class GoodDay {
public:
    GoodDay();
    void visit();

private:
    Building *building;
};

class Building {
    friend void GoodDay::visit();
public:
    Building();
public:
    string m_SittingRoom;
private:
    string m_BedRoom;
};

Building::Building() {
    this->m_SittingRoom = "客厅";
    this->m_BedRoom = "卧室";
}

GoodDay::GoodDay() {
    this->building = new Building();
}

void GoodDay::visit() {
    cout << "正在访问: " << this->building->m_SittingRoom;
    cout << "正在访问: " << this->building->m_BedRoom;
}