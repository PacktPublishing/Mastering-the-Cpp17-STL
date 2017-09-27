#include <algorithm>
#include <boost/iterator/iterator_facade.hpp>
#include <cassert>
#include <cstdio>
#include <cctype>
#include <functional>
#include <iostream>
#include <list>
#include <numeric>
#include <set>
#include <type_traits>
#include <vector>

namespace ex1 {
void test() {
//ex1
    constexpr int N = 10;
    int a[N];

    // A correct for-loop.
    for (int i=0; i < N; ++i) {
        // ...
    }

    // One variety of "smelly" for-loop.
    for (int i=0; i <= N; ++i) {
        // ...
    }

    // A correct invocation of a standard algorithm.
    std::count_if(std::begin(a), std::end(a), [](int){ return true; });

    // A "smelly" invocation.
    std::count_if(std::begin(a), std::end(a) - 1, [](int){ return true; });

    // A "trivial" invocation: counting a range of length zero.
    std::count_if(std::begin(a), std::begin(a), [](int){ return true; });
//dex1
}
} // namespace ex1

namespace ex2 {
void test() {
//ex2
    int a[] {1, 2, 3, 4, 5};
    std::list<int> lst {1, 2, 3, 4, 5};
    std::forward_list<int> flst {1, 2, 3, 4, 5};

    assert(std::distance(std::begin(a), std::end(a)) == 5);
    assert(std::distance(std::begin(lst), std::end(lst)) == 5);
    assert(std::distance(std::begin(flst), std::end(flst)) == 5);

    assert(std::distance(std::end(a), std::begin(a)) == -5);
//dex2
//ex39
    // The following line gives an "incorrect" answer!
    // assert(std::distance(std::end(lst), std::begin(lst)) == 1);
    // And this one just segfaults!
    // std::distance(std::end(flst), std::begin(flst));
//dex39
}
} // namespace ex2

namespace ex3 {
void test() {
//ex3
    std::set<int> s { 1, 2, 3, 10, 42, 99 };
    bool present;

    // O(n): compare each element with 42
    present = std::count(s.begin(), s.end(), 42);

    // O(log n): ask the container to look up 42 itself
    present = s.count(42);
//dex3
    (void)present;
}
} // namespace ex3

namespace ex4 {
//ex4
template<class InputIterator, class UnaryPredicate>
InputIterator find_if(InputIterator first, InputIterator last, UnaryPredicate p)
{
    for (; first != last; ++first) {
        if (p(*first)) {
            return first;
        }
    }
    return last;
}

template<class It, class U>
It find_if_not(It first, It last, U p) {
    return std::find_if(first, last, [&](auto&& e){ return !p(e); });
}

template<class It, class T>
It find(It first, It last, T value) {
    return std::find_if(first, last, [&](auto&& e){ return e == value; });
}
//dex4
//ex5
template<class It, class UnaryPredicate>
bool all_of(It first, It last, UnaryPredicate p)
{
    return std::find_if_not(first, last, p) == last;
}

template <class It, class U>
bool any_of(It first, It last, U p)
{
    return std::find_if(first, last, p) != last;
}

template <class It, class U>
bool none_of(It first, It last, U p)
{
    return std::find_if(first, last, p) == last;
}
//dex5
} // namespace ex4

namespace ex6 {
//ex6
template <class It, class FwdIt>
It find_first_of(It first, It last, FwdIt targetfirst, FwdIt targetlast)
{
    return std::find_if(first, last, [&](auto&& e) {
        return std::any_of(targetfirst, targetlast, [&](auto&& t) {
            return e == t;
        });
    });
}

template <class It, class FwdIt, class BinaryPredicate>
It find_first_of(It first, It last, FwdIt targetfirst, FwdIt targetlast, BinaryPredicate p)
{
    return std::find_if(first, last, [&](auto&& e) {
        return std::any_of(targetfirst, targetlast, [&](auto&& t) {
            return p(e, t);
        });
    });
}
//dex6

void test()
{
    std::vector<int> v{1,2,3,4};
    std::vector<int> t{5,3};
    auto it = ex6::find_first_of(v.begin(), v.end(), t.begin(), t.end());
    auto it2 = std::find_first_of(v.begin(), v.end(), t.begin(), t.end());
    assert(it == it2);
    assert(*it == 3);
}
} // namespace ex6

namespace ex7 {
void test()
{
//ex7
    std::istream_iterator<char> ii(std::cin);
    std::istream_iterator<char> iend{};
    std::string s = "hello";

    // Chomp characters from std::cin until finding an 'h', 'e', 'l', or 'o'.
    std::find_first_of(ii, iend, s.begin(), s.end());
//dex7
}
} // namespace ex7

namespace ex8 {
//ex8
template<class T> constexpr bool is_random_access_iterator_v =
    std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<T>::iterator_category>;

template<class It1, class It2, class B>
auto mismatch(It1 first1, It1 last1, It2 first2, It2 last2, B p)
{
    while (first1 != last1 && first2 != last2 && p(*first1, *first2)) {
        ++first1;
        ++first2;
    }
    return std::make_pair(first1, first2);
}

template<class It1, class It2>
auto mismatch(It1 first1, It1 last1, It2 first2, It2 last2)
{
    return std::mismatch(first1, last1, first2, last2, std::equal_to<>{});
}

template<class It1, class It2, class B>
bool equal(It1 first1, It1 last1, It2 first2, It2 last2, B p)
{
    if constexpr (is_random_access_iterator_v<It1> && is_random_access_iterator_v<It2>) {
        // Ranges of different lengths can never be equal.
        if ((last2 - first2) != (last1 - first1)) {
            return false;
        }
    }
    return std::mismatch(first1, last1, first2, last2, p) == std::make_pair(last1, last2);
}

template<class It1, class It2>
bool equal(It1 first1, It1 last1, It2 first2, It2 last2)
{
    return std::equal(first1, last1, first2, last2, std::equal_to<>{});
}
//dex8
} // namespace ex8

namespace ex9 {
//ex9
template<class InIt, class OutIt>
OutIt copy(InIt first1, InIt last1, OutIt destination)
{
    while (first1 != last1) {
        *destination = *first1;
        ++first1;
        ++destination;
    }
    return destination;
}
//dex9
//ex10
class putc_iterator : public boost::iterator_facade<
    putc_iterator,  // T
    const putc_iterator,  // value_type
    std::output_iterator_tag
>
{
    friend class boost::iterator_core_access;

    auto& dereference() const { return *this; }
    void increment() {}
    bool equal(const putc_iterator&) const { return false; }
public:
    // This iterator is its own proxy object!
    void operator= (char ch) const { putc(ch, stdout); }
};

void test()
{
    std::string s = "hello";
    std::copy(s.begin(), s.end(), putc_iterator{});
}
//dex10
} // namespace ex9
namespace ex11 {
namespace std {
    using ::std::output_iterator_tag;
    using ::std::move;
    using ::std::copy;
    using ::std::string;
    using ::std::vector;
}
//ex11
namespace std {
    template<class Container>
    class back_insert_iterator {
        using CtrValueType = typename Container::value_type;
        Container *c;
    public:
        using iterator_category = output_iterator_tag;
        using difference_type = void;
        using value_type = void;
        using pointer = void;
        using reference = void;

        explicit back_insert_iterator(Container& ctr) : c(&ctr) {}

        auto& operator*() { return *this; }
        auto& operator++() { return *this; }
        auto& operator++(int) { return *this; }

        auto& operator= (const CtrValueType& v) {
            c->push_back(v);
            return *this;
        }
        auto& operator= (CtrValueType&& v) {
            c->push_back(std::move(v));
            return *this;
        }
    };

    template<class Container>
    auto back_inserter(Container& c)
    {
        return back_insert_iterator<Container>(c);
    }
}

void test()
{
    std::string s = "hello";
    std::vector<char> dest;
    std::copy(s.begin(), s.end(), std::back_inserter(dest));
    assert(dest.size() == 5);
}
//dex11
} // namespace ex11

namespace ex12 {
namespace std {
using ::std::remove_reference_t;
using ::std::is_reference_v;
using ::std::conditional_t;
using ::std::iterator_traits;
using ::std::string;
using ::std::vector;
using ::std::copy;

template<class T> decltype(auto) move(T&& t) { return static_cast<remove_reference_t<T>&&>(t); }

//ex12
template<class InIt, class OutIt>
OutIt move(InIt first1, InIt last1, OutIt destination)
{
    while (first1 != last1) {
        *destination = std::move(*first1);
        ++first1;
        ++destination;
    }
    return destination;
}
//dex12
//ex13
template<class It>
class move_iterator {
    using OriginalRefType = typename std::iterator_traits<It>::reference;
    It iter;
public:
    using iterator_category = typename std::iterator_traits<It>::iterator_category;
    using difference_type = typename std::iterator_traits<It>::difference_type;
    using value_type = typename std::iterator_traits<It>::value_type;
    using pointer = It;
    using reference = std::conditional_t<
        std::is_reference_v<OriginalRefType>,
        std::remove_reference_t<OriginalRefType>&&,
        OriginalRefType
    >;

    constexpr move_iterator() = default;
    constexpr explicit move_iterator(It it) : iter(std::move(it)) {}

    // Allow constructing or assigning from any kind of move-iterator.
    // These templates also serve as our own type's copy constructor
    // and assignment operator, respectively.
    template<class U>
    constexpr move_iterator(const move_iterator<U>& m) : iter(m.base()) {}
    template<class U>
    constexpr auto& operator=(const move_iterator<U>& m) { iter = m.base(); return *this; }

    constexpr It base() const { return iter; }

    reference operator*() { return static_cast<reference>(*iter); }
    It operator->() { return iter; }
    constexpr decltype(auto) operator[](difference_type n) const { return *std::move(iter[n]); }

    auto& operator++() { ++iter; return *this; }
    auto& operator++(int) { auto result = *this; ++*this; return result; }
    auto& operator--() { --iter; return *this; }
    auto& operator--(int) { auto result = *this; --*this; return result; }

    constexpr auto& operator+=(difference_type n) const { iter += n; return *this; }
    constexpr auto& operator-=(difference_type n) const { iter -= n; return *this; }
};

// I've omitted the definitions of non-member operators
// == != < <= > >= + - ; can you fill them in?

template<class InputIterator>
auto make_move_iterator(InputIterator& c)
{
    return move_iterator(c);
}
//dex13
template<class T, class U>
bool operator!=(const move_iterator<T>& t, const move_iterator<U>& u)
{
    return t.base() != u.base();
}
template<class T>
bool operator-(const move_iterator<T>& t, const move_iterator<T>& u)
{
    // libstdc++ needs this for std::copy, for whatever reason
    return t.base() - u.base();
}
} // namespace std
void test()
{
//ex14
    std::vector<std::string> input = {"hello", "world"};
    std::vector<std::string> output(2);

    // First approach: use the std::move algorithm
    std::move(input.begin(), input.end(), output.begin());

    // Second approach: use move_iterator
    std::copy(
        std::move_iterator(input.begin()),
        std::move_iterator(input.end()),
        output.begin()
    );
//dex14
}
} // namespace ex12

namespace ex15 {
void test()
{
//ex15
    std::vector<const char *> input = {"hello", "world"};
    std::vector<std::string> output(2);

    std::copy(input.begin(), input.end(), output.begin());

    assert(output[0] == "hello");
    assert(output[1] == "world");
//dex15
}
} // namespace ex15

namespace ex16 {
//ex16
template<class InIt, class OutIt, class Unary>
OutIt transform(InIt first1, InIt last1, OutIt destination, Unary op)
{
    while (first1 != last1) {
        *destination = op(*first1);
        ++first1;
        ++destination;
    }
    return destination;
}

void test()
{
    std::vector<std::string> input = {"hello", "world"};
    std::vector<std::string> output(2);

    std::transform(
        input.begin(),
        input.end(),
        output.begin(),
        [](std::string s) {
            // It works for transforming in-place, too!
            std::transform(s.begin(), s.end(), s.begin(), ::toupper);
            return s;
        }
    );

    assert(input[0] == "hello");
    assert(output[0] == "HELLO");
}
//dex16
} // namespace ex16

namespace ex17 {
//ex17
template<class InIt1, class InIt2, class OutIt, class Binary>
OutIt transform(InIt1 first1, InIt1 last1, InIt2 first2, OutIt destination, Binary op)
{
    while (first1 != last1) {
        *destination = op(*first1, *first2);
        ++first1;
        ++first2;
        ++destination;
    }
    return destination;
}
//dex17
} // namespace ex17

namespace ex18 {
void test()
{
//ex18
    std::vector<std::string> input = {"hello", "world"};
    std::vector<std::string> output(2);

    // Third approach: use std::transform
    std::transform(
        input.begin(),
        input.end(),
        output.begin(),
        std::move<std::string&>
    );
//dex18
    assert(output[0] == "hello");
}
} // namespace ex18

namespace ex19 {
//ex19
template<class FwdIt, class T>
void fill(FwdIt first, FwdIt last, T value) {
    while (first != last) {
        *first = value;
        ++first;
    }
}

template<class FwdIt, class T>
void iota(FwdIt first, FwdIt last, T value) {
    while (first != last) {
        *first = value;
        ++value;
        ++first;
    }
}

template<class FwdIt, class G>
void generate(FwdIt first, FwdIt last, G generator) {
    while (first != last) {
        *first = generator();
        ++first;
    }
}
//dex19
void test()
{
//ex20
    std::vector<std::string> v(4);

    std::fill(v.begin(), v.end(), "hello");
    assert(v[0] == "hello");
    assert(v[1] == "hello");
    assert(v[2] == "hello");
    assert(v[3] == "hello");

    std::iota(v.begin(), v.end(), "hello");
    assert(v[0] == "hello");
    assert(v[1] == "ello");
    assert(v[2] == "llo");
    assert(v[3] == "lo");

    std::generate(v.begin(), v.end(), [i=0]() mutable {
        return ++i % 2 ? "hello" : "world";
    });
    assert(v[0] == "hello");
    assert(v[1] == "world");
    assert(v[2] == "hello");
    assert(v[3] == "world");
//dex20
}
} // namespace ex19

namespace ex21 {
void test()
{
//ex21
    std::vector<int> v = {3, 1, 4, 1, 5, 9};
    std::sort(v.begin(), v.end(), [](auto&& a, auto&& b) {
        return a % 7 < b % 7;
    });
    assert((v == std::vector{1, 1, 9, 3, 4, 5}));
//dex21
}
} // namespace ex21

namespace ex22 {
//ex22
namespace my {
    class obj {
        int v;
    public:
        obj(int value) : v(value) {}

        void swap(obj& other) {
            using std::swap;
            swap(this->v, other.v);
        }
    };

    void swap(obj& a, obj& b) {
        a.swap(b);
    }
} // namespace my

void test()
{
    int i1 = 1, i2 = 2;
    std::vector<int> v1 = {1}, v2 = {2};
    my::obj m1 = 1, m2 = 2;
    using std::swap;
    swap(i1, i2);  // calls std::swap<int>(int&, int&)
    swap(v1, v2);  // calls std::swap(vector&, vector&)
    swap(m1, m2);  // calls my::swap(obj&, obj&)
}
//dex22
} // namespace ex22

namespace ex23 {
//ex23
void reverse_words_in_place(std::string& s)
{
    // First, reverse the whole string.
    std::reverse(s.begin(), s.end());

    // Next, un-reverse each individual word.
    for (auto it = s.begin(); true; ++it) {
        auto next = std::find(it, s.end(), ' ');
        // Reverse the order of letters in this word.
        std::reverse(it, next);
        if (next == s.end()) {
            break;
        }
        it = next;
    }
}

void test()
{
    std::string s = "the quick brown fox jumps over the lazy dog";
    reverse_words_in_place(s);
    assert(s == "dog lazy the over jumps fox brown quick the");
}
//dex23
} // namespace ex23

namespace ex24 {
namespace std {
    using ::std::vector;
    using ::std::swap;
//ex24
template<class BidirIt>
void reverse(BidirIt first, BidirIt last)
{
    while (first != last) {
        --last;
        if (first == last) break;
        using std::swap;
        swap(*first, *last);
        ++first;
    }
}

template<class BidirIt, class Unary>
auto partition(BidirIt first, BidirIt last, Unary p)
{
    while (first != last && p(*first)) {
        ++first;
    }

    while (first != last) {
        do {
            --last;
        } while (last != first && !p(*last));
        if (first == last) break;
        using std::swap;
        swap(*first, *last);
        do {
            ++first;
        } while (first != last && p(*first));
    }
    return first;
}

void test()
{
    std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    auto it = std::partition(v.begin(), v.end(), [](int x) {
        return x % 2 == 0;
    });
    assert(it == v.begin() + 3);
    assert((v == std::vector{6, 2, 4, 1, 5, 9, 1, 3, 5}));
}
//dex24
} // namespace std
} // namespace ex24

namespace ex25 {
namespace std {
using ::std::swap;
using ::std::find_if;
using ::std::find_if_not;
using ::std::vector;
using ::std::reverse_iterator;
//ex25
// Shorthands for "reversing" and "unreversing".
template<class It>
auto rev(It it) {
    return std::reverse_iterator(it);
};

template<class InnerIt>
auto unrev(std::reverse_iterator<InnerIt> it) {
    return it.base();
}

template<class BidirIt, class Unary>
auto partition(BidirIt first, BidirIt last, Unary p)
{
    first = std::find_if_not(first, last, p);

    while (first != last) {
        last = unrev(std::find_if(rev(last), rev(first), p));
        if (first == last) break;
        using std::swap;
        swap(*first++, *--last);
        first = std::find_if_not(first, last, p);
    }
    return first;
}
//dex25
}
void test()
{
    std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    auto it = std::partition(v.begin(), v.end(), [](int x) {
        return x % 2 == 0;
    });
    assert(it == v.begin() + 3);
    assert((v == std::vector{6, 2, 4, 1, 5, 9, 1, 3, 5}));
}
} // namespace ex25

namespace ex26 {
#define make_heap MakeHeap
#define push_heap PushHeap
#define pop_heap PopHeap
#define sort_heap SortHeap
//ex26
template<class RandomIt>
void push_heap(RandomIt a, RandomIt b)
{
    auto child = ((b-1) - a);
    while (child != 0) {
        auto parent = (child - 1) / 2;
        if (a[child] < a[parent]) {
            return;  // max-heap property has been restored
        }
        std::iter_swap(a+child, a+parent);
        child = parent;
    }
}

template<class RandomIt>
void pop_heap(RandomIt a, RandomIt b)
{
    using DistanceT = decltype(b - a);

    std::iter_swap(a, b-1);

    DistanceT parent = 0;
    DistanceT new_heap_size = ((b-1) - a);

    while (true) {
        auto leftchild = 2 * parent + 1;
        auto rightchild = 2 * parent + 2;
        if (leftchild >= new_heap_size) {
            return;
        }
        auto biggerchild = leftchild;
        if (rightchild < new_heap_size && a[leftchild] < a[rightchild]) {
            biggerchild = rightchild;
        }
        if (a[biggerchild] < a[parent]) {
            return;  // max-heap property has been restored
        }
        std::iter_swap(a+parent, a+biggerchild);
        parent = biggerchild;
    }
}

template<class RandomIt>
void make_heap(RandomIt a, RandomIt b)
{
    for (auto it = a; it != b; ) {
        push_heap(a, ++it);
    }
}

template<class RandomIt>
void sort_heap(RandomIt a, RandomIt b)
{
    for (auto it = b; it != a; --it) {
        pop_heap(a, it);
    }
}

template<class RandomIt>
void sort(RandomIt a, RandomIt b)
{
    make_heap(a, b);
    sort_heap(a, b);
}
//dex26
#undef make_heap
#undef push_heap
#undef pop_heap
#undef sort_heap

void test()
{
    std::vector<int> v = {3,1,4,1,5,9,2,6,5};
    ex26::sort(v.begin(), v.end());
    assert((v == std::vector{1,1,2,3,4,5,5,6,9}));
    ex26::sort(v.begin()+2, v.begin()+2);
    assert((v == std::vector{1,1,2,3,4,5,5,6,9}));
    ex26::sort(v.begin()+2, v.begin()+3);
    assert((v == std::vector{1,1,2,3,4,5,5,6,9}));
}
} // namespace ex26

namespace ex27 {
namespace std {
    using ::std::vector;
    using ::std::inplace_merge;
    using ::std::distance;
//ex27
template<class RandomIt>
void sort(RandomIt a, RandomIt b)
{
    auto n = std::distance(a, b);
    if (n >= 2) {
        auto mid = a + n/2;
        std::sort(a, mid);
        std::sort(mid, b);
        std::inplace_merge(a, mid, b);
    }
}
//dex27
} // namespace std
void test()
{
    std::vector<int> v = {3,1,4,1,5,9,2,6,5};
    std::sort(v.begin(), v.end());
    assert((v == std::vector{1,1,2,3,4,5,5,6,9}));
    std::sort(v.begin()+2, v.begin()+2);
    assert((v == std::vector{1,1,2,3,4,5,5,6,9}));
    std::sort(v.begin()+2, v.begin()+3);
    assert((v == std::vector{1,1,2,3,4,5,5,6,9}));
}
} // namespace ex27

namespace ex28 {
namespace std {
    using ::std::vector;
    using ::std::find;
    using ::std::reverse;
//ex28
template<class FwdIt>
FwdIt rotate(FwdIt a, FwdIt mid, FwdIt b)
{
    auto result = a + (b - mid);

    // First, reverse the whole range.
    std::reverse(a, b);

    // Next, un-reverse each individual segment.
    std::reverse(a, result);
    std::reverse(result, b);

    return result;
}

void test()
{
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    auto five = std::find(v.begin(), v.end(), 5);
    auto one = std::rotate(v.begin(), five, v.end());
    assert((v == std::vector{5, 6, 1, 2, 3, 4}));
    assert(*one == 1);
}
//dex28
} // namespace std
} // namespace ex28

namespace ex29 {
void test()
{
//ex29
    std::vector<int> p = {10, 20, 30};
    std::vector<std::vector<int>> results;

    // Collect the permutations of these three elements.
    for (int i=0; i < 6; ++i) {
        results.push_back(p);
        std::next_permutation(p.begin(), p.end());
    }

    assert((results == std::vector<std::vector<int>>{
        {10, 20, 30},
        {10, 30, 20},
        {20, 10, 30},
        {20, 30, 10},
        {30, 10, 20},
        {30, 20, 10},
    }));
//dex29
}
} // namespace ex29

namespace ex30 {
//ex30
template<class FwdIt, class T, class C>
FwdIt lower_bound(FwdIt first, FwdIt last, const T& value, C lessthan)
{
    using DiffT = typename std::iterator_traits<FwdIt>::difference_type;
    FwdIt it;
    DiffT count = std::distance(first, last);

    while (count > 0) {
        DiffT step = count / 2;
        it = first;
        std::advance(it, step);
        if (lessthan(*it, value)) {
            ++it;
            first = it;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

template<class FwdIt, class T>
FwdIt lower_bound(FwdIt first, FwdIt last, const T& value)
{
    return std::lower_bound(first, last, value, std::less<>{});
}
//dex30
void test()
{
    std::vector<std::string> v = {"hello", "world"};
    auto it = ex30::lower_bound(v.begin(), v.end(), "literally");
    assert(it == v.begin()+1);
}
} // namespace ex30

namespace ex31 {
void test()
{
//ex31
    std::vector<int> vec = {3, 7};
    for (int value : {1, 5, 9}) {
        // Find the appropriate insertion point...
        auto it = std::lower_bound(vec.begin(), vec.end(), value);
        // ...and insert our value there.
        vec.insert(it, value);
    }
    // The vector has remained sorted.
    assert((vec == std::vector{1, 3, 5, 7, 9}));
//dex31
}
} // namespace ex31

namespace ex32 {
void test()
{
//ex32
    std::vector<int> vec = {2, 3, 3, 3, 4};
    auto lower = std::lower_bound(vec.begin(), vec.end(), 3);

    // First approach:
    // upper_bound's interface is identical to lower_bound's.
    auto upper = std::upper_bound(vec.begin(), vec.end(), 3);

    // Second approach:
    // We don't need to binary-search the whole array the second time.
    auto upper2 = std::upper_bound(lower, vec.end(), 3);
    assert(upper2 == upper);

    // Third approach:
    // Linear scan from the lower bound might well be faster
    // than binary search if our total range is really big.
    auto upper3 = std::find_if(lower, vec.end(), [](int v) {
        return v != 3;
    });
    assert(upper3 == upper);

    // No matter which approach we take, this is what we end up with.
    assert(*lower >= 3);
    assert(*upper > 3);
    assert(std::all_of(lower, upper, [](int v) { return v == 3; }));
//dex32
}
} // namespace ex32

namespace ex33 {
void test()
{
//ex33
    std::vector<int> vec = {1, 3, 3, 4, 6, 8};

    // Partition our vector so that all the non-3s are at the front
    // and all the 3s are at the end.
    auto first_3 = std::stable_partition(
        vec.begin(), vec.end(), [](int v){ return v != 3; }
    );

    assert((vec == std::vector{1, 4, 6, 8, 3, 3}));

    // Now erase the "tail" of our vector.
    vec.erase(first_3, vec.end());

    assert((vec == std::vector{1, 4, 6, 8}));
//dex33
}
} // namespace ex33

namespace ex34 {
//ex34
template<class FwdIt, class T>
FwdIt remove(FwdIt first, FwdIt last, const T& value)
{
    auto out = std::find(first, last, value);
    if (out != last) {
        auto in = out;
        while (++in != last) {
            if (*in == value) {
                // don't bother with this item
            } else {
                *out++ = std::move(*in);
            }
        }
    }
    return out;
}

void test()
{
    std::vector<int> vec = {1, 3, 3, 4, 6, 8};

    // Partition our vector so that all the non-3s are at the front.
    auto new_end = std::remove(
        vec.begin(), vec.end(), 3
    );

    // std::remove_if doesn't preserve the "removed" elements.
    assert((vec == std::vector{1, 4, 6, 8, 6, 8}));

    // Now erase the "tail" of our vector.
    vec.erase(new_end, vec.end());

    assert((vec == std::vector{1, 4, 6, 8}));

    // Or, do both steps together in a single line.
    // This is the "erase-remove idiom":
    vec.erase(
        std::remove(vec.begin(), vec.end(), 3),
        vec.end()
    );

    // But if the array is very long, and we know it's sorted,
    // then perhaps it would be better to binary-search for
    // the elements to erase.
    // Here the "shifting-down" is still happening, but it's
    // happening inside vector::erase instead of inside std::remove.
    auto first = std::lower_bound(vec.begin(), vec.end(), 3);
    auto last = std::upper_bound(first, vec.end(), 3);
    vec.erase(first, last);
}
//dex34
} // namespace ex34

namespace ex35 {
void test()
{
//ex35
    std::vector<int> vec = {1, 2, 2, 3, 3, 3, 1, 3, 3};

    vec.erase(
        std::unique(vec.begin(), vec.end()),
        vec.end()
    );

    assert((vec == std::vector{1, 2, 3, 1, 3}));
//dex35
}
} // namespace ex35

namespace ex36 {
//ex36
namespace my {
    template<class BidirIt, class T>
    BidirIt unstable_remove(BidirIt first, BidirIt last, const T& value)
    {
        while (true) {
            // Find the first instance of "value"...
            first = std::find(first, last, value);
            // ...and the last instance of "not value"...
            do {
                if (first == last) {
                    return last;
                }
                --last;
            } while (*last == value);
            // ...and move the latter over top of the former.
            *first = std::move(*last);
            // Rinse and repeat.
            ++first;
        }
    }
} // namespace my

void test()
{
    std::vector<int> vec = {4, 1, 3, 6, 3, 8};

    vec.erase(
        my::unstable_remove(vec.begin(), vec.end(), 3),
        vec.end()
    );

    assert((vec == std::vector{4, 1, 8, 6}));
}
//dex36
} // namespace ex36

namespace ex37 {
//ex37
template<class T>
void destroy_at(T *p)
{
    p->~T();
}

template<class FwdIt>
void destroy(FwdIt first, FwdIt last)
{
    for ( ; first != last; ++first) {
        std::destroy_at(std::addressof(*first));
    }
}
//dex37
//ex38
template<class It, class FwdIt>
FwdIt uninitialized_copy(It first, It last, FwdIt out)
{
    using T = typename std::iterator_traits<FwdIt>::value_type;
    FwdIt old_out = out;
    try {
        while (first != last) {
            ::new (static_cast<void*>(std::addressof(*out))) T(*first);
            ++first;
            ++out;
        }
        return out;
    } catch (...) {
        std::destroy(old_out, out);
        throw;
    }
}

void test()
{
    alignas(std::string) char b[5 * sizeof (std::string)];
    std::string *sb = reinterpret_cast<std::string *>(b);

    std::vector<const char *> vec = {"quick", "brown", "fox"};

    // Construct three std::strings.
    auto end = std::uninitialized_copy(vec.begin(), vec.end(), sb);

    assert(end == sb + 3);

    // Destroy three std::strings.
    std::destroy(sb, end);
}
//dex38
} // namespace ex37

int main()
{
    ex1::test();
    ex2::test();
    ex3::test();
    ex6::test();
    ex7::test();
    ex9::test();
    ex11::test();
    ex12::test();
    ex15::test();
    ex18::test();
    ex19::test();
    ex21::test();
    ex22::test();
    ex23::test();
    ex24::std::test();
    ex25::test();
    ex26::test();
    ex27::test();
    ex28::std::test();
    ex29::test();
    ex30::test();
    ex31::test();
    ex32::test();
    ex33::test();
    ex34::test();
    ex35::test();
    ex36::test();
    ex37::test();
}
