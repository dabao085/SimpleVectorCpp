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

    cout << "--------------" << endl;
    ivec.assign(ivec1.begin(), ivec1.end());
    printElement<int>(ivec);
    ivec1.assign(3, 1);
    printElement<int>(ivec1);
    ivec1.assign({1,3,2});
    printElement<int>(ivec1);

    ivec1.insert(ivec1.end(), 4);
    printElement<int>(ivec1);
    ivec1.insert(ivec1.begin(), 0);
    printElement<int>(ivec1);

    ivec1.clear();
    printElement<int>(ivec1);

    cout << "-------------" << endl;
    Vector<int> ivec2{1,2,3,4,5};
    ivec2.erase(ivec2.begin());
    printElement<int>(ivec2);
    cout << "-------------" << endl;
    ivec2.erase(ivec2.end());
    printElement<int>(ivec2);  
    cout << "-------------" << endl;
    ivec2.erase(ivec2.begin() + 1, ivec2.end());
    printElement<int>(ivec2);
    ivec2.erase(ivec2.begin());
    printElement<int>(ivec2);

    return 0;
}