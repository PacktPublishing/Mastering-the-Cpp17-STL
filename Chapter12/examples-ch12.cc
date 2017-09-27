#include <cassert>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <future>
#include <iostream>
#include <string>

#define USE_BOOST 1
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#define u8string string

#if 0
 #include <charconv>
#else
 #include <system_error>
namespace std {
    struct from_chars_result {
         const char* ptr;
         std::errc ec;
    };
    struct to_chars_result {
         char* ptr;
         std::errc ec;
    };
    enum class chars_format {
        scientific = 1,
        fixed = 2,
        hex = 4,
        general = fixed | scientific
    };
    from_chars_result from_chars(const char*, const char*, int&, int = 10) { return {}; }
    from_chars_result from_chars(const char* first, const char* last, float& value, chars_format fmt = chars_format::general);
    to_chars_result to_chars(char* first, char* last, int value, int base = 10);
    to_chars_result to_chars(char* first, char* last, float value);
    to_chars_result to_chars(char* first, char* last, float value, chars_format fmt);
    to_chars_result to_chars(char* first, char* last, float value, chars_format fmt, int precision);
} // namespace std
#endif

//ex3
#if USE_CXX17
 #include <filesystem>
 namespace fs = std::filesystem;
#elif USE_FILESYSTEM_TS
 #include <experimental/filesystem>
 namespace fs = std::experimental::filesystem;
#elif USE_BOOST
 #include <boost/filesystem.hpp>
 namespace fs = boost::filesystem;
#endif
//dex3

namespace std::filesystem { using fs::path; using fs::remove; };

namespace ex1 {
//ex1
using namespace std::filesystem;

void foo(path p)
{
    remove(p);  // What function is this?
}
//dex1
} // namespace ex1

namespace ex2 {
//ex2
namespace fs = std::filesystem;

void foo(fs::path p)
{
    fs::remove(p);  // Much clearer!
}
//dex2
} // namespace ex2

namespace ex4 {
void test() {
    std::ifstream f;
    f.exceptions(std::ios_base::failbit);
//ex4
    try {
        f.open("hello.txt");
        // Opening succeeded.
    } catch (const std::ios_base::failure&) {
        // Opening failed.
    }
//dex4
    assert(!f.is_open());
    f.exceptions(std::ios_base::goodbit);
//ex5
    f.open("hello.txt");
    if (f.is_open()) {
        // Opening succeeded.
    } else {
        // Opening failed.
    }
//dex5
    assert(!f.is_open());
}
} // namespace ex4

namespace ex6 {
void test() {
    char s[] = "hello";
    using std::end;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
//ex6
    // Exception-throwing approach.
    try {
        int i = std::stoi(s);
        // Parsing succeeded.
    } catch (...) {
        // Parsing failed.
    }
//dex6
    {
//ex7
    char *endptr = nullptr;
    errno = 0;
    long i = strtol(s, &endptr, 10);
    if (endptr != s && !errno) {
        // Parsing succeeded.
    } else {
        // Parsing failed.
    }
//dex7
    }{
//ex8
    int i = 0;
    auto [ptr, ec] = std::from_chars(s, end(s), i);
    if (ec != std::errc{}) {
        // Parsing succeeded.
    } else {
        // Parsing failed.
    }
//dex8
    }
#pragma GCC diagnostic pop
}
} // namespace ex4

namespace ex9 {
//ex9
uintmax_t file_size(const fs::path& p);

uintmax_t file_size(const fs::path& p,
                    std::error_code& ec) noexcept;
//dex9
} // namespace ex9

namespace ex10 {
namespace std { using ::std::string; }
//ex10
namespace std {
    enum class errc {
        // implicitly, "0" means "no error"
        operation_not_permitted = EPERM,
        no_such_file_or_directory = ENOENT,
        no_such_process = ESRCH,
        // ...
        value_too_large = EOVERFLOW
    };
} // namespace std
//dex10
//ex11
namespace std {

class error_category {
public:
    virtual const char *name() const noexcept = 0;
    virtual std::string message(int err) const = 0;

    // other virtual methods not shown

    bool operator==(const std::error_category& rhs) const {
        return this == &rhs;
    }
};

} // namespace std
//dex11
//ex12
namespace std {

class error_code {
    const std::error_category *m_cat;
    int m_err;
public:
    const auto& category() const { return m_cat; }
    int value() const { return m_err; }
    std::string message() const { return m_cat->message(m_err); }
    explicit operator bool() const { return m_err != 0; }

    // other convenience methods not shown
};

} // namespace std
//dex12
} // namespace ex10

namespace ex13 {
struct Unreachable {};
//ex13
namespace FinickyFS {

enum class Error : int {
    success = 0,
    forbidden_character = 1,
    forbidden_word = 2,
};

struct ErrorCategory : std::error_category
{
    const char *name() const noexcept override {
        return "finicky filesystem";
    }

    std::string message(int err) const override {
        switch (err) {
            case 0: return "Success";
            case 1: return "Invalid filename";
            case 2: return "Bad word in filename";
        }
        throw Unreachable();
    }

    static ErrorCategory& instance() {
        static ErrorCategory instance;
        return instance;
    }
};

std::error_code make_error_code(Error err) noexcept
{
    return std::error_code(int(err), ErrorCategory::instance());
}

} // namespace FinickyFS
//dex13
void test() {
//ex14
        // An error fits comfortably in a statically typed
        // and value-semantic std::error_code object...
    std::error_code ec =
        make_error_code(FinickyFS::Error::forbidden_word);

        // ...Yet its "what-string" remains just as
        // accessible as if it were a dynamically typed
        // exception!
    assert(ec.message() == "Bad word in filename");
//dex14
}
} // namespace ex13

namespace ex15 {
struct Unreachable {};
namespace FinickyFS {
enum class Error : int {
    success = 0,
    forbidden_character = 1,
    forbidden_word = 2,
};
struct ErrorCategory : std::error_category {
    const char *name() const noexcept override {
        return "finicky filesystem";
    }
    std::string message(int err) const override {
        switch (err) {
            case 0: return "Success";
            case 1: return "Invalid filename";
            case 2: return "Bad word in filename";
        }
        throw Unreachable();
    }
    static ErrorCategory& instance() {
        static ErrorCategory instance;
        return instance;
    }
};
std::error_code make_error_code(Error err) noexcept {
    return std::error_code(int(err), ErrorCategory::instance());
}

bool is_malformed_name(std::error_code ec);

} // namespace FinickyFS

} // namespace ex15

#define FinickyFS ex15::FinickyFS
//ex15
namespace std {
template<>
struct is_error_code_enum<::FinickyFS::Error> : true_type {};
} // namespace std
//dex15

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wterminate"
//ex19
namespace FinickyFS {

enum class Condition : int {
    success = 0,
    malformed_name = 1,
};

struct ConditionCategory : std::error_category {
    const char *name() const noexcept override {
        return "finicky filesystem";
    }
    std::string message(int cond) const override {
        switch (cond) {
            case 0: return "Success";
            case 1: return "Malformed name";
        }
        throw Unreachable();
    }
    bool equivalent(const std::error_code& ec, int cond) const noexcept override {
        switch (cond) {
            case 0: return !ec;
            case 1: return is_malformed_name(ec);
        }
        throw Unreachable();
    }
    static ConditionCategory& instance() {
        static ConditionCategory instance;
        return instance;
    }
};
std::error_condition make_error_condition(Condition cond) noexcept {
    return std::error_condition(int(cond), ConditionCategory::instance());
}

} // namespace FinickyFS

namespace std {
template<>
struct is_error_condition_enum<::FinickyFS::Condition> : true_type {};
} // namespace std
//dex19
#undef FinickyFS
#pragma GCC diagnostic pop

namespace ex15 {  // must be same as above

namespace FinickyFS {
//ex18
bool is_malformed_name(std::error_code ec) {
    return (
        ec == FinickyFS::Error::forbidden_character ||
        ec == FinickyFS::Error::forbidden_word ||
        ec == std::errc::illegal_byte_sequence);
}
//dex18
} // namespace FinickyFS

void test() {
//ex17
    std::error_code ec = FinickyFS::Error::forbidden_character;

        // Comparisons are strongly typed.
    assert(ec == FinickyFS::Error::forbidden_character);
    assert(ec != std::io_errc::stream);
//dex17
}
void test2() {
//ex20
    std::error_code ec = FinickyFS::Error::forbidden_word;

        // RHS is implicitly converted to error_code
    assert(ec == FinickyFS::Error::forbidden_word);

        // RHS is implicitly converted to error_condition
    assert(ec == FinickyFS::Condition::malformed_name);
//dex20
}
} // namespace ex15

namespace ex16 {
using ::std::is_error_code_enum_v;
using ::std::enable_if_t;
//ex16
class error_code {
    // ...
    template<
        class E,
        class = enable_if_t<is_error_code_enum_v<E>>
    >
    error_code(E err) noexcept {
        *this = make_error_code(err);
    }
};
//dex16
} // namespace ex16

namespace ex21 {
#define file_size file_size2
//ex21
// The lower level is error_code-based.
uintmax_t file_size(const fs::path& p,
                    std::error_code& ec) noexcept;

// My level is throw-based.
uintmax_t file_size(const fs::path& p)
{
    std::error_code ec;
    uintmax_t size = file_size(p, ec);
    if (ec) {
        throw std::system_error(ec);
    }
    return size;
}
//dex21
uintmax_t file_size(const fs::path&, std::error_code&) noexcept { return -1; }
#undef file_size
} // namespace ex21

namespace ex22::detail { enum Error : int; }
namespace std { template<> struct is_error_code_enum<::ex22::detail::Error> : true_type {}; }

namespace ex22 {
std::error_code current_exception_to_error_code();
#define file_size file_size2
//ex22
// The lower level is throw-based.
uintmax_t file_size(const fs::path& p);

// My level is error_code-based.
uintmax_t file_size(const fs::path& p,
                    std::error_code& ec) noexcept
{
    uintmax_t size = -1;
    try {
        size = file_size(p);
    } catch (...) {
        ec = current_exception_to_error_code();
    }
    return size;
}
//dex22
uintmax_t file_size(const fs::path&) { return -1; }
#undef file_size
//ex23
namespace detail {

enum Error : int {
    success = 0,
    bad_alloc_thrown = 1,
    unknown_exception_thrown = 2,
};
struct ErrorCategory : std::error_category {
    const char *name() const noexcept override;
    std::string message(int err) const override;
    static ErrorCategory& instance();
};
std::error_code make_error_code(Error err) noexcept {
    return std::error_code(int(err), ErrorCategory::instance());
}

} // namespace detail

std::error_code current_exception_to_error_code()
{
    try {
        throw;
    } catch (const std::system_error& e) {
        // also catches std::ios_base::failure
        // and fs::filesystem_error
        return e.code();
    } catch (const std::future_error& e) {
        // catches the oddball
        return e.code();
    } catch (const std::bad_alloc&) {
        // bad_alloc is often of special interest
        return detail::bad_alloc_thrown;
    } catch (...) {
        return detail::unknown_exception_thrown;
    }
}
//dex23
} // namespace ex22

namespace ex22::detail {
    struct Unreachable{};
    const char *ErrorCategory::name() const noexcept {
        return "current_exception_to_error_code";
    }
    std::string ErrorCategory::message(int err) const {
        switch (err) {
            case 0: return "Success";
            case 1: return "std::bad_alloc";
            case 2: return "unknown exception";
        }
        throw Unreachable();
    }
    ErrorCategory& ErrorCategory::instance() {
        static ErrorCategory instance;
        return instance;
    }
} // namespace ex22::detail

namespace ex24 {
static constexpr bool IsWindows = true;
//ex24
class path {
public:
    using value_type = std::conditional_t<
        IsWindows, wchar_t, char
    >;
    using string_type = std::basic_string<value_type>;

    const auto& native() const { return m_path; }
    operator string_type() const { return m_path; }
    auto c_str() const { return m_path.c_str(); }

    // many constructors and accessors omitted
private:
    string_type m_path;
};
//dex24
} // namespace ex24

namespace ex25 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
void test() {
//ex25
    fs::path p("/foo/bar");

    const fs::path::value_type *a = p.c_str();
        // Portable, for whatever that's worth.

    const char *b = p.c_str();
        // OK on POSIX; compilation error on Windows.

    std::string s = p.u8string();
    const char *c = s.c_str();
        // OK on both POSIX and Windows.
        // Performs 16-to-8 conversion on Windows.
//dex25
}
#pragma GCC diagnostic pop
} // namespace ex25

namespace ex26 {
static constexpr bool IsWindows = false;
//ex27
static fs::path operator+(fs::path a, const fs::path& b)
{
    a += b;
    return a;
}
//dex27
void test() {
    fs::path path("c:/foo/bar/data.tar.gz");
#define X(x) auto x = path.x();
X(root_path) X(root_name) X(root_directory)
X(relative_path) X(parent_path) X(filename)
X(stem) X(extension) X(is_absolute) X(is_relative)
#undef X
//ex26
    assert(root_path == root_name / root_directory);
    assert(path == root_name / root_directory / relative_path);
    assert(path == root_path / relative_path);

    assert(path == parent_path / filename);
    assert(filename == stem + extension);

    assert(is_absolute == !is_relative);
    if (IsWindows) {
        assert(is_relative == (root_name.empty() || root_directory.empty()));
    } else {
        assert(is_relative == (root_name.empty() && root_directory.empty()));
    }
//dex26
}
} // namespace ex26

namespace ex28 {
void test() {
//ex28
    fs::path p = "/foo/bar/baz.txt";
    std::vector<fs::path> v(p.begin(), p.end());
    assert((v == std::vector<fs::path>{
        "/", "foo", "bar", "baz.txt"
    }));
//dex28
}
} // namespace ex28

#if !USE_BOOST
namespace ex29 {
void test() {
    using namespace std::literals;
//ex29
    fs::path p = "/tmp/foo/bar.txt";
    fs::directory_entry entry(p);
        // Here, we still have not touched the file system.

    while (!entry.exists()) {
        std::cout << entry.path() << " does not exist yet\n";
        std::this_thread::sleep_for(100ms);
        entry.refresh();
            // Without refresh(), this would loop forever.
    }
        // If the file is deleted right now, the following
        // line might print stale cached values, or it
        // might try to refresh the cache and throw.
    std::cout << entry.path() << " has size "
              << entry.file_size() << "\n";
//dex29
}
} // namespace ex29
#endif // USE_BOOST

namespace ex30 {
#if USE_BOOST
void test() {
    fs::path p = fs::current_path();
        // List the current directory.
    for (fs::directory_entry entry : fs::directory_iterator(p)) {
        std::cout << entry.path().string() << ": "
                  << file_size(entry.path()) << " bytes\n";
    }
}
#else
void test() {
//ex30
    fs::path p = fs::current_path();
        // List the current directory.
    for (fs::directory_entry entry : fs::directory_iterator(p)) {
        std::cout << entry.path().string() << ": "
                  << entry.file_size() << " bytes\n";
    }
//dex30
}
#endif // USE_BOOST
} // namespace ex30

namespace ex31 {
#if USE_BOOST
template<class F>
void walk_down(const fs::path& p, const F& callback)
{
    for (auto entry : fs::directory_iterator(p)) {
        if (entry.status().type() == fs::file_type::directory_file) {
            walk_down(entry.path(), callback);
        } else {
            callback(entry);
        }
    }
}
#else
//ex31
template<class F>
void walk_down(const fs::path& p, const F& callback)
{
    for (auto entry : fs::directory_iterator(p)) {
        if (entry.is_directory()) {
            walk_down(entry.path(), callback);
        } else {
            callback(entry);
        }
    }
}
//dex31
#endif
} // namespace ex31

namespace ex32 {
//ex32
template<class F>
void walk_down(const fs::path& p, const F& callback)
{
    for (auto entry : fs::recursive_directory_iterator(p)) {
        callback(entry);
    }
}
//dex32
} // namespace ex32

namespace ex33 {
//ex33
struct space_info {
    uintmax_t capacity;
    uintmax_t free;
    uintmax_t available;
};
//dex33
} // namespace ex33

int main()
{
    ex4::test();
    ex13::test();
    ex15::test();
    ex15::test2();
    ex25::test();
    ex26::test();
    ex28::test();
    ex30::test();
}
