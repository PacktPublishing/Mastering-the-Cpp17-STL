#include <cassert>
#include <cstdio>
#include <iterator>
#include <vector>
#include <cstddef>
#include <boost/iterator/iterator_facade.hpp>

namespace ex1 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
//ex1
template<typename Container>
void double_each_element(Container& arr)
{
    for (int i=0; i < arr.size(); ++i) {
        arr.at(i) *= 2;
    }
}
//dex1
#pragma GCC diagnostic pop


//ex2
class list_of_ints {
    struct node {
        int data;
        node *next;
    };
    node *head_ = nullptr;
    node *tail_ = nullptr;
    int size_ = 0;
public:
    int size() const { return size_; }
    int& at(int i) {
        if (i >= size_) throw std::out_of_range("at");
        node *p = head_;
        for (int j=0; j < i; ++j) {
            p = p->next;
        }
        return p->data;
    }
    void push_back(int value) {
        node *new_tail = new node{value, nullptr};
        if (tail_) {
            tail_->next = new_tail;
        } else {
            head_ = new_tail;
        }
        tail_ = new_tail;
        size_ += 1;
    }
    ~list_of_ints() {
        for (node *next, *p = head_; p != nullptr; p = next) {
            next = p->next;
            delete p;
        }
    }
};
//dex2

void test()
{
    std::vector<int> v;
    double_each_element(v);
    list_of_ints lst;
    lst.push_back(1);
    assert(lst.at(0) == 1);
    lst.push_back(2);
    lst.push_back(3);
    double_each_element(lst);
    assert(lst.at(0) == 2);
    assert(lst.at(1) == 4);
    assert(lst.at(2) == 6);
}
} //namespace

namespace ex3{
struct node {
    int data;
    node *next;
};

struct list_of_ints {
    node *head_ = nullptr;
    node *tail_ = nullptr;
};

void test()
{
    list_of_ints lst;
    int sum = 0;
    auto pred = [](int){ return true; };
//ex3
    for (node *p = lst.head_; p != nullptr; p = p->next) {
        if (pred(p->data)) {
            sum += 1;
        }
    }
//dex3
}
} //namespace

namespace ex4 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-private-field"
//ex4
struct list_node {
    int data;
    list_node *next;
};

class int_list_iter {
    list_node *ptr_;

    friend class list_of_ints;
    explicit int_list_iter(list_node *p) : ptr_(p) {}
public:
    int& operator*() const
        { return ptr_->data; }
    int_list_iter& operator++()
        { ptr_ = ptr_->next; return *this; }
    int_list_iter operator++(int)
        { auto result = *this; ++*this; return result; }
    bool operator==(const int_list_iter& rhs) const
        { return ptr_ == rhs.ptr_; }
    bool operator!=(const int_list_iter& rhs) const
        { return ptr_ != rhs.ptr_; }
};

class list_of_ints {
    list_node *head_ = nullptr;
    list_node *tail_ = nullptr;
    // ...
public:
    using iterator = int_list_iter;
    iterator begin() { return iterator{head_}; }
    iterator end() { return iterator{nullptr}; }
};

template<class Container, class Predicate>
int count_if(Container& ctr, Predicate pred)
{
    int sum = 0;
    for (auto it = ctr.begin(); it != ctr.end(); ++it) {
        if (pred(*it)) {
            sum += 1;
        }
    }
    return sum;
}
//dex4
#pragma GCC diagnostic pop
void test()
{
    list_of_ints lst;
    int s = count_if(lst, [](int& i){ return i > 5; });
    assert(s == 0);
}
} //namespace ex4

namespace ex5 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-private-field"
//ex5
struct list_node {
    int data;
    list_node *next;
};

template<bool Const>
class int_list_iter {
    friend class list_of_ints;
    friend class int_list_iter<!Const>;

    using node_ptr = std::conditional_t<Const, const list_node*, list_node*>;
    using reference = std::conditional_t<Const, const int&, int&>;

    node_ptr ptr_;

    explicit int_list_iter(node_ptr p) : ptr_(p) {}
public:
    reference operator*() const
        { return ptr_->data; }
    auto& operator++()
        { ptr_ = ptr_->next; return *this; }
    auto operator++(int)
        { auto result = *this; ++*this; return result; }

    // Support comparison between iterator and const_iterator types
    template<bool R>
    bool operator==(const int_list_iter<R>& rhs) const
        { return ptr_ == rhs.ptr_; }

    template<bool R>
    bool operator!=(const int_list_iter<R>& rhs) const
        { return ptr_ != rhs.ptr_; }

    // Support implicit conversion of iterator to const_iterator (but not vice versa)
    operator int_list_iter<true>() const
        { return int_list_iter<true>{ptr_}; }
};

class list_of_ints {
    list_node *head_ = nullptr;
    list_node *tail_ = nullptr;
    // ...
public:
    using const_iterator = int_list_iter<true>;
    using iterator = int_list_iter<false>;

    iterator begin() { return iterator{head_}; }
    iterator end() { return iterator{nullptr}; }
    const_iterator begin() const { return const_iterator{head_}; }
    const_iterator end() const { return const_iterator{nullptr}; }
};
//dex5
#pragma GCC diagnostic pop
void test()
{
    list_of_ints lst;
    list_of_ints::iterator it = lst.begin();
    list_of_ints::const_iterator itc = lst.begin();
    itc = itc;
    itc = it;
    assert(it == it);
    assert(it == itc);
}
} // namespace ex5

namespace ex6 {
//ex6
template<class Iterator>
void double_each_element(Iterator begin, Iterator end)
{
    for (auto it = begin; it != end; ++it) {
        *it *= 2;
    }
}

void test()
{
    std::vector<int> v {1, 2, 3, 4, 5, 6};
    double_each_element(v.begin(), v.end());
        // double each element in the entire vector
    double_each_element(v.begin(), v.begin()+3);
        // double each element in the first half
    double_each_element(&v[0], &v[3]);
        // double each element in the first half
}
//dex6
} //namespace ex6

namespace ex7{
#define count_if count_if_
#define distance distance_
//ex7
template<typename Iterator>
int distance(Iterator begin, Iterator end)
{
    int sum = 0;
    for (auto it = begin; it != end; ++it) {
        sum += 1;
    }
    return sum;
}

template<typename Iterator, typename Predicate>
int count_if(Iterator begin, Iterator end, Predicate pred)
{
    int sum = 0;
    for (auto it = begin; it != end; ++it) {
        if (pred(*it)) {
            sum += 1;
        }
    }
    return sum;
}

void test()
{
    std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};

    int number_above = count_if(v.begin(), v.end(), [](int e) { return e > 5; });
    int number_below = count_if(v.begin(), v.end(), [](int e) { return e < 5; });

    int total = distance(v.begin(), v.end());  // DUBIOUS

    assert(number_above == 2);
    assert(number_below == 5);
    assert(total == 8);
}
//dex7
#undef count_if
#undef distance
} // namespace

namespace ex8{
#define count_if count_if_
#define distance distance_
//ex8
template<typename Iterator>
int distance(Iterator begin, Iterator end)
{
    int sum = 0;
    for (auto it = begin; it != end; ++it) {
        sum += 1;
    }
    return sum;
}

template<>
int distance(int *begin, int *end)
{
    return end - begin;
}
//dex8

template<typename Iterator, typename Predicate>
int count_if(Iterator begin, Iterator end, Predicate pred)
{
    int sum = 0;
    for (auto it = begin; it != end; ++it) {
        if (pred(*it)) {
            sum += 1;
        }
    }
    return sum;
}

void test()
{
    std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};

    int number_above = count_if(v.begin(), v.end(), [](int e) { return e > 5; });
    int number_below = count_if(v.begin(), v.end(), [](int e) { return e < 5; });

    int total = distance(v.begin(), v.end());  // DUBIOUS

    assert(number_above == 2);
    assert(number_below == 5);
    assert(total == 8);
}
#undef count_if
#undef distance
} // namespace

namespace ex9{
//ex9
class getc_iterator {
    char ch;
public:
    getc_iterator() : ch(getc(stdin)) {}
    char operator*() const { return ch; }
    auto& operator++() { ch = getc(stdin); return *this; }
    auto operator++(int) { auto result(*this); ++*this; return result; }
    bool operator==(const getc_iterator&) const { return false; }
    bool operator!=(const getc_iterator&) const { return true; }
};
//dex9
//ex10
class putc_iterator {
    struct proxy {
        void operator= (char ch) { putc(ch, stdout); }
    };
public:
    proxy operator*() const { return proxy{}; }
    auto& operator++() { return *this; }
    auto& operator++(int) { return *this; }
    bool operator==(const putc_iterator&) const { return false; }
    bool operator!=(const putc_iterator&) const { return true; }
};

void test()
{
    putc_iterator it;
    for (char ch : {'h', 'e', 'l', 'l', 'o', '\n'}) {
        *it++ = ch;
    }
}
//dex10
}//namespace ex9
namespace ex11{
struct list_node {};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-private-field"
//ex11
class getc_iterator {
    char ch;
public:
    using iterator_category = std::input_iterator_tag;

    // ...
};

class putc_iterator {
    struct proxy {
        void operator= (char ch) { putc(ch, stdout); }
    };
public:
    using iterator_category = std::output_iterator_tag;

    // ...
};

template<bool Const>
class list_of_ints_iterator {
    using node_pointer = std::conditional_t<Const, const list_node*, list_node*>;
    node_pointer ptr_;

public:
    using iterator_category = std::forward_iterator_tag;

    // ...
};
//dex11
#pragma GCC diagnostic pop
}//namespace ex11

namespace ex12{
//ex12
struct input_iterator_tag { };
struct output_iterator_tag { };
struct forward_iterator_tag : public input_iterator_tag { };
struct bidirectional_iterator_tag : public forward_iterator_tag { };
struct random_access_iterator_tag : public bidirectional_iterator_tag { };
//dex12
}//namespace ex12
namespace ex13{
//ex13
void foo(std::bidirectional_iterator_tag t [[maybe_unused]])
{
    puts("std::vector's iterators are indeed bidirectional...");
}

void bar(std::random_access_iterator_tag)
{
    puts("...and random-access, too!");
}

void bar(std::forward_iterator_tag)
{
    puts("forward_iterator_tag is not as good a match");
}

void test()
{
    using It = std::vector<int>::iterator;
    foo(It::iterator_category{});
    bar(It::iterator_category{});
}
//dex13
}//namespace ex13
namespace ex14{
//ex14
struct list_node {
    int data;
    list_node *next;
};

template<bool Const>
class list_of_ints_iterator {
    friend class list_of_ints;
    friend class list_of_ints_iterator<!Const>;

    using node_pointer = std::conditional_t<Const, const list_node*, list_node*>;
    node_pointer ptr_;

    explicit list_of_ints_iterator(node_pointer p) : ptr_(p) {}
public:
    // Member typedefs required by std::iterator_traits
    using difference_type = std::ptrdiff_t;
    using value_type = int;
    using pointer = std::conditional_t<Const, const int*, int*>;
    using reference = std::conditional_t<Const, const int&, int&>;
    using iterator_category = std::forward_iterator_tag;

    reference operator*() const { return ptr_->data; }
    auto& operator++() { ptr_ = ptr_->next; return *this; }
    auto operator++(int) { auto result = *this; ++*this; return result; }

    // Support comparison between iterator and const_iterator types
    template<bool R>
    bool operator==(const list_of_ints_iterator<R>& rhs) const { return ptr_ == rhs.ptr_; }

    template<bool R>
    bool operator!=(const list_of_ints_iterator<R>& rhs) const { return ptr_ != rhs.ptr_; }

    // Support implicit conversion of iterator to const_iterator (but not vice versa)
    operator list_of_ints_iterator<true>() const { return list_of_ints_iterator<true>{ptr_}; }
};

class list_of_ints {
    list_node *head_ = nullptr;
    list_node *tail_ = nullptr;
    int size_ = 0;
public:
    using const_iterator = list_of_ints_iterator<true>;
    using iterator = list_of_ints_iterator<false>;

    // Begin and end member functions
    iterator begin() { return iterator{head_}; }
    iterator end() { return iterator{nullptr}; }
    const_iterator begin() const { return const_iterator{head_}; }
    const_iterator end() const { return const_iterator{nullptr}; }

    // Other member operations
    int size() const { return size_; }
    void push_back(int value) {
        list_node *new_tail = new list_node{value, nullptr};
        if (tail_) {
            tail_->next = new_tail;
        } else {
            head_ = new_tail;
        }
        tail_ = new_tail;
        size_ += 1;
    }
    ~list_of_ints() {
        for (list_node *next, *p = head_; p != nullptr; p = next) {
            next = p->next;
            delete p;
        }
    }
};
//dex14
//ex19
template<typename Iterator>
auto distance(Iterator begin, Iterator end)
{
    using Traits = std::iterator_traits<Iterator>;
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag, typename Traits::iterator_category>) {
        return (end - begin);
    } else {
        auto result = typename Traits::difference_type{};
        for (auto it = begin; it != end; ++it) {
            ++result;
        }
        return result;
    }
}

template<typename Iterator, typename Predicate>
auto count_if(Iterator begin, Iterator end, Predicate pred)
{
    using Traits = std::iterator_traits<Iterator>;
    auto sum = typename Traits::difference_type{};
    for (auto it = begin; it != end; ++it) {
        if (pred(*it)) {
            ++sum;
        }
    }
    return sum;
}

void test()
{
    list_of_ints lst;
    lst.push_back(1);
    lst.push_back(2);
    lst.push_back(3);
    int s = count_if(lst.begin(), lst.end(), [](int i){
        return i >= 2;
    });
    assert(s == 2);
    int d = distance(lst.begin(), lst.end());
    assert(d == 3);
}
//dex19
} //namespace ex14

namespace ex15 {
namespace std {
    using ::std::forward_iterator_tag;
    using ::std::ptrdiff_t;
}
//ex15
namespace std {
    template<
        class Category,
        class T,
        class Distance = std::ptrdiff_t,
        class Pointer = T*,
        class Reference = T&
    > struct iterator {
        using iterator_category	= Category;
        using value_type = T;
        using difference_type = Distance;
        using pointer = Pointer;
        using reference = Reference;
    };
}

class list_of_ints_iterator :
    public std::iterator<std::forward_iterator_tag, int>
{
    // ...
};
//dex15
}
namespace ex16 {
struct list_node;
//ex16
template<
    bool Const,
    class Base = std::iterator<
        std::forward_iterator_tag,
        int,
        std::ptrdiff_t,
        std::conditional_t<Const, const int*, int*>,
        std::conditional_t<Const, const int&, int&>
    >
>
class list_of_ints_iterator : public Base
{
    using typename Base::reference;  // Awkward!

    using node_pointer = std::conditional_t<Const, const list_node*, list_node*>;
    node_pointer ptr_;

public:
    reference operator*() const { return ptr_->data; }
    // ...
};
//dex16
//ex17
template<typename... Ts, typename Predicate>
int count_if(const std::iterator<Ts...>& begin,
             const std::iterator<Ts...>& end,
             Predicate pred);
//dex17
}//namespace ex16

namespace ex18{
struct list_node {
    int data;
    list_node *next;
};
//ex18
#include <boost/iterator/iterator_facade.hpp>

template<bool Const>
class list_of_ints_iterator : public boost::iterator_facade<
    list_of_ints_iterator<Const>,
    std::conditional_t<Const, const int, int>,
    std::forward_iterator_tag
>
{
    friend class boost::iterator_core_access;
    friend class list_of_ints;
    friend class list_of_ints_iterator<!Const>;

    using node_pointer = std::conditional_t<Const, const list_node*, list_node*>;
    node_pointer ptr_;

    explicit list_of_ints_iterator(node_pointer p) : ptr_(p) {}

    auto& dereference() const { return ptr_->data; }
    void increment() { ptr_ = ptr_->next; }

    // Support comparison between iterator and const_iterator types
    template<bool R>
    bool equal(const list_of_ints_iterator<R>& rhs) const { return ptr_ == rhs.ptr_; }

public:
    // Support implicit conversion of iterator to const_iterator (but not vice versa)
    operator list_of_ints_iterator<true>() const { return list_of_ints_iterator<true>{ptr_}; }
};
//dex18

class list_of_ints {
    list_node *head_ = nullptr;
    list_node *tail_ = nullptr;
    int size_ = 0;
public:
    using const_iterator = list_of_ints_iterator<true>;
    using iterator = list_of_ints_iterator<false>;

    // Begin and end member functions
    iterator begin() { return iterator{head_}; }
    iterator end() { return iterator{nullptr}; }
    const_iterator begin() const { return const_iterator{head_}; }
    const_iterator end() const { return const_iterator{nullptr}; }

    // Other member operations
    int size() const { return size_; }
    void push_back(int value) {
        list_node *new_tail = new list_node{value, nullptr};
        if (tail_) {
            tail_->next = new_tail;
        } else {
            head_ = new_tail;
        }
        tail_ = new_tail;
        size_ += 1;
    }
    ~list_of_ints() {
        for (list_node *next, *p = head_; p != nullptr; p = next) {
            next = p->next;
            delete p;
        }
    }
};

template<typename Iterator>
auto distance(Iterator begin, Iterator end)
{
    using Traits = std::iterator_traits<Iterator>;
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag, typename Traits::iterator_category>) {
        return (end - begin);
    } else {
        auto result = typename Traits::difference_type{};
        for (auto it = begin; it != end; ++it) {
            ++result;
        }
        return result;
    }
}

template<typename Iterator, typename Predicate>
auto count_if(Iterator begin, Iterator end, Predicate pred)
{
    using Traits = std::iterator_traits<Iterator>;
    auto sum = typename Traits::difference_type{};
    for (auto it = begin; it != end; ++it) {
        if (pred(*it)) {
            ++sum;
        }
    }
    return sum;
}

void test()
{
    list_of_ints lst;
    assert(lst.begin() == lst.end());
    lst.push_back(1);
    assert(lst.begin() != std::cend(lst));
    lst.push_back(2);
    lst.push_back(3);
    int s = count_if(lst.begin(), lst.end(), [](int i){ return i >= 2; });
    assert(s == 2);
    s = count_if(std::cbegin(lst), std::cend(lst), [](int i){ return i >= 2; });
    assert(s == 2);
    s = std::count_if(lst.begin(), lst.end(), [](int i){ return i >= 2; });
    assert(s == 2);
    s = std::count_if(std::cbegin(lst), std::cend(lst), [](int i){ return i >= 2; });
    assert(s == 2);
    int d = distance(lst.begin(), lst.end());
    assert(d == 3);
}
} // namespace ex18

int main()
{
    ex1::test();
    ex3::test();
    ex4::test();
    ex5::test();
    ex6::test();
    ex7::test();
    ex8::test();
    ex9::test();
    ex14::test();
    ex18::test();
}
