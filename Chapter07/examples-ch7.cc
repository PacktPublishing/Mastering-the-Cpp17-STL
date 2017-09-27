#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <sstream>
#include <thread>
#include <utility>
#include <vector>
#define BOOST_ASIO_DISABLE_HANDLER_TYPE_REQUIREMENTS
#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace ex1 {
volatile int& memory_mapped_register_x() { static int x = 0; return x; }
volatile bool& memory_mapped_register_y() { static bool y = 0; return y; }
void test() {
//ex1
    volatile int& x = memory_mapped_register_x();
    volatile bool& y = memory_mapped_register_y();
    int stack;

    stack = x;      // load
    y = true;       // store
    stack += x;     // load
//dex1
//ex2
    stack = 2*x;    // load
    y = true;       // store
//dex2
}
} // namespace ex1

namespace ex3 {
//ex3
// Global variables:
int64_t x = 0;
bool y = false;

void thread_A() {
    x = 0x42'00000042;
    y = true;
}

void thread_B() {
    if (x) {
        assert(x == 0x42'00000042);
    }
}

void thread_C() {
    if (y) {
        assert(x == 0x42'00000042);
    }
}
//dex3
} // namespace ex3

namespace ex4 {
//ex4
// Global variables:
std::atomic<int64_t> x = 0;
std::atomic<bool> y = false;

void thread_A() {
    x = 0x42'00000042;  // atomic!
    y = true;           // atomic!
}

void thread_B() {
    if (x) {
        // The assignment to x happens atomically.
        assert(x == 0x42'00000042);
    }
}

void thread_C() {
    if (y) {
        // The assignment to x "happens before" the
        // assignment to y, even from another thread's
        // point of view.
        assert(x == 0x42'00000042);
    }
}
//dex4
} // namespace ex4

namespace ex5 {
void test() {
#if 0
//ex5
    std::atomic<int> a, b;

    a = b;  // DOES NOT COMPILE!
//dex5
#endif
    std::atomic<int> a = 0;
    const std::atomic<int> b = 0;
//ex6
    int shortlived = b;  // atomic load
    a = shortlived;  // atomic store
//dex6
}
} // namespace ex5

namespace ex6 {
void test() {
    std::atomic<int> a = 0;
    const std::atomic<int> b = 0;
//ex7
    int shortlived = b.load();  // atomic load
    a.store(shortlived);  // atomic store
//dex7
}
} // namespace ex6

namespace ex8 {
void operator*=(std::atomic<int>&, int) {}
void test() {
//ex8
    std::atomic<int> a = 6;

    a *= 9;  // This isn't allowed.

    // But this is:

    int expected, desired;
    do {
        expected = a.load();
        desired = expected * 9;
    } while (!a.compare_exchange_weak(expected, desired));

    // At the end of this loop, a's value will
    // have been "atomically" multiplied by 9.
//dex8
    assert(a == 54);
}
} // namespace ex8

namespace ex9 {
void test() {
    std::atomic<int> a = 6;
//ex9
    int expected = a.load();
    while (!a.compare_exchange_weak(expected, expected * 9)) {
        // continue looping
    }
//dex9
    assert(a == 54);
}
} // namespace ex9

namespace ex10 {
//ex10
void log(const char *message)
{
    static std::mutex m;
    m.lock();  // avoid interleaving messages on stdout
    puts(message);
    m.unlock();
}
//dex10
} // namespace ex10

namespace ex11 {
//ex11
static std::mutex m;

void log1(const char *message) {
    m.lock();
    printf("LOG1: %s\n", message);
    m.unlock();
}

void log2(const char *message) {
    m.lock();
    printf("LOG2: %s\n", message);
    m.unlock();
}
//dex11
} // namespace ex11

namespace ex12 {
//ex12
struct Logger {
    std::mutex m_mtx;

    void log1(const char *message) {
        m_mtx.lock();
        printf("LOG1: %s\n", message);
        m_mtx.unlock();
    }

    void log2(const char *message) {
        m_mtx.lock();
        printf("LOG2: %s\n", message);
        m_mtx.unlock();
    }
};
//dex12
} // namespace ex12

namespace ex13 {
//ex13
template<typename M>
class unique_lock {
    M *m_mtx = nullptr;
    bool m_locked = false;
public:
    constexpr unique_lock() noexcept = default;
    constexpr unique_lock(M *p) noexcept : m_mtx(p) {}

    M *mutex() const noexcept { return m_mtx; }
    bool owns_lock() const noexcept { return m_locked; }

    void lock() { m_mtx->lock(); m_locked = true; }
    void unlock() { m_mtx->unlock(); m_locked = false; }

    unique_lock(unique_lock&& rhs) noexcept {
        m_mtx = std::exchange(rhs.m_mtx, nullptr);
        m_locked = std::exchange(rhs.m_locked, false);
    }

    unique_lock& operator=(unique_lock&& rhs) {
        if (m_locked) {
            unlock();
        }
        m_mtx = std::exchange(rhs.m_mtx, nullptr);
        m_locked = std::exchange(rhs.m_locked, false);
        return *this;
    }

    ~unique_lock() {
        if (m_locked) {
            unlock();
        }
    }
};
//dex13
void test() {
    std::mutex m;
    std::unique_lock<std::mutex> lk(m);
    assert(lk.owns_lock());
    auto lk2 = std::move(lk);
    assert(!lk.owns_lock());
    assert(lk2.owns_lock());
    lk2.unlock();
    assert(!lk2.owns_lock());
}
} // namespace ex13

namespace ex14 {
//ex14
struct Lockbox {
    std::mutex m_mtx;
    int m_value = 0;

    void locked_increment() {
        std::lock_guard<std::mutex> lk(m_mtx);
        m_value += 1;
    }

    void locked_decrement() {
        std::lock_guard lk(m_mtx);  // C++17 only
        m_value -= 1;
    }
};
//dex14
void test() {
    Lockbox l;
    l.locked_increment();
    l.locked_decrement();
    assert(l.m_value == 0);
}
} // namespace ex14

namespace ex15 {
//ex15
class StreamingAverage {
    double m_sum = 0;
    int m_count = 0;
    double m_last_average = 0;
    std::mutex m_mtx;
public:
    // Called from the single producer thread
    void add_value(double x) {
        std::lock_guard lk(m_mtx);
        m_sum += x;
        m_count += 1;  // A
    }

    // Called from the single consumer thread
    double get_current_average() {
        std::lock_guard lk(m_mtx);
        m_last_average = m_sum / m_count;  // B
        return m_last_average;
    }

    // Called from the single consumer thread
    double get_last_average() const {
        return m_last_average;  // C
    }

    // Called from the single consumer thread
    double get_current_count() const {
        return m_count;  // D
    }
};
//dex15
} // namespace ex15

namespace ex16 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wunused-private-field"
//ex16
class StreamingAverage {
    double m_sum = 0;
    int m_count = 0;
    double m_last_average = 0;
    std::mutex m_sum_count_mtx;

    // ...
};
//dex16
#pragma GCC diagnostic pop
} // namespace ex16

namespace ex17 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wunused-private-field"
//ex17
class StreamingAverage {
    struct {
        double sum = 0;
        int count = 0;
        std::mutex mtx;
    } m_guarded_sc;
    double m_last_average = 0;

    // ...
};
//dex17
#pragma GCC diagnostic pop
} // namespace ex17

namespace ex18 {
//ex18
template<class Data>
class Guarded {
    std::mutex m_mtx;
    Data m_data;

    class Handle {
        std::unique_lock<std::mutex> m_lk;
        Data *m_ptr;
    public:
        Handle(std::unique_lock<std::mutex> lk, Data *p) :
            m_lk(std::move(lk)), m_ptr(p) {}
        auto operator->() const { return m_ptr; }
    };
public:
    Handle lock() {
        std::unique_lock lk(m_mtx);
        return Handle{std::move(lk), &m_data};
    }
};
//dex18
//ex19
class StreamingAverage {
    struct Guts {
        double m_sum = 0;
        int m_count = 0;
    };
    Guarded<Guts> m_sc;
    double m_last_average = 0;

    // ...

    double get_current_average() {
        auto h = m_sc.lock();
        m_last_average = h->m_sum / h->m_count;
        return m_last_average;
    }
};
//dex19
} // namespace ex18

namespace ex20 {
class StreamingAverage {
    struct Guts {
        double m_sum = 0;
        int m_count = 0;
    };
    ::ex18::Guarded<Guts> m_sc;

//ex20
    double get_sum() {
        return m_sc.lock()->m_sum;
    }

    int get_count() {
        return m_sc.lock()->m_count;
    }

    double get_current_average() {
        return get_sum() / get_count();
    }
//dex20
};
} // namespace ex20

namespace ex21 {
class StreamingAverage {
    struct Guts {
        double m_sum = 0;
        int m_count = 0;
    };
    ::ex18::Guarded<Guts> m_sc;

//ex21
    double get_sum() {
        return m_sc.lock()->m_sum;  // LOCK 2
    }

    int get_count() {
        return m_sc.lock()->m_count;
    }

    double get_current_average() {
        auto h = m_sc.lock();  // LOCK 1
        return get_sum() / get_count();
    }
//dex21
};
} // namespace ex21

namespace ex22 {
void test() {
    using namespace std::literals;
//ex22
    std::timed_mutex m;
    std::atomic<bool> ready = false;

    std::thread thread_b([&]() {
        std::lock_guard lk(m);
        puts("Thread B got the lock.");
        ready = true;
        std::this_thread::sleep_for(100ms);
    });

    while (!ready) {
        puts("Thread A is waiting for thread B to launch.");
        std::this_thread::sleep_for(10ms);
    }

    while (!m.try_lock_for(10ms)) {
        puts("Thread A spent 10ms trying to get the lock and failed.");
    }

    puts("Thread A finally got the lock!");
    m.unlock();
//dex22
    thread_b.join();
}
} // namespace ex22

namespace ex23 {
void test() {
    using namespace std::literals;

//ex24
    auto count_ms = [](auto&& d) -> int {
        using namespace std::chrono;
        return duration_cast<milliseconds>(d).count();
    };
//dex24

//ex23
    std::timed_mutex m1, m2;
    std::atomic<bool> ready = false;

    std::thread thread_b([&]() {
        std::unique_lock lk1(m1);
        std::unique_lock lk2(m2);
        puts("Thread B got the locks.");
        ready = true;
        std::this_thread::sleep_for(50ms);
        lk1.unlock();
        std::this_thread::sleep_for(50ms);
    });

    while (!ready) {
        std::this_thread::sleep_for(10ms);
    }

    auto start_time = std::chrono::system_clock::now();
    auto deadline = start_time + 100ms;

    bool got_m1 = m1.try_lock_until(deadline);
    auto elapsed_m1 = std::chrono::system_clock::now() - start_time;

    bool got_m2 = m2.try_lock_until(deadline);
    auto elapsed_m2 = std::chrono::system_clock::now() - start_time;

    if (got_m1) {
        printf("Thread A got the first lock after %dms.\n",
            count_ms(elapsed_m1));
        m1.unlock();
    }
    if (got_m2) {
        printf("Thread A got the second lock after %dms.\n",
            count_ms(elapsed_m2));
        m2.unlock();
    }
//dex23
    thread_b.join();
}
} // namespace ex23

namespace ex25 {
//ex25
template<class M>
std::unique_lock<M> upgrade(std::shared_lock<M> lk)
{
    lk.unlock();
    // Some other writer might sneak in here.
    return std::unique_lock<M>(*lk.mutex());
}
//dex25
void test() {
    std::shared_mutex m;
    std::shared_lock slk(m);
    auto ulk = upgrade(std::move(slk));
    assert(ulk.owns_lock());
}
} // namespace ex25

namespace ex26 {
//ex26
template<class M>
std::shared_lock<M> downgrade(std::unique_lock<M> lk)
{
    lk.unlock();
    // Some other writer might sneak in here.
    return std::shared_lock<M>(*lk.mutex());
}
//dex26
void test() {
    std::shared_mutex m;
    std::unique_lock ulk(m);
    auto slk = downgrade(std::move(ulk));
    assert(slk.owns_lock());
}
} // namespace ex26

namespace ex27 {
using namespace std::literals;
bool prepped = false;
void prep_work() { prepped = true; }
void main_work() {}
void test() {
//ex27
    std::atomic<bool> ready = false;

    std::thread thread_b([&]() {
        prep_work();
        ready = true;
        main_work();
    });

    // Wait for thread B to be ready.
    while (!ready) {
        std::this_thread::sleep_for(10ms);
    }
    // Now thread B has completed its prep work.
//dex27
    assert(prepped);
    thread_b.join();
}
} // namespace ex27

namespace ex28 {
bool prepped = false;
void prep_work() { prepped = true; }
void main_work() {}
void test() {
//ex28
    bool ready = false;  // not atomic!
    std::mutex ready_mutex;
    std::condition_variable cv;

    std::thread thread_b([&]() {
        prep_work();
        {
            std::lock_guard lk(ready_mutex);
            ready = true;
        }
        cv.notify_one();
        main_work();
    });

    // Wait for thread B to be ready.
    {
        std::unique_lock lk(ready_mutex);
        while (!ready) {
            cv.wait(lk);
        }
    }
    // Now thread B has completed its prep work.
//dex28
    assert(prepped);
    thread_b.join();
}
} // namespace ex28

namespace ex29 {
bool prepped = false;
void prep_work() { prepped = true; }
void main_work() {}
void test() {
//ex29
    bool ready = false;
    std::shared_mutex ready_rwlock;
    std::condition_variable_any cv;

    std::thread thread_b([&]() {
        prep_work();
        {
            std::lock_guard lk(ready_rwlock);
            ready = true;
        }
        cv.notify_one();
        main_work();
    });

    // Wait for thread B to be ready.
    {
        std::shared_lock lk(ready_rwlock);
        while (!ready) {
            cv.wait(lk);
        }
    }
    // Now thread B has completed its prep work.
//dex29
    assert(prepped);
    thread_b.join();
}
} // namespace ex29

namespace ex30 {
void test() {
    using namespace std::literals;
    auto count_ms = [](auto&& d) -> int {
        using namespace std::chrono;
        return duration_cast<milliseconds>(d).count();
    };
//ex30
    std::promise<int> p1, p2;
    std::future<int> f1 = p1.get_future();
    std::future<int> f2 = p2.get_future();

    // If the promise is satisfied first,
    // then f.get() will not block.
    p1.set_value(42);
    assert(f1.get() == 42);

    // If f.get() is called first, then it
    // will block until set_value() is called
    // from some other thread.
    std::thread t([&](){
        std::this_thread::sleep_for(100ms);
        p2.set_value(43);
    });
    auto start_time = std::chrono::system_clock::now();
    assert(f2.get() == 43);
    auto elapsed = std::chrono::system_clock::now() - start_time;
    printf("f2.get() took %dms.\n", count_ms(elapsed));
    t.join();
//dex30
}
} // namespace ex30

namespace ex31 {
bool prepped = false;
void prep_work() { prepped = true; }
void main_work() {}
void test() {
//ex31
    std::promise<void> ready_p;
    std::future<void> ready_f = ready_p.get_future();

    std::thread thread_b([&]() {
        prep_work();
        ready_p.set_value();
        main_work();
    });

    // Wait for thread B to be ready.
    ready_f.wait();
    // Now thread B has completed its prep work.
//dex31
    assert(prepped);
    thread_b.join();
}
} // namespace ex31

namespace ex32 {
template<class T = void>
struct MyAllocator {
    using value_type = T;
    MyAllocator() = default;
    template<class U> MyAllocator(const MyAllocator<U>&) {}
    T *allocate(size_t n) { return reinterpret_cast<T*>(new char[sizeof(T) * n]); }
    void deallocate(T *p, size_t) { delete [] reinterpret_cast<char*>(p); }
    template<class U> struct rebind { using other = MyAllocator<U>; };
};
MyAllocator() -> MyAllocator<void>;
void test() {
//ex32
    MyAllocator myalloc{};
    std::promise<int> p(std::allocator_arg, myalloc);
    std::future<int> f = p.get_future();
//dex32
    (void)p;
    (void)f;
}
} // namespace ex32

namespace ex33 {
//ex33
template<class T>
class simple_packaged_task {
    std::function<T()> m_func;
    std::promise<T> m_promise;
public:
    template<class F>
    simple_packaged_task(const F& f) : m_func(f) {}

    auto get_future() { return m_promise.get_future(); }

    void operator()() {
        try {
            T result = m_func();
            m_promise.set_value(result);
        } catch (...) {
            m_promise.set_exception(std::current_exception());
        }
    }
};
//dex33
} // namespace ex33

namespace ex34 {
using Data = int;
using Connection = double;
Connection slowly_open_connection() { return 0; }
Data slowly_get_data_from_disk() { return 1; }
Data slowly_get_data_from_connection(Connection) { return 1; }
#define bool t1.join(); t2.join(); t3.join(); bool
//ex34
template<class T>
auto pf() {
    std::promise<T> p;
    std::future<T> f = p.get_future();
    return std::make_pair(std::move(p), std::move(f));
}

void test() {
    auto [p1, f1] = pf<Connection>();
    auto [p2, f2] = pf<Data>();
    auto [p3, f3] = pf<Data>();

    auto t1 = std::thread([p1 = std::move(p1)]() mutable {
        Connection conn = slowly_open_connection();
        p1.set_value(conn);
        // DANGER: what if slowly_open_connection throws?
    });
    auto t2 = std::thread([p2 = std::move(p2)]() mutable {
        Data data = slowly_get_data_from_disk();
        p2.set_value(data);
    });
    auto t3 = std::thread(
    [p3 = std::move(p3), f1 = std::move(f1)]() mutable {
        Data data = slowly_get_data_from_connection(f1.get());
        p3.set_value(data);
    });
    bool success = (f2.get() == f3.get());

    assert(success);
}
//dex34
#undef bool
} // namespace ex34

namespace ex35 {
using Data = int;
using Connection = double;
Connection slowly_open_connection() { return 0; }
Data slowly_get_data_from_disk() { return 1; }
Data slowly_get_data_from_connection(Connection) { return 1; }
//ex35
void test() {
    auto f1 = std::async(slowly_open_connection);
    auto f2 = std::async(slowly_get_data_from_disk);
    auto f3 = std::async([f1 = std::move(f1)]() mutable {
        return slowly_get_data_from_connection(f1.get());
        // No more danger.
    });
    bool success = (f2.get() == f3.get());

    assert(success);
}
//dex35
} // namespace ex35

namespace ex36 {
using Data = int;
using Connection = double;
Connection slowly_open_connection() { return 0; }
Data slowly_get_data_from_disk() { return 1; }
Data slowly_get_data_from_connection(Connection) { return 1; }
#if 0  // too hard to get Folly in here to test with
//ex36
void test() {
    auto f1 = my::async(slowly_open_connection);
    auto f2 = my::async(slowly_get_data_from_disk);
    auto f3 = f1.then([](Connection conn) {
        return slowly_get_data_from_connection(conn);
    });
    bool success = f2.get() == f3.get();

    assert(success);
}
//dex36
#else
void test() {}
#endif
} // namespace ex36

namespace ex37 {
void test() {
//ex37
    using namespace std::literals; // for "ms"

    std::thread a([](){
        puts("Thread A says hello ~0ms");
        std::this_thread::sleep_for(10ms);
        puts("Thread A says goodbye ~10ms");
    });

    std::thread b([](){
        puts("Thread B says hello ~0ms");
        std::this_thread::sleep_for(20ms);
        puts("Thread B says goodbye ~20ms");
    });

    puts("The main thread says hello ~0ms");
    a.join();  // waits for thread A
    b.detach();  // doesn't wait for thread B
    puts("The main thread says goodbye ~10ms");
//dex37
}
} // namespace ex37

namespace ex38 {
//ex38
template<class F>
auto async(F&& func) {
    using ResultType = std::invoke_result_t<std::decay_t<F>>;
    using PromiseType = std::promise<ResultType>;
    using FutureType = std::future<ResultType>;

    PromiseType promise;
    FutureType future = promise.get_future();
    auto t = std::thread([
        func = std::forward<F>(func),
        promise = std::move(promise)
    ]() mutable {
        try {
            ResultType result = func();
            promise.set_value(result);
        } catch (...) {
            promise.set_exception(std::current_exception());
        }
    });
    // This special behavior is not implementable
    // outside of the library, but async does do it.
    // future.on_destruction([t = std::move(t)]() {
    //     t.join();
    // });
    return future;
}
//dex38
void test() {
    if (false) {
        auto p = std::make_unique<int>(42);
        async([p = std::move(p)](){ return *p; });
    }
}
} // namespace ex38

namespace ex39 {
//ex39
template<class F>
void fire_and_forget_wrong(const F& f) {
    // WRONG! Runs f in another thread, but blocks anyway.
    std::async(f);
}

template<class F>
void fire_and_forget_better(const F& f) {
    // BETTER! Launches f in another thread without blocking.
    std::thread(f).detach();
}
//dex39
} // namespace ex39

namespace ex40 {
//ex40
int test() {
    int i = 0;
    auto future = std::async([&]() {
        i += 1;
    });
    // suppose we do not call f.wait() here
    return i;
}
//dex40
} // namespace ex40

namespace ex41 {
//ex41
std::string to_string(std::thread::id id)
{
    std::ostringstream o;
    o << id;
    return o.str();
}
//dex41

void test() {
    using namespace std::literals;
//ex42
    std::mutex ready;
    std::unique_lock lk(ready);
    std::vector<std::thread> threads;

    auto task = [&](){
        // Block here until the main thread is ready.
        (void)std::lock_guard(ready);
        // Now go. Find my thread-id in the vector.
        auto my_id = std::this_thread::get_id();
        auto iter = std::find_if(
            threads.begin(), threads.end(),
            [=](const std::thread& t) {
                return t.get_id() == my_id;
            }
        );
        printf("Thread %s %s in the list.\n",
            to_string(my_id).c_str(),
            iter != threads.end() ? "is" : "is not");
    };

    std::vector<std::thread> others;
    for (int i = 0; i < 10; ++i) {
        std::thread t(task);
        if (i % 2) {
            threads.push_back(std::move(t));
        } else {
            others.push_back(std::move(t));
        }
    }

        // Let all the threads run.
    ready.unlock();

        // Join all the threads.
    for (std::thread& t : threads) t.join();
    for (std::thread& t : others) t.join();
//dex42
}
} // namespace ex41

namespace ex43 {
//ex43
class ThreadPool {
    using UniqueFunction = std::packaged_task<void()>;
    struct {
        std::mutex mtx;
        std::queue<UniqueFunction> work_queue;
        bool aborting = false;
    } m_state;
    std::vector<std::thread> m_workers;
    std::condition_variable m_cv;
//dex43
//ex44
public:
    ThreadPool(int size) {
        for (int i=0; i < size; ++i) {
            m_workers.emplace_back([this]() { worker_loop(); });
        }
    }
//dex44
//ex45
    ~ThreadPool() {
        if (std::lock_guard lk(m_state.mtx); true) {
            m_state.aborting = true;
        }
        m_cv.notify_all();
        for (std::thread& t : m_workers) {
            t.join();
        }
    }
//dex45
//ex46
    void enqueue_task(UniqueFunction task) {
        if (std::lock_guard lk(m_state.mtx); true) {
            m_state.work_queue.push(std::move(task));
        }
        m_cv.notify_one();
    }
//dex46
//ex47
private:
    void worker_loop() {
        while (true) {
            std::unique_lock lk(m_state.mtx);

            while (m_state.work_queue.empty() && !m_state.aborting) {
                m_cv.wait(lk);
            }
            if (m_state.aborting) break;

            // Pop the next task, while still under the lock.
            assert(!m_state.work_queue.empty());
            UniqueFunction task = std::move(m_state.work_queue.front());
            m_state.work_queue.pop();

            lk.unlock();
            // Actually run the task. This might take a while.
            task();
            // When we're done with this task, go get another.
        }
    }
//dex47
//ex48
public:
    template<class F>
    auto async(F&& func) {
        using ResultType = std::invoke_result_t<std::decay_t<F>>;

        std::packaged_task<ResultType()> pt(std::forward<F>(func));
        std::future<ResultType> future = pt.get_future();

        UniqueFunction task(
            [pt = std::move(pt)]() mutable { pt(); }
        );

        enqueue_task(std::move(task));

        // Give the user a future for retrieving the result.
        return future;
    }
}; // class ThreadPool
//dex48

//ex49
void test() {
    std::atomic<int> sum(0);
    ThreadPool tp(4);
    std::vector<std::future<int>> futures;
    for (int i=0; i < 60000; ++i) {
        auto f = tp.async([i, &sum](){
            sum += i;
            return i;
        });
        futures.push_back(std::move(f));
    }
    assert(futures[42].get() == 42);
    assert(903 <= sum && sum <= 1799970000);
}
//dex49

void test2() {
    using namespace std::literals;

    std::future<int> f6;
    if (ThreadPool tp(4); true) {
        auto f1 = tp.async([](){ std::this_thread::sleep_for(10ms); return 1; });
        auto f2 = tp.async([](){ std::this_thread::sleep_for(20ms); return 2; });
        auto f3 = tp.async([](){ std::this_thread::sleep_for(30ms); return 3; });
        auto f4 = tp.async([](){ std::this_thread::sleep_for(40ms); return 4; });
        auto f5 = tp.async([](){ std::this_thread::sleep_for(50ms); return 5; });
        tp.async([]{ std::this_thread::sleep_for(100ms); });
        tp.async([]{ std::this_thread::sleep_for(100ms); });
        tp.async([]{ std::this_thread::sleep_for(100ms); });
        tp.async([]{ std::this_thread::sleep_for(100ms); });
        f6 = tp.async([](){ std::this_thread::sleep_for(60ms); return 6; });
        assert(f4.get() == 4);
    }
    puts("Done!");
    try {
        f6.get();
        assert(false);
    } catch (const std::future_error& ex) {
        assert(ex.code() == std::future_errc::broken_promise);
    }
}
} // namespace ex43

namespace ex50 {
//ex50
class ThreadPool {
    boost::thread_group m_workers;
    boost::asio::io_service m_io;
    boost::asio::io_service::work m_work;
public:
    ThreadPool(int size) : m_work(m_io) {
        for (int i=0; i < size; ++i) {
            m_workers.create_thread([&](){ m_io.run(); });
        }
    }

    template<class F>
    void enqueue_task(F&& func) {
        m_io.post(std::forward<F>(func));
    }

    ~ThreadPool() {
        m_io.stop();
        m_workers.join_all();
    }
};
//dex50

template<class F>
auto tp_async(ThreadPool& tp, F&& func) {
    using ResultType = std::invoke_result_t<std::decay_t<F>>;

    std::packaged_task<ResultType()> pt(std::forward<F>(func));
    std::future<ResultType> future = pt.get_future();

    tp.enqueue_task([pt = std::move(pt)]() mutable { pt(); });

    // Give the user a future for retrieving the result.
    return future;
}

void test() {
    std::atomic<int> sum(0);
    ThreadPool tp(4);
    std::vector<std::future<int>> futures;
    for (int i=0; i < 60000; ++i) {
        auto f = tp_async(tp, [i, &sum](){
            sum += i;
            return i;
        });
        futures.push_back(std::move(f));
    }
    assert(futures[42].get() == 42);
    assert(903 <= sum && sum <= 1799970000);
}
void test2() {
    using namespace std::literals;

    std::future<int> f6;
    if (ThreadPool tp(4); true) {
        auto f1 = tp_async(tp, [](){ std::this_thread::sleep_for(10ms); return 1; });
        auto f2 = tp_async(tp, [](){ std::this_thread::sleep_for(20ms); return 2; });
        auto f3 = tp_async(tp, [](){ std::this_thread::sleep_for(30ms); return 3; });
        auto f4 = tp_async(tp, [](){ std::this_thread::sleep_for(40ms); return 4; });
        auto f5 = tp_async(tp, [](){ std::this_thread::sleep_for(50ms); return 5; });
        tp_async(tp, []{ std::this_thread::sleep_for(100ms); });
        tp_async(tp, []{ std::this_thread::sleep_for(100ms); });
        tp_async(tp, []{ std::this_thread::sleep_for(100ms); });
        tp_async(tp, []{ std::this_thread::sleep_for(100ms); });
        f6 = tp_async(tp, [](){ std::this_thread::sleep_for(60ms); return 6; });
        assert(f4.get() == 4);
    }
    puts("Done!");
    try {
        f6.get();
        assert(false);
    } catch (const std::future_error& ex) {
        assert(ex.code() == std::future_errc::broken_promise);
    }
}
} // namespace ex50

int main()
{
    using namespace std::literals;
    ex1::test();
    ex5::test();
    ex6::test();
    ex8::test();
    ex9::test();
    ex13::test();
    ex14::test();
    ex22::test();
    ex23::test();
    ex25::test();
    ex26::test();
    ex27::test();
    ex28::test();
    ex29::test();
    ex30::test();
    ex31::test();
    ex32::test();
    ex34::test();
    ex35::test();
    ex36::test();
    ex37::test();
    std::this_thread::sleep_for(20ms);
    ex38::test();
    ex40::test();
    ex41::test();
    ex43::test();
    ex43::test2();
    ex50::test();
    ex50::test2();
}
