#include "Vector.h"
#include <iostream>
using namespace std;

// 要放在.cc文件
template<typename T>
std::allocator<T> Vector<T>::alloc_;

class Test
{
friend ostream& operator<<(ostream& os, const Test&);
public:
    explicit Test(int val): val_(val){}
private:
    int val_;
};

ostream& operator<<(ostream& os, const Test& t)
{
    os << t.val_;
}

template<typename T>
void printElement(const Vector<T>& vec)
{
    for(auto &a : vec)
    {
        cout << a << " ";
    }
    cout << endl;
    cout << "size: " << vec.size() << ", capacity: " << vec.capacity() << endl;
}

int main()
{
    Vector<int> ivec{1, 2, 3, 4, 5};
    printElement<int>(ivec);
    ivec.reserve(3);
    printElement<int>(ivec);
    ivec.reserve(10);
    printElement<int>(ivec);
    cout << "ivec.reserve(7);" << endl;
    ivec.reserve(7);
    printElement<int>(ivec);

    Vector<int> ivec1{1, 2, 3};
    printElement<int>(ivec1);
    ivec1.pop_back();
    printElement<int>(ivec1);

    return 0;
}