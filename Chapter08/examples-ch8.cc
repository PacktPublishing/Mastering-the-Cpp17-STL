#include <cassert>
#include <cstddef>
#include <cstdio>
#include <forward_list>
#include <iterator>
#include <list>
#include <map>
#include <memory>
//#include <memory_resource>
#include <new>
#include <scoped_allocator>
#include <type_traits>
#include <vector>

namespace ex1 {
//ex1
struct int_allocator_2014 {
    int *allocate(size_t n, const void *hint = nullptr);
};

struct int_allocator_2017 {
    int *allocate(size_t n);
};
//dex1
} // namespace ex1

namespace ex2 {
//ex2
struct classical_base {
    virtual int *allocate(size_t n) = 0;
};

struct classical_derived : public classical_base {
    int *allocate(size_t n) override;
};
//dex2
} // namespace ex2

namespace ex3 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
//ex3
static char big_buffer[10000];
static size_t index = 0;

void *allocate(size_t bytes) {
    if (bytes > sizeof big_buffer - index) {
        throw std::bad_alloc();
    }
    index += bytes;
    return &big_buffer[index - bytes];
}

void deallocate(void *p, size_t bytes) {
    // drop it on the floor
}
//dex3
#pragma GCC diagnostic pop
namespace two {
//ex4
void deallocate(void *p, size_t bytes) {
    if ((char*)p + bytes == &big_buffer[index]) {
        // aha! we can roll back our index!
        index -= bytes;
    } else {
        // drop it on the floor
    }
}
//dex4
} // namespace two
} // namespace ex3

namespace ex5 {
namespace std::pmr {
//ex5
class memory_resource {
    virtual void *do_allocate(size_t bytes, size_t align) = 0;
    virtual void do_deallocate(void *p, size_t bytes, size_t align) = 0;
    virtual bool do_is_equal(const memory_resource& rhs) const = 0;
public:
    void *allocate(size_t bytes, size_t align) {
        return do_allocate(bytes, align);
    }
    void deallocate(void *p, size_t bytes, size_t align) {
        return do_deallocate(p, bytes, align);
    }
    bool is_equal(const memory_resource& rhs) const {
        return do_is_equal(rhs);
    }
};
//dex5
} // namespace std::pmr
namespace std { using ::std::bad_alloc; }
namespace std { using ::std::max_align_t; }
//ex6
class example_resource : public std::pmr::memory_resource {
    alignas(std::max_align_t) char big_buffer[10000];
    size_t index = 0;
    void *do_allocate(size_t bytes, size_t align) override {
        if (align > alignof(std::max_align_t) ||
            (-index % align) > sizeof big_buffer - index ||
            bytes > sizeof big_buffer - index - (-index % align))
        {
            throw std::bad_alloc();
        }
        index += (-index % align) + bytes;
        return &big_buffer[index - bytes];
    }
    void do_deallocate(void *, size_t, size_t) override {
        // drop it on the floor
    }
    bool do_is_equal(const memory_resource& rhs) const override {
        return this == &rhs;
    }
};
//dex6
//ex7
class UNKNOWN : public std::pmr::memory_resource {
    void *do_allocate(size_t, size_t) override {
        throw std::bad_alloc();
    }
    void do_deallocate(void *, size_t, size_t) override {}
    bool do_is_equal(const memory_resource& rhs) const override {
        return this == &rhs;
    }
};

std::pmr::memory_resource *null_memory_resource() noexcept {
    static UNKNOWN singleton;
    return &singleton;
}
//dex7
void test() {
#if 0  // neither libc++ nor libstdc++ support <memory_resource> yet
//ex8
    alignas(16) char big_buffer[10000];

    std::pmr::monotonic_buffer_resource a(
        big_buffer, sizeof big_buffer,
        std::pmr::null_memory_resource()
    );

    void *p1 = a.allocate(100);
    assert(p1 == big_buffer + 0);

    void *p2 = a.allocate(100, 16);  // alignment
    assert(p1 == big_buffer + 112);

    // Now clear everything allocated so far and start over.
    a.release();
    void *p3 = a.allocate(100);
    assert(p3 == big_buffer + 0);

    // When the buffer is exhausted, a will go upstream
    // to look for more buffers... and not find any.
    try {
        a.allocate(9901);
    } catch (const std::bad_alloc&) {
        puts("The null_memory_resource did its job!");
    }
//dex8
#endif // 0
}
void test2() {
#if 0 // neither libc++ nor libstdc++ support <memory_resource> yet
//ex9
    std::pmr::pool_options options;
    options.max_blocks_per_chunk = 100;
    options.largest_required_pool_block = 256;

    std::pmr::synchronized_pool_resource a(
        options,
        std::pmr::new_delete_resource()
    );
//dex9
    (void)a;
#endif // 0
}
} // namespace ex5

namespace ex10 {
//ex10
template<class T>
struct allocator {
    using value_type = T;

    T *allocate(size_t n) {
        return static_cast<T *>(::operator new(n * sizeof (T)));
    }
    void deallocate(T *p, size_t) {
        ::operator delete(static_cast<void *>(p));
    }

    // NOTE 1
    template<class U>
    explicit allocator(const allocator<U>&) noexcept {}

    // NOTE 2
    allocator() = default;
    allocator(const allocator&) = default;
};
//dex10
void test() {
    std::vector<int, allocator<int>> v;
    v.push_back(42);
    v.clear();
    v.push_back(43);
}
} // namespace ex10

namespace ex11 {
//ex11
template<class T>
struct helloworld {
    using value_type = T;

    T *allocate(size_t n) {
        printf("hello world %zu\n", n);
        return static_cast<T *>(::operator new(n * sizeof (T)));
    }
    void deallocate(T *p, size_t) {
        ::operator delete(static_cast<void *>(p));
    }
};

void test() {
    std::vector<int, helloworld<int>> v;
    v.push_back(42);  // prints "hello world 1"
    v.push_back(42);  // prints "hello world 2"
    v.push_back(42);  // prints "hello world 4"
}
//dex11
} // namespace ex11

namespace ex12 {
template<class T>
struct helloworld {
    using value_type = T;

    T *allocate(size_t n) {
        printf("hello world %zu\n", n);
        return static_cast<T *>(::operator new(n * sizeof (T)));
    }
    void deallocate(T *p, size_t) {
        ::operator delete(static_cast<void *>(p));
    }
};
//ex12
void test() {
    std::list<int, helloworld<int>> v;
    v.push_back(42);
}
//dex12
} // namespace ex12

namespace ex13 {
template<class T>
struct helloworld {
    using value_type = T;

    T *allocate(size_t n) {
        printf("hello world %zu\n", n);
        return static_cast<T *>(::operator new(n * sizeof (T)));
    }
    void deallocate(T *p, size_t) {
        ::operator delete(static_cast<void *>(p));
    }
};
void test() {
//ex13
    using AllocOfInt = helloworld<int>;

    using AllocOfChar =
        std::allocator_traits<AllocOfInt>::rebind_alloc<char>;

    // Now alloc_of_char is helloworld<char>
//dex13
    static_assert(std::is_same_v<AllocOfChar, helloworld<char>>);
}
} // namespace ex13

namespace ex14 {
void test() {
//ex14
    using PtrToInt = int*;

    using PtrToChar =
        std::pointer_traits<PtrToInt>::rebind<char>;

    // Now PtrToChar is char*

    using PtrToConstVoid =
        std::pointer_traits<PtrToInt>::rebind<const void>;

    // Now PtrToConstVoid is const void*
//dex14
    static_assert(std::is_same_v<PtrToChar, char*>);
    static_assert(std::is_same_v<PtrToConstVoid, const void*>);
}
} // namespace ex14

namespace ex15 {
class Chunk;
namespace fake {
//ex15
template<class T>
class ChunkyPtr {
    T *m_ptr = nullptr;
    Chunk *m_chunk = nullptr;
public:
    explicit ChunkyPtr(T *p, Chunk *ch) :
        m_ptr(p), m_chunk(ch) {}

    T& operator *() const {
        return *m_ptr;
    }
    explicit operator T *() const {
        return m_ptr;
    }
    // ... and so on ...

    // ... plus this extra accessor:
    auto chunk() const {
        return m_chunk;
    }
};
//dex15
} // namespace fake

template<class T>
class ChunkyPtr {
    T *m_ptr = nullptr;
    Chunk *m_chunk = nullptr;
public:
    ChunkyPtr() = default;
    ChunkyPtr(std::nullptr_t) {}
    ChunkyPtr& operator--() { --m_ptr; return *this; }
    ChunkyPtr& operator++() { ++m_ptr; return *this; }
    void operator++(int) { ++m_ptr; }
    explicit ChunkyPtr(T *p, Chunk *ch) :
        m_ptr(p), m_chunk(ch) {}
    template<class U> ChunkyPtr(ChunkyPtr<U> p) : m_ptr(static_cast<T*>(static_cast<U*>(p))), m_chunk(p.chunk()) {}
    T& operator *() const {
        return *m_ptr;
    }
    T *operator->() const {
        return m_ptr;
    }
    operator bool() const { return m_ptr; }
    auto operator-(ChunkyPtr<T> p) const { return m_ptr - p.m_ptr; }
    template<class U> auto operator-(U i) const { return ChunkyPtr(m_ptr-i, m_chunk); }
    auto operator+(size_t i) const { return ChunkyPtr(m_ptr+i, m_chunk); }
    auto operator<(ChunkyPtr<T> p) const { return m_ptr < p.m_ptr; }
    explicit operator T *() const {
        return m_ptr;
    }
    auto chunk() const {
        return m_chunk;
    }
    using iterator_category = std::random_access_iterator_tag;
    using pointer = ChunkyPtr<T>;
    using reference = T&;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
//ex38
    static ChunkyPtr<T> pointer_to(T &r) noexcept {
        return ChunkyPtr<T>(&r, nullptr);
    }
//dex38
};
template<class T> bool operator==(ChunkyPtr<T> a, std::nullptr_t) { return static_cast<T*>(a) == nullptr; }
template<class T> bool operator!=(ChunkyPtr<T> a, std::nullptr_t) { return static_cast<T*>(a) != nullptr; }
template<class T> bool operator==(ChunkyPtr<T> a, ChunkyPtr<T> b) { return static_cast<T*>(a) == static_cast<T*>(b); }
template<class T> bool operator!=(ChunkyPtr<T> a, ChunkyPtr<T> b) { return static_cast<T*>(a) != static_cast<T*>(b); }
template<>
class ChunkyPtr<void> {
    void *m_ptr = nullptr;
    Chunk *m_chunk = nullptr;
public:
    explicit ChunkyPtr(void *p, Chunk *ch) :
        m_ptr(p), m_chunk(ch) {}
    template<class U> ChunkyPtr(ChunkyPtr<U> p) : m_ptr(static_cast<U*>(p)), m_chunk(p.chunk()) {}
    explicit operator void *() const {
        return m_ptr;
    }
    auto chunk() const {
        return m_chunk;
    }
};
//ex17
class Chunk {
    char buffer[10000];
    int index = 0;
    int freed = 0;
public:
    bool can_allocate(size_t bytes) {
        return (sizeof buffer - index) >= bytes;
    }
    auto allocate(size_t bytes) {
        index += bytes;
        void *p = &buffer[index - bytes];
        return ChunkyPtr<void>(p, this);
    }
    void deallocate(void *, size_t bytes) {
        freed += bytes;
        if (freed == index) {
            index = freed = 0;
        }
    }
};

class ChunkyMemoryResource {
    std::list<Chunk> m_chunks;
public:
    ChunkyPtr<void> allocate(size_t bytes, size_t align) {
        assert(align <= alignof(std::max_align_t));
        bytes += -bytes % alignof(std::max_align_t);
        assert(bytes <= 10000);

        for (auto&& ch : m_chunks) {
            if (ch.can_allocate(bytes)) {
                return ch.allocate(bytes);
            }
        }
        return m_chunks.emplace_back().allocate(bytes);
    }
    void deallocate(ChunkyPtr<void> p, size_t bytes, size_t) {
        bytes += -bytes % alignof(std::max_align_t);
        p.chunk()->deallocate(static_cast<void*>(p), bytes);
    }
};
//dex17
//ex16
template<class T>
struct ChunkyAllocator {
    using value_type = T;
    using pointer = ChunkyPtr<T>;

    ChunkyAllocator(ChunkyMemoryResource *mr) :
        m_resource(mr) {}

    template<class U>
    ChunkyAllocator(const ChunkyAllocator<U>& rhs) :
        m_resource(rhs.m_resource) {}

    pointer allocate(size_t n) {
        return m_resource->allocate(
            n * sizeof(T), alignof(T));
    }
    void deallocate(pointer p, size_t n) {
        m_resource->deallocate(
            p, n * sizeof(T), alignof(T));
    }
private:
    ChunkyMemoryResource *m_resource;

    template<class U>
    friend struct ChunkyAllocator;
};
//dex16
void test() {
    using AllocOfInt = ChunkyAllocator<int>;
    using AllocOfChar = std::allocator_traits<AllocOfInt>::rebind_alloc<char>;
    static_assert(std::is_same_v<std::allocator_traits<AllocOfChar>::void_pointer, ChunkyPtr<void>>);

//ex18
    ChunkyMemoryResource mr;
    std::vector<int, ChunkyAllocator<int>> v{&mr};
    v.push_back(42);
        // All the memory for v's underlying array
        // is coming from blocks owned by "mr".
//dex18
    // Neither GCC nor Clang support fancy pointers with std::list.
    // std::list<int, ChunkyAllocator<int>> lst{&mr};
    // lst.push_back(42);
}
} // namespace ex15

namespace ex19 {
//ex19
namespace my {

template<class VoidPtr>
class fancy_memory_resource {
public:
    VoidPtr allocate(size_t bytes,
            size_t align = alignof(std::max_align_t)) {
        return do_allocate(bytes, align);
    }
    void deallocate(VoidPtr p, size_t bytes,
            size_t align = alignof(std::max_align_t)) {
        return do_deallocate(p, bytes, align);
    }
    bool is_equal(const fancy_memory_resource& rhs) const noexcept {
        return do_is_equal(rhs);
    }
    virtual ~fancy_memory_resource() = default;
private:
    virtual VoidPtr do_allocate(size_t bytes, size_t align) = 0;
    virtual void do_deallocate(VoidPtr p, size_t bytes, size_t align) = 0;
    virtual bool do_is_equal(const fancy_memory_resource& rhs) const noexcept = 0;
};

using memory_resource = fancy_memory_resource<void*>;

} // namespace my
//dex19
} // namespace ex19

namespace ex21 {
namespace std::pmr {
class memory_resource {
    virtual void *do_allocate(size_t bytes, size_t align) = 0;
    virtual void do_deallocate(void *p, size_t bytes, size_t align) = 0;
    virtual bool do_is_equal(const memory_resource& rhs) const = 0;
public:
    void *allocate(size_t bytes, size_t align) {
        return do_allocate(bytes, align);
    }
    void deallocate(void *p, size_t bytes, size_t align) {
        return do_deallocate(p, bytes, align);
    }
    bool is_equal(const memory_resource& rhs) const {
        return do_is_equal(rhs);
    }
};
class simple_buffer_resource : public memory_resource {
public:
    simple_buffer_resource(void *p, size_t cap) {
        m_buffer = p;
        m_capacity = cap;
        m_size = 0;
    }
    void release() {
        m_size = 0;
    }
private:
    void *m_buffer;
    size_t m_capacity;
    size_t m_size;
    void *do_allocate(size_t bytes, size_t align) override {
        if ((-m_size % align) > (m_capacity - m_size) ||
            bytes > (m_capacity - m_size - (-m_size % align)))
        {
            throw ::std::bad_alloc();
        }
        m_size += (-m_size % align) + bytes;
        return (char*)m_buffer + (m_size - bytes);
    }
    void do_deallocate(void *, size_t, size_t) override {}
    bool do_is_equal(const memory_resource& rhs) const noexcept override {
        return (this == &rhs);
    }
};
using monotonic_buffer_resource = simple_buffer_resource;
} // namespace std::pmr
namespace std {
    using ::std::list;
    using ::std::swap;
    using ::std::vector;
}
//ex21
template<class T>
struct WidgetAlloc {
    std::pmr::memory_resource *mr;

    using value_type = T;

    WidgetAlloc(std::pmr::memory_resource *mr) : mr(mr) {}

    template<class U>
    WidgetAlloc(const WidgetAlloc<U>& rhs) : mr(rhs.mr) {}

    T *allocate(size_t n) {
        return (T *)mr->allocate(n * sizeof(T), alignof(T));
    }
    void deallocate(void *p, size_t n) {
        mr->deallocate(p, n * sizeof(T), alignof(T));
    }
};

class Widget {
    char buffer[10000];
    std::pmr::monotonic_buffer_resource mr {buffer, sizeof buffer};
    std::vector<int, WidgetAlloc<int>> v {&mr};
    std::list<int, WidgetAlloc<int>> lst {&mr};
public:
    static void swap_elems(Widget& a, Widget& b) {
        std::swap(a.v, b.v);
    }
};
//dex21
void test() {
    Widget w1, w2;
    Widget::swap_elems(w1, w2);
}
} // namespace ex21

namespace ex22 {
namespace std {
    using ::std::vector;
    using ::std::map;
    using ::std::less;
    namespace pmr {
        template<class T> class polymorphic_allocator;
    }
}
//ex22
namespace std::pmr {

template<class T>
using vector = std::vector<T,
    polymorphic_allocator<T>>;

template<class K, class V, class Cmp = std::less<K>>
using map = std::map<K, V, Cmp,
    polymorphic_allocator<typename std::map<K, V>::value_type>>;

// ...

} // namespace std::pmr
//dex22
} // namespace ex22

namespace ex23 {
void test() {
#if 0  // neither libc++ nor libstdc++ support <memory_resource> yet
//ex23
    std::pmr::vector<int> v2({1, 2, 3}, std::pmr::new_delete_resource());
        // Specifying a specific memory resource

    std::pmr::vector<int> v1 = {1, 2, 3};
        // Using the default memory resource
//dex23
#endif
}
} // namespace ex23

namespace ex24 {
//ex24
template<class T, class A = std::allocator<T>>
class uniqueish {
    using Traits = std::allocator_traits<A>;
    using FancyPtr = typename Traits::pointer;

    A m_allocator;
    FancyPtr m_ptr = nullptr;

public:
    using allocator_type = A;

    uniqueish(A a = {}) : m_allocator(a) {
        this->emplace();
    }

    ~uniqueish() {
        clear();
    }

    T& value() { return *m_ptr; }
    const T& value() const { return *m_ptr; }

    template<class... Args>
    void emplace(Args&&... args) {
        clear();
        m_ptr = Traits::allocate(m_allocator, 1);
        try {
            T *raw_ptr = static_cast<T *>(m_ptr);
            Traits::construct(m_allocator, raw_ptr,
                std::forward<Args>(args)...
            );
        } catch (...) {
            Traits::deallocate(m_allocator, m_ptr, 1);
            throw;
        }
    }

    void clear() noexcept {
        if (m_ptr) {
            T *raw_ptr = static_cast<T *>(m_ptr);
            Traits::destroy(m_allocator, raw_ptr);
            Traits::deallocate(m_allocator, m_ptr, 1);
            m_ptr = nullptr;
        }
    }
};
//dex24
void test() {
    uniqueish<int> ei;
    ei.emplace(42);
    ei.emplace(32);
}
} // namespace ex24

namespace ex25 {
template<class T, class A = std::allocator<T>>
class uniqueish {
    using Traits = std::allocator_traits<A>;
    using FancyPtr = typename Traits::pointer;

    A m_allocator;
    FancyPtr m_ptr = nullptr;

public:
    using allocator_type = A;

    uniqueish(A a = {}) : m_allocator(a) {
        this->emplace();
    }

    template<class... Args>
    void emplace(Args&&... args) {
        clear();
        m_ptr = Traits::allocate(m_allocator, 1);
        try {
            T *raw_ptr = static_cast<T *>(m_ptr);
            Traits::construct(m_allocator, raw_ptr,
                std::forward<Args>(args)...
            );
        } catch (...) {
            Traits::deallocate(m_allocator, m_ptr, 1);
            throw;
        }
    }

    ~uniqueish() {
        clear();
    }

    void clear() noexcept {
        if (m_ptr) {
            T *raw_ptr = static_cast<T *>(m_ptr);
            Traits::destroy(m_allocator, raw_ptr);
            Traits::deallocate(m_allocator, m_ptr, 1);
            m_ptr = nullptr;
        }
    }

    T& value() { return *m_ptr; }
    const T& value() const { return *m_ptr; }

//ex25
    uniqueish(uniqueish&& rhs) : m_allocator(rhs.m_allocator)
    {
        m_ptr = std::exchange(rhs.m_ptr, nullptr);
    }

    uniqueish& operator=(uniqueish&& rhs)
    {
        constexpr bool pocma =
            Traits::propagate_on_container_move_assignment::value;
        if constexpr (pocma) {
            // We can adopt the new allocator, since
            // our allocator type is not "sticky".
            this->clear();  // using the old allocator
            this->m_allocator = rhs.m_allocator;
            this->m_ptr = std::exchange(rhs.m_ptr, nullptr);
        } else if (m_allocator() == rhs.m_allocator()) {
            // Our allocator is "stuck" to this container;
            // but since it's equivalent to rhs's allocator,
            // we can still adopt rhs's memory.
            this->clear();
            this->m_ptr = std::exchange(rhs.m_ptr, nullptr);
        } else {
            // We must not propagate this new allocator
            // and thus cannot adopt its memory.
            if (rhs.m_ptr) {
                this->emplace(std::move(rhs.value()));
                rhs.clear();
            } else {
                this->clear();
            }
        }
        return *this;
    }
//dex25
//ex26
    void swap(uniqueish& rhs) noexcept {
        constexpr bool pocs =
            Traits::propagate_on_container_swap::value;
        using std::swap;
        if constexpr (pocs) {
            // We can swap allocators, since
            // our allocator type is not "sticky".
            swap(this->m_allocator, rhs.m_allocator);
            swap(this->m_ptr, rhs.m_ptr);
        } else if (m_allocator == rhs.m_allocator) {
            // Our allocator is "stuck" to this container;
            // but since it's equivalent to rhs's allocator,
            // we can still adopt rhs's memory and vice versa.
            swap(this->m_ptr, rhs.m_ptr);
        } else {
            // Neither side can adopt the other's memory, and
            // so one side or the other must allocate.
            auto temp = std::move(*this);
            *this = std::move(rhs);  // might throw
            rhs = std::move(temp);  // might throw
        }
    }
//dex26
};
void test() {
    uniqueish<int> ei;
    uniqueish<int> ej;
    ej.emplace(42);
    ei = std::move(ej);
    ei = std::move(ej);
    ei.swap(ej);
}
} // namespace ex25


namespace ex27 {
template<class T, class A = std::allocator<T>>
class uniqueish {
    using Traits = std::allocator_traits<A>;
    using FancyPtr = typename Traits::pointer;

    A m_allocator;
    FancyPtr m_ptr = nullptr;

public:
    using allocator_type = A;

    uniqueish(A a = {}) : m_allocator(a) {
        this->emplace();
    }

    template<class... Args>
    void emplace(Args&&... args) {
        clear();
        m_ptr = Traits::allocate(m_allocator, 1);
        try {
            T *raw_ptr = static_cast<T *>(m_ptr);
            Traits::construct(m_allocator, raw_ptr,
                std::forward<Args>(args)...
            );
        } catch (...) {
            Traits::deallocate(m_allocator, m_ptr, 1);
            throw;
        }
    }

    ~uniqueish() {
        clear();
    }

    void clear() noexcept {
        if (m_ptr) {
            T *raw_ptr = static_cast<T *>(m_ptr);
            Traits::destroy(m_allocator, raw_ptr);
            Traits::deallocate(m_allocator, m_ptr, 1);
            m_ptr = nullptr;
        }
    }

    T& value() { return *m_ptr; }
    const T& value() const { return *m_ptr; }

    uniqueish(uniqueish&& rhs) : m_allocator(rhs.m_allocator)
    {
        m_ptr = std::exchange(rhs.m_ptr, nullptr);
    }

    uniqueish& operator=(uniqueish&& rhs)
    {
        constexpr bool pocma =
            Traits::propagate_on_container_move_assignment::value;
        if constexpr (pocma) {
            // We can adopt the new allocator, since
            // our allocator type is not "sticky".
            this->clear();  // using the old allocator
            this->m_allocator = rhs.m_allocator;
            this->m_ptr = std::exchange(rhs.m_ptr, nullptr);
        } else if (m_allocator() == rhs.m_allocator()) {
            // Our allocator is "stuck" to this container;
            // but since it's equivalent to rhs's allocator,
            // we can still adopt rhs's memory.
            this->clear();
            this->m_ptr = std::exchange(rhs.m_ptr, nullptr);
        } else {
            // We must not propagate this new allocator
            // and thus cannot adopt its memory.
            if (rhs.m_ptr) {
                this->emplace(std::move(rhs.value()));
                rhs.clear();
            } else {
                this->clear();
            }
        }
        return *this;
    }
//ex27
    void swap(uniqueish& rhs) noexcept {
        constexpr bool pocs =
            Traits::propagate_on_container_swap::value;
        using std::swap;
        if constexpr (pocs) {
            swap(this->m_allocator, rhs.m_allocator);
        }
        // Don't even check that we know how to free
        // the adopted pointer; just assume that we can.
        swap(this->m_ptr, rhs.m_ptr);
    }
//dex27
};
void test() {
    uniqueish<int> ei;
    uniqueish<int> ej;
    ej.emplace(42);
    ei = std::move(ej);
    ei = std::move(ej);
    ei.swap(ej);
}
} // namespace ex27

namespace ex28 {
void test() {
#if 0  // neither libc++ nor libstdc++ support <memory_resource> yet
//ex28
    char buffer[100];
    auto mr = std::pmr::monotonic_buffer_resource(buffer, 100);

    std::pmr::vector<int> a {1,2,3};
    std::pmr::vector<int> b({4,5,6}, &mr);

    std::swap(a, b);
        // UNDEFINED BEHAVIOR

    a.reserve(a.capacity() + 1);
        // this line will undoubtedly crash, as
        // it tries to delete[] a stack pointer
//dex28
//ex29
    auto temp = std::move(a);  // OK
    a = std::move(b);          // OK
    b = std::move(temp);       // OK
//dex29
    a.reserve(a.capacity() + 1);
#endif
}
} // namespace ex28

namespace ex30 {
//ex30
template<class T>
struct my_allocator : std::allocator<T>
{
    my_allocator() = default;

    template<class U>
    my_allocator(const my_allocator<U>&) {}

    template<class... Args>
    void construct(T *p, Args&&... args) {
        if (sizeof...(Args) == 0) {
            ::new ((void*)p) T;
        } else {
            ::new ((void*)p) T(std::forward<Args>(args)...);
        }
    }
};
//dex30
void test()
{
    std::vector<int, my_allocator<int>> v;
    v.push_back(100);
    v.resize(10);
    std::list<int, my_allocator<int>> lst;
    lst.push_back(100);
    lst.resize(10);
}
} // namespace ex30

namespace ex32 {
namespace std::pmr {
class memory_resource {
    virtual void *do_allocate(size_t bytes, size_t align) = 0;
    virtual void do_deallocate(void *p, size_t bytes, size_t align) = 0;
    virtual bool do_is_equal(const memory_resource& rhs) const = 0;
public:
    void *allocate(size_t bytes, size_t align) {
        return do_allocate(bytes, align);
    }
    void deallocate(void *p, size_t bytes, size_t align) {
        return do_deallocate(p, bytes, align);
    }
    bool is_equal(const memory_resource& rhs) const {
        return do_is_equal(rhs);
    }
};
class simple_buffer_resource : public memory_resource {
public:
    simple_buffer_resource(void *p, size_t cap) {
        m_buffer = p;
        m_capacity = cap;
        m_size = 0;
    }
    void release() {
        m_size = 0;
    }
private:
    void *m_buffer;
    size_t m_capacity;
    size_t m_size;
    void *do_allocate(size_t bytes, size_t align) override {
        if ((-m_size % align) > (m_capacity - m_size) ||
            bytes > (m_capacity - m_size - (-m_size % align)))
        {
            throw ::std::bad_alloc();
        }
        m_size += (-m_size % align) + bytes;
        return (char*)m_buffer + (m_size - bytes);
    }
    void do_deallocate(void *, size_t, size_t) override {}
    bool do_is_equal(const memory_resource& rhs) const noexcept override {
        return (this == &rhs);
    }
};
using monotonic_buffer_resource = simple_buffer_resource;
} // namespace std::pmr
namespace std {
    using ::std::scoped_allocator_adaptor;
    using ::std::vector;
    using ::std::max_align_t;
}
template<class T>
struct WidgetAlloc {
    std::pmr::memory_resource *mr;

    using value_type = T;

    WidgetAlloc(std::pmr::memory_resource *mr) : mr(mr) {}

    template<class U>
    WidgetAlloc(const WidgetAlloc<U>& rhs) : mr(rhs.mr) {}

    T *allocate(size_t n) {
        return (T *)mr->allocate(n * sizeof(T), alignof(T));
    }
    void deallocate(void *p, size_t n) {
        mr->deallocate(p, n * sizeof(T), alignof(T));
    }
    bool operator==(const WidgetAlloc& rhs) const { return mr == rhs.mr; }
    bool operator!=(const WidgetAlloc& rhs) const { return mr != rhs.mr; }
};

void test() {{
//ex31
    std::vector<std::vector<int>> vv;
    vv.emplace_back();
    vv.emplace_back();
    vv[0].push_back(1);
    vv[1].push_back(2);
    vv[1].push_back(3);
//dex31
}{
//ex32
    char buffer[10000];
    std::pmr::monotonic_buffer_resource mr {buffer, sizeof buffer};

    using InnerAlloc = WidgetAlloc<int>;
    using InnerVector = std::vector<int, InnerAlloc>;
    using OuterAlloc = WidgetAlloc<InnerVector>;

    std::vector<InnerVector, OuterAlloc> vv(&mr);
    vv.emplace_back(&mr);
    vv.emplace_back(&mr);
    vv[0].push_back(1);
    vv[1].push_back(2);
    vv[1].push_back(3);
//dex32
}{
//ex33
    char buffer[10000];
    std::pmr::monotonic_buffer_resource mr {buffer, sizeof buffer};

    using InnerAlloc = WidgetAlloc<int>;
    using InnerVector = std::vector<int, InnerAlloc>;
    using OuterAlloc = std::scoped_allocator_adaptor<WidgetAlloc<InnerVector>>;

    std::vector<InnerVector, OuterAlloc> vv(&mr);
    vv.emplace_back();
    vv.emplace_back();
    vv[0].push_back(1);
    vv[1].push_back(2);
    vv[1].push_back(3);
//dex33
}}

void test2() {
//ex34
    using InnerAlloc = WidgetAlloc<int>;
    using InnerVector = std::vector<int, InnerAlloc>;

    using MiddleAlloc = std::scoped_allocator_adaptor<
        WidgetAlloc<InnerVector>,
        WidgetAlloc<int>
    >;
    using MiddleVector = std::vector<InnerVector, MiddleAlloc>;

    using OuterAlloc = std::scoped_allocator_adaptor<
        WidgetAlloc<MiddleVector>,
        WidgetAlloc<InnerVector>,
        WidgetAlloc<int>
    >;
    using OuterVector = std::vector<MiddleVector, OuterAlloc>;
//dex34
//ex35
    char bi[1000];
    std::pmr::monotonic_buffer_resource mri {bi, sizeof bi};
    char bm[1000];
    std::pmr::monotonic_buffer_resource mrm {bm, sizeof bm};
    char bo[1000];
    std::pmr::monotonic_buffer_resource mro {bo, sizeof bo};

    OuterAlloc saa(&mro, &mrm, &mri);
//dex35
//ex36
    OuterVector vvv(saa);

    vvv.emplace_back();
        // This allocation comes from buffer "bo".

    vvv[0].emplace_back();
        // This allocation comes from buffer "bm".

    vvv[0][0].emplace_back(42);
        // This allocation comes from buffer "bi".
//dex36
    assert(*(int*)bi == 42);
    {
//ex37
    using InnerAlloc = WidgetAlloc<int>;
    using InnerVector = std::vector<int, InnerAlloc>;

    using MiddleAlloc = std::scoped_allocator_adaptor<
        WidgetAlloc<InnerVector>
    >;
    using MiddleVector = std::vector<InnerVector, MiddleAlloc>;

    using TooShortAlloc = std::scoped_allocator_adaptor<
        WidgetAlloc<MiddleVector>,
        WidgetAlloc<InnerVector>
    >;
    using OuterVector = std::vector<MiddleVector, TooShortAlloc>;

    TooShortAlloc tsa(&mro, WidgetAlloc<InnerVector>(&mri));
    OuterVector tsv(tsa);

    tsv.emplace_back();
        // This allocation comes from buffer "bo".

    tsv[0].emplace_back();
        // This allocation comes from buffer "bi".

    tsv[0][0].emplace_back(42);
        // This allocation AGAIN comes from buffer "bi"!
//dex37
    }
}
} // namespace ex32

int main()
{
    ex5::test();
    ex5::test2();
    ex10::test();
    ex11::test();
    ex12::test();
    ex13::test();
    ex14::test();
    ex15::test();
    ex21::test();
    ex24::test();
    ex25::test();
    ex28::test();
    ex30::test();
    ex32::test();
    ex32::test2();
}
