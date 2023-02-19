#include <iostream>
#include <string.h>
using namespace std;

class Person {
    friend ostream& operator << (ostream cout, Person p);
public:
    Person(int age) {
        m_Age = new int(age);
    }
    ~Person() {
        cout << "开始析构" << endl;
        if (m_Age != NULL) {
            delete m_Age;
            m_Age = NULL;
        }
    }
    Person& operator = (Person p) {
        if (m_Age != NULL)
            delete m_Age;
        m_Age = new int(*p.m_Age);
        return *this;
    }

private:
    int* m_Age;
};

ostream& operator << (ostream cout, Person p) {
    cout << "打印对象: " << *p.m_Age << endl;
}

int main() {
    Person p1(18);
    Person p2(20);
    p2 = p1;
    cout << p2;
}