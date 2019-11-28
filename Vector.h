// Xiang Cheng 2019-11-27

#ifndef _CX_VECTOR_H_
#define _CX_VECTOR_H_

#include <memory>
// #include <map>
#include <initializer_list>
#include <algorithm>
#include <stdio.h>

// Integers type
struct true_type {
};

// Not Integers type
struct false_type {
};

template <class Tp> struct Is_integer 
{
    typedef struct false_type Integral;
};

// 模板特例化
template<>
struct Is_integer<int> 
{
    typedef struct true_type Integral;
};

template <typename T>
class Vector
{
public:
    // constructors
    explicit Vector():data_(nullptr), size_(0), capacity_(0){}
    Vector(std::initializer_list<T> li);
    Vector(const Vector&);
    Vector(Vector&&) noexcept;
    Vector& operator=(const Vector&);
    Vector& operator=(Vector&&) noexcept;
    // destructor
    ~Vector(){ free(); }

    void swap(Vector& other)
    {
        using std::swap;
        swap(data_, other.data_);
        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
    }

    // Capacity
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return data_ == nullptr && size_ == 0; }
    size_t max_size() const noexcept { return static_cast<size_t>(1) / sizeof(T); }
    void resize(size_t);
    void resize(size_t, const T&);
    void reserve(size_t);

    // Iterator
    T* begin() { return data_; }
    const T* begin() const { return data_; }
    T* end() { return data_ + size_; }
    const T* end() const { return data_ + size_; }

    // Element access
    T* operator[](size_t pos){ return at(pos); }
    const T* operator[](size_t pos) const{ return at(pos);}
    T* at(size_t pos);
    const T* at(size_t pos) const;
    T* front(){ return begin(); }
    const T* front() const { return begin(); }
    T* back();
    const T* back()const;

    // Modifier
    void push_back(const T&);
    void push_back(T&&);
    void pop_back();
    void clear() noexcept;

    void assign(size_t, const T&); 
    template<typename Iter>
    void assign(Iter first, Iter last);
    void assign(std::initializer_list<T> li);

    template<typename Iter>
    Iter insert(const Iter, const T&);
    template<typename Iter>
    Iter insert(const Iter, T&&);

    template<typename Iter>
    Iter erase(const Iter);
    template<typename Iter>
    Iter erase(const Iter first, const Iter last);

    // Allocator
    std::allocator<T> get_allocator() const noexcept
    {
        return alloc_;
    }

private:
    // InputParam: 
    // Const T*: Begin iterator
    // size_t: size
    // size_t: capacity
    //std::pair<T*, T*> alloc_n_copy(const T*, size_t, size_t);
    T* alloc_n_copy(const T*, size_t, size_t);
    // 保证有足够的空间进行插入等操作。
    void reallocate();
    void free();
    bool check(size_t pos)
    {
        return pos < size_;
    }
    void fill_assign(size_t, const T&);
    template<typename Integer>
    void assign_dispatch(Integer n, Integer value, true_type) // 分发请求(迭代器或fill)
    {
        fill_assign(n, value);
    }
    template<typename Iter>
    void assign_dispatch(Iter first, Iter last, false_type)
    {
        assign_aux(first, last);
    }
    template<typename Iter>
    void assign_aux(Iter first, Iter last);  // 处理迭代器
private:
    static std::allocator<T> alloc_;
    T* data_;
    // T* end_;
    // T* capacity_;
    size_t size_;
    size_t capacity_;
};

// constructors
template<typename T>
Vector<T>::Vector(std::initializer_list<T> li)
{
    auto data = alloc_n_copy(li.begin(), li.size(), li.size());
    data_ = data;
    size_ = capacity_ = li.size();
}

template<typename T>
Vector<T>::Vector(const Vector& vec)
{
    auto data = alloc_n_copy(vec.begin(), vec.size(), vec.capacity());
    data_ = data;
    size_ = vec.size_;
    capacity_ = vec.capacity_;
}

template<typename T>
Vector<T>::Vector(Vector&& other) noexcept
{
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    
    other.data_ = nullptr;
    other.size_ = other.capacity_ = 0;
}

// copy and swap
template<typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other)
{
    Vector tmpvec(other);
    swap(other);
    return *this;
}

// TODO: 自身赋值怎么处理?
template<typename T>
Vector<T>& Vector<T>::operator=(Vector<T>&& other) noexcept
{
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    
    other.data_ = nullptr;
    other.size_ = other.capacity_ = 0;
}

// Capacity
template<typename T>
void Vector<T>::resize(size_t n)
{
    resize(n, T());
}

template<typename T>
void Vector<T>::resize(size_t n, const T& value)
{
    if(n <= capacity_)  // 不需要申请新的空间。
    {
        if(n <= size_)  // 需要截断。
        {
            auto b = data_, e = data_ + n;
            for(; e != b; /*empty*/)
            {
                alloc_.destroy(--e);
            }
            // 更新信息。
            size_ = n;
        }
        else    // 需要填充。
        {
            auto b = data_ + size_;
            std::uninitialized_fill_n(b, n - size_, value);
            // 更新信息。
            size_ = n;
        }
    }
    else    // 需要申请新空间
    {
        auto data = alloc_n_copy(data_, size_, n);
        data_ = data;
        capacity_ = n;
        // 填充空闲部分。
        auto b = data_ + size_;
        std::uninitialized_fill_n(b, n - size_, value);
        // 更新信息。
        size_ = n;    
    }
}

template<typename T>
void Vector<T>::reserve(size_t n)
{
    if(n <= capacity_)
    {
        return ;
    }
    else
    {
        auto data = alloc_n_copy(data_, size_, n);
        data_ = data;
        capacity_ = n;
    }
}

// Element access
template<typename T>
T* Vector<T>::at(size_t pos)
{
    if(check(pos))
    {
        return *(data_ + pos);
    }
    else
    {
        throw std::out_of_range("pos out of range!");
    }
}

template<typename T>
const T* Vector<T>::at(size_t pos) const
{
    if(check(pos))
    {
        return *(data_ + pos);
    }
    else
    {
        throw std::out_of_range("pos out of range!");
    }
}

template<typename T>
T* Vector<T>::back()
{
    if(!empty())
    {
        return data_ + size_ - 1;
    }
}

template<typename T>
const T* Vector<T>::back()const
{
    if(!empty())
    {
        return data_ + size_ - 1;
    }
}


// Modifier.
template<typename T>
void Vector<T>::push_back(const T& value)
{
    printf("push_back reference...\n");
    reallocate();
    alloc_.construct(data_ + size_, value);
    ++size_;
}

// TODO: 如何实现move?
template<typename T>
void Vector<T>::push_back(T&& value)
{
    printf("push_back move...\n");
    reallocate();
    alloc_.construct(data_ + size_, std::move(value));
    ++size_;
}

// STL中的clear并不会释放空间，仅将size标记为0。
// ??? 该程序会因为这个而导致内存泄漏吗?
template<typename T>
void Vector<T>::clear() noexcept
{
    //free();
    size_ = 0;
}

template<typename T>
void Vector<T>::pop_back()
{
    if(!empty())
    {
        auto f = back();
        alloc_.destroy(f);
        --size_;
    }
}

// TODO: 如何处理自身赋值?
// TODO: 性能有优化空间:若dest空间够，就不用申请新的空间。
// 直接释放旧空间，申请新空间，存放first-last.
template<typename T>
template<typename Iter>
void Vector<T>::assign(Iter first, Iter last)
{
    typedef typename Is_integer<Iter>::Integral Integral;
    assign_dispatch(first, last, Integral());
}

template<typename T>
void Vector<T>::assign(size_t n, const T& value)
{
    fill_assign(n, value);
}

template<typename T>
void Vector<T>::assign(std::initializer_list<T> li)
{
    free();
    size_t dist = li.size();
    auto data = alloc_n_copy(li.begin(), dist, dist);
    data_ = data;
    size_ = capacity_ = dist;
}

template<typename T>
template<typename Iter>
Iter Vector<T>::insert(const Iter position, const T& value)
{
    size_t new_capacity = size_ + 1 > capacity_? size_ + 1 : capacity_;
    auto data = alloc_.allocate(new_capacity);
    auto f1 = std::uninitialized_copy(data_, position, data);
    std::uninitialized_fill(f1, f1 + 1, value);
    std::uninitialized_copy(position, data_ + size_, f1 + 1);
    data_ = data;
    ++size_;
    capacity_ = new_capacity;
    return f1;
}

template<typename T>
template<typename Iter>
Iter Vector<T>::insert(const Iter position, T&& value)
{
    // printf("move Vector<T>::insert...\n");
    size_t new_capacity = size_ + 1 > capacity_? size_ + 1 : capacity_;
    auto data = alloc_.allocate(new_capacity);
    auto f1 = std::uninitialized_copy(data_, position, data);
    std::uninitialized_fill(f1, f1 + 1, std::move(value));
    std::uninitialized_copy(position, data_ + size_, f1 + 1);
    data_ = data;
    ++size_;
    capacity_ = new_capacity;
    return f1;
}

// 不申请新的空间，直接在以前的空间上操作。
template<typename T>
template<typename Iter>
Iter Vector<T>::erase(const Iter position)
{
    // TODO: check position.
    auto end = data_ + size_;
    if(position == end)
    {
        return position;
    }

    alloc_.destroy(position);
    --size_;
    if(size_ != 0)  // 非最后一个元素
    {
        auto start = position + 1;
        for(; start != end; ++start)
        {
            std::uninitialized_fill(start - 1, start, *start);
            alloc_.destroy(start);
        }
    }
}

// 不申请新的空间，直接在以前的空间上操作。多次调用erase(const Iter position); 
// TODO: 性能有优化空间, check position.
// first, n
template<typename T>
template<typename Iter>
Iter  Vector<T>::erase(const Iter first, const Iter last)
{
    // TODO: check position.
    size_t dist = last - first;
    while(dist > 0)
    {
        erase(first);
        --dist;
    }
}

// private member functions.
template <typename T>
void Vector<T>::free()
{
    if(data_ != nullptr)
    {
        auto endIter = data_ + size_;
        for(; endIter != data_; )
        {
            alloc_.destroy(--endIter);
        }
        alloc_.deallocate(data_, capacity_);
        data_ = nullptr;
        size_ = capacity_ = 0;
    }
}

template<typename T>
T* Vector<T>::alloc_n_copy(const T* b, size_t size, size_t capacity)
{
    auto data = alloc_.allocate(capacity);
    std::uninitialized_copy(b, b + size, data);
    // data_ = data;    // 在函数调用的外层，会进行一次单独拷贝，此处可以不写。
    size_ = size;
    capacity_ = capacity;

    return data;
}

template<typename T>
void Vector<T>::reallocate()
{
    if(size_ == capacity_)
    {
        size_t new_capacity = capacity_? capacity_ * 2 : 1;
        auto data = alloc_n_copy(data_, size_, new_capacity);
        data_ = data;
        //size_ = size_;    // size_大小未变，可不写。
        capacity_ = new_capacity;   // capacity_变大
    }
}

// TODO: 实现方式需要调整，此处直接申请新的空间，效率略低。
template<typename T>
void Vector<T>::fill_assign(size_t n, const T& value)
{
    free();
    auto data = alloc_.allocate(n);
    std::uninitialized_fill_n(data, n, value);
    data_ = data;
    size_ = capacity_ = n;
}

// TODO: 实现方式需要调整，此处直接申请新的空间，效率略低。
template<typename T>
template<typename Iter>
void Vector<T>::assign_aux(Iter first, Iter last)  // 处理迭代器
{
    free();
    size_t dist = last - first;
    auto data = alloc_n_copy(first, dist, dist);
    data_ = data;
    size_ = capacity_ = dist;
}

// Non-member functions.
template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& vec)
{
    auto b = vec.begin();
    auto e = vec.end();
    for(; b != e; )
    {
        os << *(b++) << " ";
    }
    return os;
}

#endif