#include <iostream>
#include <string.h>

using namespace std;

class Building;

class GoodDay
{
public:
    GoodDay();
    void visit();
private:
    Building *building;
};

class Building
{
friend class GoodDay;
public:
    Building();
public:
    string m_Sittingroom;
private:
    string m_Bedroom;
};

Building::Building()
{
    this->m_Sittingroom = "客厅";
    this->m_Bedroom = "卧室";
}

GoodDay::GoodDay()
{
    this->building = new Building();
}

void GoodDay::visit()
{
    cout << "正在访问：" << this->building->m_Sittingroom << endl;
    cout << "正在访问：" << this->building->m_Bedroom << endl;
}

int main() {
    GoodDay day;
    day.visit();
}