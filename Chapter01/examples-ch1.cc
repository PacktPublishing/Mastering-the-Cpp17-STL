#include <stdexcept>
#include <vector>
#include <cassert>

namespace ex1 {
//ex1
class array_of_ints {
    int data[10] = {};
public:
    int size() const { return 10; }
    int& at(int i) { return data[i]; }
};

void double_each_element(array_of_ints& arr)
{
    for (int i=0; i < arr.size(); ++i) {
        arr.at(i) *= 2;
    }
}
//dex1
} // namespace ex1

namespace ex2 {
//ex2
class container_of_ints {
public:
    virtual int size() const = 0;
    virtual int& at(int) = 0;
};

class array_of_ints : public container_of_ints {
    int data[10] = {};
public:
    int size() const override { return 10; }
    int& at(int i) override { return data[i]; }
};

class list_of_ints : public container_of_ints {
    struct node {
        int data;
        node *next;
    };
    node *head_ = nullptr;
    int size_ = 0;
public:
    int size() const override { return size_; }
    int& at(int i) override {
        if (i >= size_) throw std::out_of_range("at");
        node *p = head_;
        for (int j=0; j < i; ++j) {
            p = p->next;
        }
        return p->data;
    }
    ~list_of_ints();
};

void double_each_element(container_of_ints& arr)
{
    for (int i=0; i < arr.size(); ++i) {
        arr.at(i) *= 2;
    }
}

void test()
{
    array_of_ints arr;
    double_each_element(arr);

    list_of_ints lst;
    double_each_element(lst);
}
//dex2
list_of_ints::~list_of_ints() {
    for (node *next, *p = head_; p != nullptr; p = next) {
        next = p->next;
        delete p;
    }
}
} // namespace ex2

namespace ex3 {
class container_of_ints {
public:
    virtual int size() const = 0;
    virtual int& at(int) = 0;
};

class array_of_ints : public container_of_ints {
    int data[10] = {};
public:
    int size() const override { return 10; }
    int& at(int i) override { return data[i]; }
};

class list_of_ints : public container_of_ints {
    struct node {
        int data;
        node *next;
    };
    node *head_ = nullptr;
    int size_ = 0;
public:
    int size() const override { return size_; }
    int& at(int i) override {
        node *p = head_;
        for (int j=0; p != nullptr && j < i; ++j) {
            p = p->next;
        }
        return *(p ? &p->data : throw std::out_of_range("at"));
    }
};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
//ex3
template<class ContainerModel>
void double_each_element(ContainerModel& arr)
{
    for (int i=0; i < arr.size(); ++i) {
        arr.at(i) *= 2;
    }
}

void test()
{
    array_of_ints arr;
    double_each_element(arr);

    list_of_ints lst;
    double_each_element(lst);

    std::vector<int> vec = {1, 2, 3};
    double_each_element(vec);
}
//dex3
#pragma GCC diagnostic pop
void test2() {
//ex4
    std::vector<double> vecd = {1.0, 2.0, 3.0};
    double_each_element(vecd);
//dex4
}
} // namespace ex3

namespace ex5 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
//ex5
template<class Container>
int count(const Container& container)
{
    int sum = 0;
    for (auto&& elt : container) {
        sum += 1;
    }
    return sum;
}
//dex5
#pragma GCC diagnostic pop

//ex6
template<class Container, class Predicate>
int count_if(const Container& container, Predicate pred)
{
    int sum = 0;
    for (auto&& elt : container) {
        if (pred(elt)) {
            sum += 1;
        }
    }
    return sum;
}
//dex6

void test()
{
//ex7
    std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};

    assert(count(v) == 8);

    int number_above =
        count_if(v, [](int e) { return e > 5; });
    int number_below =
        count_if(v, [](int e) { return e < 5; });

    assert(number_above == 2);
    assert(number_below == 5);
//dex7
}
}

int main()
{
    ex2::test();
    ex3::test();
    ex3::test2();
    ex5::test();
}
