#include <algorithm>
#include <array>
#include <cassert>
#include <iterator>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ex1 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
struct B { };
//ex1
struct owning_A {
    B b_;
};

struct non_owning_A {
    B& b_;
};

void test()
{
    B b;

    // a1 takes ownership of [a copy of] b.
    owning_A a1 { b };

    // a2 merely holds a reference to b;
    // a2 doesn't own b.
    non_owning_A a2 { b };
}
//dex1
#pragma GCC diagnostic pop
} // namespace ex1

namespace ex2 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
struct B { };
//ex2
struct owning_A {
    B *b_;

    explicit owning_A(B *b) : b_(b) {}

    owning_A(owning_A&& other) : b_(other.b_) {
        other.b_ = nullptr;
    }

    owning_A& operator= (owning_A&& other) {
        delete b_;
        b_ = other.b_;
        other.b_ = nullptr;
        return *this;
    }

    ~owning_A() {
        delete b_;
    }
};

struct non_owning_A {
    B *b_;
};

void test()
{
    B *b = new B;

    // a1 takes ownership of *b.
    owning_A a1 { b };

    // a2 merely holds a pointer to *b;
    // a2 doesn't own *b.
    non_owning_A a2 { b };
}
//dex2
#pragma GCC diagnostic pop
} // namespace ex2

namespace ex3 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
//ex3
void dont_do_this()
{
    // This variable takes up 4 megabytes of stack space ---
    // enough to blow your stack and cause a segmentation fault!
    int arr[1'000'000];
}

void dont_do_this_either()
{
    // Changing it into a C++ std::array doesn't fix the problem.
    std::array<int, 1'000'000> arr;
}
//dex3
#pragma GCC diagnostic pop
} // namespace ex3

namespace ex4 {
void test()
{
    if (true) {
//ex4
        std::string c_style[4] = {
            "the", "quick", "brown", "fox"
        };
        assert(c_style[2] == "brown");
        assert(std::size(c_style) == 4);
        assert(std::distance(std::begin(c_style), std::end(c_style)) == 4);

        // Copying via operator= isn't supported.
        std::string other[4];
        std::copy(std::begin(c_style), std::end(c_style), std::begin(other));

        // Swapping IS supported... in linear time, of course.
        using std::swap;
        swap(c_style, other);

        // Comparison isn't supported; you have to use a standard algorithm.
        // Worse, operator== does the "wrong" thing: address comparison!
        assert(c_style != other);
        assert(std::equal(
            c_style, c_style + 4,
            other, other + 4
        ));
        assert(!std::lexicographical_compare(
            c_style, c_style + 4,
            other, other + 4
        ));
//dex4
    }
    if (true) {
//ex5
        std::array<std::string, 4> arr = {{
            "the", "quick", "brown", "fox"
        }};
        assert(arr[2] == "brown");

        // .begin(), .end(), and .size() are all provided.
        assert(arr.size() == 4);
        assert(std::distance(arr.begin(), arr.end()) == 4);

        // Copying via operator= is supported... in linear time.
        std::array<std::string, 4> other;
        other = arr;

        // Swapping is also supported... in linear time.
        using std::swap;
        swap(arr, other);

        // operator== does the natural thing: value comparison!
        assert(&arr != &other);  // The arrays have different addresses...
        assert(arr == other);    // ...but still compare lexicographically equal.
        assert(arr >= other);  // Relational operators are also supported.
    //dex5
    }
}
} // namespace ex4

namespace ex6 {
//ex6
// You can't return a C-style array from a function.
// auto cross_product(const int (&a)[3], const int (&b)[3]) -> int[3];

// But you can return a std::array.
auto cross_product(const std::array<int, 3>& a, const std::array<int, 3>& b)
    -> std::array<int, 3>
{
    return {{
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0],
    }};
}
//dex6
void test()
{
    using Vec3 = std::array<int,3>;
    Vec3 a {{2,3,4}};
    Vec3 b {{5,6,7}};
    auto c = cross_product(a, b);
    assert((c == Vec3{{-3,6,-3}}));
    assert((c < Vec3{{-3,6,-2}}));
}
} // namespace ex6

namespace ex7 {
//ex7
struct Vec3 {
    int x, y, z;
    Vec3(int x, int y, int z) : x(x), y(y), z(z) {}
};

bool operator==(const Vec3& a, const Vec3& b) {
    return std::tie(a.x, a.y, a.z) ==
            std::tie(b.x, b.y, b.z);
}

bool operator!=(const Vec3& a, const Vec3& b) {
    return !(a == b);
}

// Operators < <= > >= don't make sense for Vec3

Vec3 cross_product(const Vec3& a, const Vec3& b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}
//dex7
void test()
{
    Vec3 a = {2,3,4};
    Vec3 b = {5,6,7};
    auto c = cross_product(a, b);
    assert((c == Vec3{-3,6,-3}));
}
} // namespace ex7

namespace ex8 {

//ex8
template<typename T>
inline void destroy_n_elements(T *p, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        p[i].~T();
    }
}

template<typename T>
class vector {
    T *ptr_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;

public:
    // ...

    void reserve(size_t c) {
        if (capacity_ >= c) {
            // do nothing
            return;
        }

        // For now, we'll ignore the problem of
        // "What if malloc fails?"
        T *new_ptr = (T *)malloc(c * sizeof (T));

        for (size_t i=0; i < size_; ++i) {
            if constexpr (std::is_nothrow_move_constructible_v<T>) {
                // If the elements can be moved without risking
                // an exception, then we'll move the elements.
                ::new (&new_ptr[i]) T(std::move(ptr_[i]));
            } else {
                // If moving the elements might throw an exception,
                // then moving isn't safe. Make a copy of the elements
                // until we're sure that we've succeeded; then destroy
                // the old elements.
                try {
                    ::new (&new_ptr[i]) T(ptr_[i]);
                } catch (...) {
                    destroy_n_elements(new_ptr, i);
                    free(new_ptr);
                    throw;
                }
            }
        }
        // Having successfully moved or copied the elements,
        // destroy the old array and point ptr_ at the new one.
        destroy_n_elements(ptr_, size_);
        free(ptr_);
        ptr_ = new_ptr;
        capacity_ = c;
    }

    ~vector() {
        destroy_n_elements(ptr_, size_);
        free(ptr_);
    }
};
//dex8
void test()
{
    vector<int> vec;
    vec.reserve(10);
    vector<std::string> vec2;
    vec2.reserve(10);
    vec2.reserve(100);
}
} // namespace ex8

namespace ex9 {

template<typename T>
class vector {
    T *ptr_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;

public:
    // ...

    void reserve(size_t c) {
        if (capacity_ >= c) {
            // do nothing
            return;
        }

        // For now, we'll ignore the problem of
        // "What if malloc fails?"
        T *new_ptr = (T *)malloc(c * sizeof (T));

//ex9
        // If the elements can be moved without risking
        // an exception, then we'll move the elements.
        std::conditional_t<
            std::is_nothrow_move_constructible_v<T>,
            std::move_iterator<T*>,
            T*
        > first(ptr_);

        try {
            // Move or copy the elements via a standard algorithm.
            std::uninitialized_copy(first, first + size_, new_ptr);
        } catch (...) {
            free(new_ptr);
            throw;
        }

        // Having successfully moved or copied the elements,
        // destroy the old array and point ptr_ at the new one.
        std::destroy(ptr_, ptr_ + size_);
        free(ptr_);
        ptr_ = new_ptr;
        capacity_ = c;
//dex9
    }

    ~vector() {
        std::destroy(ptr_, ptr_ + size_);
        free(ptr_);
    }
};

void test()
{
    vector<int> vec;
    vec.reserve(10);
    vector<std::string> vec2;
    vec2.reserve(10);
    vec2.reserve(100);
}
} // namespace ex9

namespace ex10 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
void test()
{
//ex10
    std::vector<int> v = {3, 1, 4};

    auto iter = v.begin();
    v.reserve(6);  // iter is invalidated!

    // This might look like a way to produce the result
    // {3, 1, 4, 3, 1, 4}; but if the first insertion
    // triggers reallocation, then the next insertion
    // will be reading garbage from a dangling iterator!
    v = std::vector{3, 1, 4};
    std::copy(
        v.begin(),
        v.end(),
        std::back_inserter(v)
    );
//dex10
//ex11
    auto end = v.end();
    for (auto it = v.begin(); it != end; ++it) {
        if (*it == 4) {
            v.erase(it);  // WRONG!
        }
    }

    // Asking the vector for its .end() each time
    // through the loop does fix the bug...
    for (auto it = v.begin(); it != v.end(); ) {
        if (*it == 4) {
            it = v.erase(it);
        } else {
            ++it;
        }
    }

    // ...But it's much more efficient to use the
    // erase-remove idiom.
    v.erase(
        std::remove_if(v.begin(), v.end(), [](auto&& elt) {
            return elt == 4;
        }),
        v.end()
    );
//dex11
}
#pragma GCC diagnostic pop
} // namespace ex10

namespace ex12 {
void test() {
//ex12
    std::vector<int> v = {1, 2};
    std::vector<int> w = {5, 6};

    // Insert a single element.
    v.insert(v.begin() + 1, 3);
    assert((v == std::vector{1, 3, 2}));

    // Insert n copies of a single element.
    v.insert(v.end() - 1, 3, 4);
    assert((v == std::vector{1, 3, 4, 4, 4, 2}));

    // Insert a whole range of elements.
    v.insert(v.begin() + 3, w.begin(), w.end());
    assert((v == std::vector{1, 3, 4, 5, 6, 4, 4, 2}));

    // Insert a braced list of elements.
    v.insert(v.begin(), {7, 8});
    assert((v == std::vector{7, 8, 1, 3, 4, 5, 6, 4, 4, 2}));
//dex12
}
} // namespace ex12

namespace ex13 {
void test()
{
//ex13
    std::vector<bool> vb = {true, false, true, false};

    // vector<bool>::reference has one public member function:
    vb[3].flip();
    assert(vb[3] == true);

    // The following line won't compile!
    // bool& oops = vb[0];

    auto ref = vb[0];
    assert((!std::is_same_v<decltype(ref), bool>));
    assert(sizeof vb[0] > sizeof (bool));

    if (sizeof std::as_const(vb)[0] == sizeof (bool)) {
        puts("Your library vendor is libstdc++ or Visual Studio");
    } else {
        puts("Your library vendor is libc++");
    }
//dex13
}
} // namespace ex13

namespace ex14 {
//ex14
struct Bad {
    int x = 0;
    Bad() = default;
    Bad(const Bad&) { puts("copy Bad"); }
    Bad(Bad&&) { puts("move Bad"); }
};

struct Good {
    int x = 0;
    Good() = default;
    Good(const Good&) { puts("copy Good"); }
    Good(Good&&) noexcept { puts("move Good"); }
};

class ImplicitlyGood {
    std::string x;
    Good y;
};

class ImplicitlyBad {
    std::string x;
    Bad y;
};
//dex14
//ex15
template<class T>
void test_resizing()
{
    std::vector<T> vec(1);
    // Force a reallocation on the vector.
    vec.resize(vec.capacity() + 1);
}

void test()
{
    test_resizing<Good>();
    test_resizing<Bad>();
    test_resizing<ImplicitlyGood>();
    test_resizing<ImplicitlyBad>();
}
//dex15
} // namespace ex14

namespace ex16 {
void test()
{
//ex16
    std::list<int> a = {3, 6};
    std::list<int> b = {1, 2, 3, 5, 6};

    a.merge(b);
    assert(b.empty());
    assert((a == std::list{1, 2, 3, 3, 5, 6, 6}));
//dex16
}
} // namespace ex16

namespace ex17 {
void test()
{
//ex17
    std::list<int> lst = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    auto begin = std::next(lst.begin(), 2);
    auto end = std::next(lst.end(), -2);

    // Sort just the range [begin, end)
    std::list<int> sub;
    sub.splice(sub.begin(), lst, begin, end);
    sub.sort();
    lst.splice(end, sub);
    assert(sub.empty());

    assert((lst == std::list{3, 1, 1, 2, 4, 5, 9, 6, 5}));
//dex17
}
} // namespace ex17

namespace ex18 {
void test()
{
//ex18
    std::stack<int> stk;
    stk.push(3); stk.push(1); stk.push(4);
    assert(stk.top() == 4);
    stk.pop();
    assert(stk.top() == 1);
    stk.pop();
    assert(stk.top() == 3);
//dex18
//ex19
    std::stack<int> a, b;
    a.push(3); a.push(1); a.push(4);
    b.push(2); b.push(7);
    assert(a != b);

    assert(a.top() < b.top());  // that is, 4 < 7
    assert(a > b);  // because 3 > 2
//dex19
}
} // namespace ex18

namespace ex20 {
void test() {
//ex20
    std::vector<int> vec = {1, 2, 3, 4};
    std::deque<int> deq = {1, 2, 3, 4};
    int *vec_p = &vec[2];
    int *deq_p = &deq[2];
    for (int i=0; i < 1000; ++i) {
        vec.push_back(i);
        deq.push_back(i);
    }
    assert(vec_p != &vec[2]);
    assert(deq_p == &deq[2]);
//dex20
}
} // namespace ex20

namespace ex21 {
void test() {
//ex21
    std::priority_queue<int> pq1;
    std::priority_queue<int, std::vector<int>, std::greater<>> pq2;

    for (int v : {3, 1, 4, 1, 5, 9}) {
        pq1.push(v);
        pq2.push(v);
    }

    assert(pq1.top() == 9);  // max-heap by default
    assert(pq2.top() == 1);  // min-heap by choice
//dex21
}
} // namespace ex21

namespace ex22 {
void test() {
//ex22
    std::set<int> s;
    for (int i : {3, 1, 4, 1, 5}) {
        s.insert(i);
    }

    // A set's items are stored sorted and deduplicated.
    assert((s == std::set{1, 3, 4, 5}));

    auto it = s.begin();
    assert(*it == 1);
    s.erase(4);
    s.erase(it);  // erase *it, which is 1

    assert((s == std::set{3, 5}));
//dex22
}
} // namespace ex22

namespace ex23 {
void test() {
//ex23
    std::set<int> s;
    auto [it1, b1] = s.insert(1);
    assert(*it1 == 1 && b1 == true);

    auto [it2, b2] = s.insert(2);
    assert(*it2 == 2 && b2 == true);

    auto [it3, b3] = s.insert(1); // again
    assert(*it3 == 1 && b3 == false);
//dex23
}
} // namespace ex23

namespace ex24 {
void test() {
//ex24
    std::map<std::string, std::string> m;
    m["hello"] = "world";
    m["quick"] = "brown";
    m["hello"] = "dolly";
    assert(m.size() == 2);
//dex24
//ex25
    assert(m["literally"] == "");
    assert(m.size() == 3);
//dex25
//ex26
    // Confusingly, "value_type" refers to a whole key-value pair.
    // The types K and V are called "key_type" and "mapped_type",
    // respectively.
    using Pair = decltype(m)::value_type;

    if (m.find("hello") == m.end()) {
        m.insert(Pair{"hello", "dolly"});

        // ...or equivalently...
        m.emplace("hello", "dolly");
    }
//dex26
}
} // namespace ex24

namespace ex27 {
void test() {
//ex27
    std::multimap<std::string, std::string> mm;
    mm.emplace("hello", "world");
    mm.emplace("quick", "brown");
    mm.emplace("hello", "dolly");
    assert(mm.size() == 3);

    // Key-value pairs are stored in sorted order.
    // Pairs with identical keys are guaranteed to be
    // stored in the order in which they were inserted.
    auto it = mm.begin();
    using Pair = decltype(mm)::value_type;
    assert(*(it++) == Pair("hello", "world"));
    assert(*(it++) == Pair("hello", "dolly"));
    assert(*(it++) == Pair("quick", "brown"));
//dex27
}
} // namespace ex27

namespace ex28 {
void test() {
//ex28
    std::multimap<std::string, std::string> mm = {
        {"hello", "world"},
        {"quick", "brown"},
        {"hello", "dolly"},
    };
    assert(mm.count("hello") == 2);
    mm.erase("hello");
    assert(mm.count("hello") == 0);
//dex28
}
} // namespace ex28

namespace ex29 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
//ex29
struct MagicLess {
    using is_transparent = std::true_type;

    template<class T, class U>
    bool operator()(T&& t, U&& u) const {
        return std::forward<T>(t) < std::forward<U>(u);
    }
};

void test()
{
    std::map<std::string, std::string, MagicLess> m;

    // The STL provides std::less<> as a synonym for MagicLess.
    std::map<std::string, std::string, std::less<>> m2;

    // Now 'find' no longer constructs a std::string!
    auto it = m2.find("hello");
}
//dex29
#pragma GCC diagnostic pop
} // namespace ex29

namespace ex30 {
void test() {
    std::map<std::string, std::string, std::less<>> m = {
        {"hello", "world"},
        {"quick", "brown"},
    };
//ex30
    auto [begin, end] = m.equal_range("hello");
    m.erase(begin, end);
//dex30
    assert(m.count("hello") == 0);
    assert(m.size() == 1);
}
} // namespace ex30

namespace ex31 {
void test() {
//ex31
    std::map<std::string, std::string> m = {
        {"hello", "world"},
        {"quick", "brown"},
    };
    std::map<std::string, std::string> otherm = {
        {"hello", "dolly"},
        {"sad", "clown"},
    };

    // This should look familiar!
    m.merge(otherm);

    assert((otherm == decltype(m){
        {"hello", "dolly"},
    }));

    assert((m == decltype(m){
        {"hello", "world"},
        {"quick", "brown"},
        {"sad", "clown"},
    }));
//dex31
}
} // namespace ex31

namespace ex32 {
void test() {
//ex32
    std::map<std::string, std::string> m = {
        {"hello", "world"},
        {"quick", "brown"},
    };
    std::map<std::string, std::string> otherm = {
        {"hello", "dolly"},
        {"sad", "clown"},
    };

    using Pair = decltype(m)::value_type;

    // Insertion may succeed...
    auto nh1 = otherm.extract("sad");
    assert(nh1.key() == "sad" && nh1.mapped() == "clown");
    auto [it2, inserted2, nh2] = m.insert(std::move(nh1));
    assert(*it2 == Pair("sad", "clown") && inserted2 == true && nh2.empty());

    // ...or be blocked by an existing element.
    auto nh3 = otherm.extract("hello");
    assert(nh3.key() == "hello" && nh3.mapped() == "dolly");
    auto [it4, inserted4, nh4] = m.insert(std::move(nh3));
    assert(*it4 == Pair("hello", "world") && inserted4 == false && !nh4.empty());

    // Overwriting an existing element is a pain.
    m.insert_or_assign(nh4.key(), nh4.mapped());

    // It is often easiest just to delete the element that's
    // blocking our desired insertion.
    m.erase(it4);
    m.insert(std::move(nh4));
//dex32
}
} // namespace ex32

namespace ex33 {
void test() {
//ex33
    std::map<std::string, std::string> m = {
        {"hello", "world"},
        {"quick", "brown"},
    };
    assert(m.begin()->first == "hello");
    assert(std::next(m.begin())->first == "quick");

    // Upper-case the {"quick", "brown"} mapping, with
    // absolutely no memory allocations anywhere.
    auto nh = m.extract("quick");
    std::transform(nh.key().begin(), nh.key().end(), nh.key().begin(), ::toupper);
    m.insert(std::move(nh));

    assert(m.begin()->first == "QUICK");
    assert(std::next(m.begin())->first == "hello");
//dex33
}
} // namespace ex33

namespace ex34 {
//ex34
class Widget {
public:
    virtual bool IsEqualTo(Widget const *b) const;
    virtual int GetHashValue() const;
};

struct myhash {
    size_t operator()(const Widget *w) const {
        return w->GetHashValue();
    }
};

struct myequal {
    bool operator()(const Widget *a, const Widget *b) const {
        return a->IsEqualTo(b);
    }
};

std::unordered_set<Widget *, myhash, myequal> s;
//dex34
} // namespace ex34

int main()
{
    ex1::test();
    ex2::test();
    ex4::test();
    ex6::test();
    ex7::test();
    ex8::test();
    ex9::test();
    ex12::test();
    ex13::test();
    ex14::test();
    ex16::test();
    ex17::test();
    ex18::test();
    ex20::test();
    ex21::test();
    ex22::test();
    ex23::test();
    ex24::test();
    ex27::test();
    ex28::test();
    ex29::test();
    ex30::test();
    ex31::test();
    ex32::test();
    ex33::test();
}
