#include <algorithm>
#include <any>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <thread>
#include <tuple>
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>
#include <boost/variant.hpp>

namespace ex1 {
//ex1
char *greet(const char *name) {
    char buffer[100];
    snprintf(buffer, 100, "hello %s", name);
    return strdup(buffer);
}

void test() {
    const char *who = "world";
    char *hw = greet(who);
    assert(strcmp(hw, "hello world") == 0);
    free(hw);
}
//dex1
}//namespace ex1

namespace ex2 {
//ex2
std::string greet(const std::string& name) {
    return "hello " + name;
}

void test() {
    std::string who = "world";
    assert(greet(who) == "hello world");
}
//dex2
}//namespace ex2

namespace ex3 {
//ex3
enum class Color {
    RED = 1,
    BLACK = 2,
};

enum class Size {
    SMALL = 1,
    MEDIUM = 2,
    LARGE = 3,
};
//dex3
#if 0
//ex4
data SixType = ColorandSizeOf Color Size;

data FiveType = ColorOf Color | SizeOf Size;
//dex4
#endif
//ex5
using sixtype = std::pair<Color, Size>;

using fivetype = std::variant<Color, Size>;
//dex5
}//namespace ex3

namespace ex6 {
//ex6
template<class A, class B>
struct pair {
    A first;
    B second;
};
//dex6
} // namespace ex6

namespace ex7 {
void test() {
//ex7
    std::string s;
    int i;

    // Assign both s and i at once.
    std::tie(s, i) = std::make_tuple("hello", 42);
//dex7
}
} // namespace ex7

namespace ex8 {
void test() {
//ex8
    using Author = std::pair<std::string, std::string>;
    std::vector<Author> authors = {
        {"Fyodor", "Dostoevsky"},
        {"Sylvia", "Plath"},
        {"Vladimir", "Nabokov"},
        {"Douglas", "Hofstadter"},
    };

    // Sort by first name then last name.
    std::sort(
        authors.begin(), authors.end(),
        [](auto&& a, auto&& b) {
            return std::tie(a.first, a.second) < std::tie(b.first, b.second);
        }
    );
    assert(authors[0] == Author("Douglas", "Hofstadter"));

    // Sort by last name then first name.
    std::sort(
        authors.begin(), authors.end(),
        [](auto&& a, auto&& b) {
            return std::tie(a.second, a.first) < std::tie(b.second, b.first);
        }
    );
    assert(authors[0] == Author("Fyodor", "Dostoevsky"));
//dex8
}
} // namespace ex8

namespace ex9 {
//ex9
template<class T>
constexpr size_t tuple_size(T&&) {
    return std::tuple_size_v<
        std::remove_reference_t<T>
    >;
}

template<class... Ts>
constexpr size_t simpler_tuple_size(
    const std::tuple<Ts...>&
) {
    return sizeof...(Ts);
}
//dex9
void test()
{
    std::tuple<int, double> t;
    static_assert(tuple_size(t) == 2);
    static_assert(tuple_size(std::make_tuple(1,2,3)) == 3);
    static_assert(simpler_tuple_size(t) == 2);
    static_assert(simpler_tuple_size(std::make_tuple(1,2,3)) == 3);
}
} // namespace ex9

namespace ex10 {
void test() {
//ex10
    auto [i, j, k] = std::tuple{1, 2, 3};

    // make_tuple decays reference_wrapper...
    auto t1 = std::make_tuple(i, std::ref(j), k);
    static_assert(std::is_same_v< decltype(t1),
        std::tuple<int, int&, int>
    >);

    // ...whereas the deduced constructor does not.
    auto t2 = std::tuple(i, std::ref(j), k);
    static_assert(std::is_same_v< decltype(t2),
        std::tuple<int, std::reference_wrapper<int>, int>
    >);
//dex10
}
} // namespace ex10

namespace ex11 {
//ex11
template<typename F>
void run_zeroarg(const F& f);

template<typename F, typename... Args>
void run_multiarg(const F& f, Args&&... args)
{
    auto fwd_args = std::forward_as_tuple(
        std::forward<Args>(args)...
    );
    auto lambda = [&f, fwd_args]() {
        std::apply(f, fwd_args);
    };
    run_zeroarg(f);
}
//dex11
} // namespace ex11

namespace ex12 {
//ex12
namespace std {
    template<typename T>
    class reference_wrapper {
        T *m_ptr;
    public:
        reference_wrapper(T& t) noexcept : m_ptr(&t) {}

        operator T& () const noexcept { return *m_ptr; }
        T& get() const noexcept { return *m_ptr; }
    };

    template<typename T>
    reference_wrapper<T> ref(T& t);
} // namespace std
//dex12
} // namespace ex12

namespace ex13 {
void test() {
//ex13
    int result = 0;
    auto task = [](int& r) {
        r = 42;
    };

    // Trying to use a native reference wouldn't compile.
    //std::thread t(task, result);

    // Correctly pass result "by reference" to the new thread.
    std::thread t(task, std::ref(result));
//dex13
    t.join();
    assert(result == 42);
}
} // namespace ex13

namespace ex14 {
void test() {
//ex14
    std::variant<int, double> v1;

    v1 = 1;  // activate the "int" member
    assert(v1.index() == 0);
    assert(std::get<0>(v1) == 1);

    v1 = 3.14;  // activate the "double" member
    assert(v1.index() == 1);
    assert(std::get<1>(v1) == 3.14);
    assert(std::get<double>(v1) == 3.14);

    assert(std::holds_alternative<int>(v1) == false);
    assert(std::holds_alternative<double>(v1) == true);

    assert(std::get_if<int>(&v1) == nullptr);
    assert(*std::get_if<double>(&v1) == 3.14);
//dex14
//ex15
    // Worst...
    try {
        std::cout << std::get<int>(v1) << std::endl;
    } catch (const std::bad_variant_access&) {}

    // Still bad...
    if (v1.index() == 0) {
        std::cout << std::get<int>(v1) << std::endl;
    }

    // Slightly better...
    if (std::holds_alternative<int>(v1)) {
        std::cout << std::get<int>(v1) << std::endl;
    }

    // ...Best.
    if (int *p = std::get_if<int>(&v1)) {
        std::cout << *p << std::endl;
    }
//dex15
}
} // namespace ex14

namespace ex16 {
//ex16
struct Visitor {
    double operator()(double d) { return d; }
    double operator()(int i) { return double(i); }
    double operator()(const std::string&) { return -1; }
};

using Var = std::variant<int, double, std::string>;

void show(Var v)
{
    std::cout << std::visit(Visitor{}, v) << std::endl;
}

void test()
{
    show(3.14);
    show(1);
    show("hello world");
}
//dex16
} // namespace ex16

namespace ex17 {
using Var = std::variant<int, double, std::string>;

void show(Var v)
{
//ex17
    std::visit([](const auto& alt) {
        if constexpr (std::is_same_v<
            decltype(alt), const std::string&
        >) {
            std::cout << double(-1) << std::endl;
        } else {
            std::cout << double(alt) << std::endl;
        }
    }, v);
//dex17
}

void test() {
    show(3.14);
    show(1);
    show("hello world");
}
} // namespace ex17

namespace ex18 {
//ex18
struct MultiVisitor {
    template<class T, class U, class V>
    void operator()(T, U, V) const { puts("wrong"); }

    void operator()(char, int, double) const { puts("right!"); }
};

void test()
{
    std::variant<int, double, char> v1 = 'x';
    std::variant<char, int, double> v2 = 1;
    std::variant<double, char, int> v3 = 3.14;
    std::visit(MultiVisitor{}, v1, v2, v3); // prints "right!"
}
//dex18
} // namespace ex18

namespace ex19 {
//ex19
struct A {
    A() { throw "ha ha!"; }
};
struct B {
    operator int () { throw "ha ha!"; }
};
struct C {
    C() = default;
    C& operator=(C&&) = default;
    C(C&&) { throw "ha ha!"; }
};

void test()
{
    std::variant<int, A, C> v1 = 42;

    try {
        v1.emplace<A>();
    } catch (const char *haha) {}
    assert(v1.valueless_by_exception());

    try {
        v1.emplace<int>(B());
    } catch (const char *haha) {}
    assert(v1.valueless_by_exception());
}
//dex19
} // namespace ex19

namespace ex20 {
struct A {
    A() { throw "ha ha!"; }
};
struct B {
    operator int () { throw "ha ha!"; }
};
struct C {
    C() = default;
    C& operator=(C&&) = default;
    C(C&&) { throw "ha ha!"; }
};

void test()
{
    std::variant<int, A, C> v1 = 42;

//ex20
    v1 = 42;

    // Constructing the right-hand side of this assignment
    // will throw; yet the variant is unaffected.
    try { v1 = A(); } catch (...) {}
    assert(std::get<int>(v1) == 42);

    // In this case as well.
    try { v1 = B(); } catch (...) {}
    assert(std::get<int>(v1) == 42);

    // But a throwing move-constructor can still foul it up.
    try { v1 = C(); } catch (...) {}
    assert(v1.valueless_by_exception());
//dex20
}
} // namespace ex20

namespace ex21 {
static void use(int) {}
static int some_default = 0;
//ex21
std::map<std::string, int> g_limits = {
    { "memory", 655360 }
};

std::variant<std::monostate, int>
get_resource_limit(const std::string& key)
{
    if (auto it = g_limits.find(key); it != g_limits.end()) {
        return it->second;
    }
    return std::monostate{};
}

void test()
{
    auto limit = get_resource_limit("memory");
    if (std::holds_alternative<int>(limit)) {
        use( std::get<int>(limit) );
    } else {
        use( some_default );
    }
}
//dex21
} // namespace ex21

namespace ex22 {
static void use(int) {}
static int some_default = 0;
std::map<std::string, int> g_limits = {
    { "memory", 655360 }
};
//ex22
std::optional<int>
get_resource_limit(const std::string& key)
{
    if (auto it = g_limits.find(key); it != g_limits.end()) {
        return it->second;
    }
    return std::nullopt;
}

void test()
{
    auto limit = get_resource_limit("memory");
    if (limit.has_value()) {
        use( *limit );
    } else {
        use( some_default );
    }
}
//dex22
} // namespace ex22

namespace ex23 {
static void use(int i) { assert(i == 42); }
static int some_default = 42;
std::optional<int> get_resource_limit(const std::string&) { return std::nullopt; }

//ex23
std::optional<int> get_resource_limit(const std::string&);

void test() {
    auto limit = get_resource_limit("memory");
    use( limit.value_or(some_default) );
}
//dex23
} // namespace ex23

namespace ex24 {
//ex24
auto make_lambda(int arg) {
    return [arg](int x) { return x + arg; };
}
using L = decltype(make_lambda(0));

static_assert(!std::is_default_constructible_v<L>);
static_assert(!std::is_move_assignable_v<L>);
//dex24
//ex25
class ProblematicAdder {
    L fn_;
};

static_assert(!std::is_default_constructible_v<ProblematicAdder>);
//dex25
//ex26
class Adder {
    std::optional<L> fn_;
public:
    void setup(int first_arg) {
        fn_.emplace(make_lambda(first_arg));
    }
    int call(int second_arg) {
        // this will throw unless setup() was called first
        return fn_.value()(second_arg);
    }
};

static_assert(std::is_default_constructible_v<Adder>);

void test() {
    Adder adder;
    adder.setup(4);
    int result = adder.call(5);
    assert(result == 9);
}
//dex26
} // namespace ex24

namespace ex27 {
#if 0
//ex27
using JSONValue = std::variant<
    std::nullptr_t,
    bool,
    double,
    std::string,
    std::vector<JSONValue>,
    std::map<std::string, JSONValue>
>;
//dex27
#endif
} // namespace ex27

namespace ex28 {
//ex28
using JSONValue = boost::variant<
    std::nullptr_t,
    bool,
    double,
    std::string,
    std::vector<boost::recursive_variant_>,
    std::map<std::string, boost::recursive_variant_>
>;
//dex28
} // namespace ex28

namespace ex29 {
//ex29
struct JSONValue {
    std::variant<
        std::nullptr_t,
        bool,
        double,
        std::string,
        std::vector<JSONValue>,
        std::map<std::string, JSONValue>
    > value_;
};
//dex29
} // namespace ex29

namespace ex30 {
void use(std::string&) {}
void test()
{
//ex30
    std::any a;  // construct an empty container

    assert(!a.has_value());

    a = std::string("hello");
    assert(a.has_value());
    assert(a.type() == typeid(std::string));

    a = 42;
    assert(a.has_value());
    assert(a.type() == typeid(int));
//dex30
//ex31
    if (std::string *p = std::any_cast<std::string>(&a)) {
        use(*p);
    } else {
        // go fish!
    }

    try {
        std::string& s = std::any_cast<std::string&>(a);
        use(s);
    } catch (const std::bad_any_cast&) {
        // go fish!
    }
//dex31
}
} // namespace ex30

namespace ex32 {
//ex32
template<class T> struct Widget {};

std::any get_widget() {
    return std::make_any<Widget<int>>();
}
//dex32
} // namespace ex32

namespace ex33 {
template<class F> int hypothetical_any_visit(F&&, std::any) { return 1; }
template<class T> struct Widget;
std::any get_widget() {
    return std::make_any<Widget<int>>();
}
//ex33
template<class T> struct Widget {};

template<class T> int size(Widget<T>& w) {
    return sizeof w;
}

void test()
{
    std::any a = get_widget();
    int sz = hypothetical_any_visit([](auto&& w){
        return size(w);
    }, a);
    assert(sz == sizeof(Widget<int>));
}
//dex33
} // namespace ex33

namespace ex34 {
//ex34
struct Animal {
    virtual ~Animal() = default;
};

struct Cat : Animal {};

void test()
{
    std::any a = Cat{};

    // The held object is a "Cat"...
    assert(a.type() == typeid(Cat));
    assert(std::any_cast<Cat>(&a) != nullptr);

    // Asking for a base "Animal" will not work.
    assert(a.type() != typeid(Animal));
    assert(std::any_cast<Animal>(&a) == nullptr);

    // Asking for void* certainly will not work!
    assert(std::any_cast<void>(&a) == nullptr);
}
//dex34
} // namespace ex34

namespace ex35 {
template<typename T> struct AnyImpl;
struct AnyBase;
class any {
    std::unique_ptr<AnyBase> p_ = nullptr;
public:
    template<typename T, typename... Args> std::decay_t<T>& emplace(Args&&... args);
    void reset() noexcept;
    const std::type_info& type() const;
    any(const any& rhs);
    any& operator=(const any& rhs);
};
//ex35
class any;

struct AnyBase {
    virtual const std::type_info& type() = 0;
    virtual void copy_to(any&) = 0;
    virtual void move_to(any&) = 0;
    virtual ~AnyBase() = default;
};

template<typename T>
struct AnyImpl : AnyBase {
    T t_;
    const std::type_info& type() {
        return typeid(T);
    }
    void copy_to(any& rhs) override {
        rhs.emplace<T>(t_);
    }
    void move_to(any& rhs) override {
        rhs.emplace<T>(std::move(t_));
    }
    // the destructor doesn't need anything
    // special in this case
};
//dex35
} // namespace ex35

namespace ex36 {

class any;

struct AnyBase {
    virtual const std::type_info& type() = 0;
    virtual void copy_to(any&) = 0;
    virtual void move_to(any&) = 0;
    virtual ~AnyBase() = default;
};

template<typename T>
struct AnyImpl : AnyBase {
    T t_;
    const std::type_info& type();
    void copy_to(any& rhs) override;
    void move_to(any& rhs) override;
};

//ex36
class any {
    std::unique_ptr<AnyBase> p_ = nullptr;
public:
    template<typename T, typename... Args>
    std::decay_t<T>& emplace(Args&&... args) {
        p_ = std::make_unique<AnyImpl<T>>(std::forward<Args>(args)...);
    }

    bool has_value() const noexcept {
        return (p_ != nullptr);
    }

    void reset() noexcept {
        p_ = nullptr;
    }

    const std::type_info& type() const {
        return p_ ? p_->type() : typeid(void);
    }

    any(const any& rhs) {
        *this = rhs;
    }

    any& operator=(const any& rhs) {
        if (rhs.has_value()) {
            rhs.p_->copy_to(*this);
        }
        return *this;
    }
};
//dex36
} // namespace ex36

namespace ex37 {
//ex37
using Ptr = std::unique_ptr<int>;

template<class T>
struct Shim {
    T get() { return std::move(*t_); }

    template<class... Args>
    Shim(Args&&... args) : t_(std::in_place, std::forward<Args>(args)...) {}

    Shim(Shim&&) = default;
    Shim& operator=(Shim&&) = default;
    Shim(const Shim&) { throw "oops"; }
    Shim& operator=(const Shim&) { throw "oops"; }
private:
    std::optional<T> t_;
};

void test()
{
    Ptr p = std::make_unique<int>(42);

    // Ptr cannot be stored in std::any because it is move-only.
    // std::any a = std::move(p);

    // But Shim<Ptr> can be!
    std::any a = Shim<Ptr>(std::move(p));
    assert(a.type() == typeid(Shim<Ptr>));

    // Moving a Shim<Ptr> is okay...
    std::any b = std::move(a);

    try {
        // ...but copying a Shim<Ptr> will throw.
        std::any c = b;
    } catch (...) {}

    // Get the move-only Ptr back out of the Shim<Ptr>.
    Ptr r = std::any_cast<Shim<Ptr>&>(b).get();
    assert(*r == 42);
}
//dex37
} // namespace ex37

namespace ex38 {
//ex38
int my_abs(int x) { return x < 0 ? -x : x; }
long unusual(long x, int y = 3) { return x + y; }

void test()
{
    std::function<int(int)> f;  // construct an empty container
    assert(!f);

    f = my_abs;  // store a function in the container
    assert(f(-42) == 42);

    f = [](long x) { return unusual(x); }; // or a lambda!
    assert(f(-42) == -39);
}
//dex38
} // namespace ex38

namespace ex39 {
void test() {
    std::function<int(int)> f;  // construct an empty container
//ex39
    f = [i=0](int) mutable { return ++i; };
    assert(f(-42) == 1);
    assert(f(-42) == 2);

    auto g = f;
    assert(f(-42) == 3);
    assert(f(-42) == 4);
    assert(g(-42) == 3);
    assert(g(-42) == 4);
//dex39
}
} // namespace ex39

namespace ex40 {
void use(int (*)(int)) {}
void test() {
    std::function<int(int)> f;
//ex40
    if (f.target_type() == typeid(int(*)(int))) {
        int (*p)(int) = *f.target<int (*)(int)>();
        use(p);
    } else {
        // go fish!
    }
//dex40
}
} // namespace ex40

namespace ex41 {
void test() {
//ex41
    auto capture = [](auto& p) {
        using T = std::decay_t<decltype(p)>;
        return std::make_shared<T>(std::move(p));
    };

    std::promise<int> p;

    std::function<void()> f = [sp = capture(p)]() {
        sp->set_value(42);
    };
//dex41
}
} // namespace ex41

namespace ex42 {
//ex42
// templated_for_each is a template and must be visible at the
// point where it is called.
template<class F>
void templated_for_each(std::vector<int>& v, F f) {
    for (int& i : v) {
        f(i);
    }
}

// type_erased_for_each has a stable ABI and a fixed address.
// It can be called with only its declaration in scope.
extern void type_erased_for_each(std::vector<int>&, std::function<void(int)>);
//dex42
} // namespace ex42

int main()
{
    ex1::test();
    ex2::test();
    ex7::test();
    ex8::test();
    ex9::test();
    ex10::test();
    ex13::test();
    ex14::test();
    ex16::test();
    ex17::test();
    ex18::test();
    ex19::test();
    ex20::test();
    ex21::test();
    ex22::test();
    ex23::test();
    ex24::test();
    ex30::test();
    ex33::test();
    ex34::test();
    ex37::test();
    ex38::test();
    ex39::test();
    ex40::test();
    ex41::test();
}
