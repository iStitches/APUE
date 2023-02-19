#include <iostream>
#include <string.h>

using namespace std;

class MyInteger {
friend ostream& operator << (ostream& cout, MyInteger &m);
public:
    MyInteger() {
        m_num = 2;
    }

    //重载前置++运算符
    MyInteger& operator++() {
        m_num++;
        return *this;
    }

    //重置后置++运算符,注意不能返回引用,因为引用会导致函数结束内存被释放
    const MyInteger operator++(int) {
        MyInteger temp = *this;
        m_num++;
        return temp;
    }
private:
    int m_num;
};

//重载左移运算符,只能声明为全局函数
ostream& operator << (ostream& cout, MyInteger &m) {
    cout << "我的数据: " << m.m_num;
}

int main() {
    MyInteger m;
    cout << m << endl;
    
    cout << ++m <<endl;

    cout << ++(++m) <<endl;

    cout << m++ <<endl;
}