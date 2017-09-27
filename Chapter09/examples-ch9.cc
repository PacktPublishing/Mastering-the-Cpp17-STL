#include <cassert>
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <clocale>
#include <fcntl.h>
#include <unistd.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#include <boost/format.hpp>
#pragma GCC diagnostic pop

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
    from_chars_result from_chars(const char* first, const char* last, int& value, int base = 10);
    from_chars_result from_chars(const char* first, const char* last, float& value, chars_format fmt = chars_format::general);
    to_chars_result to_chars(char* first, char* last, int value, int base = 10);
    to_chars_result to_chars(char* first, char* last, float value);
    to_chars_result to_chars(char* first, char* last, float value, chars_format fmt);
    to_chars_result to_chars(char* first, char* last, float value, chars_format fmt, int precision);

} // namespace std
#endif

#if (defined(__GNUC__) && !defined(__clang__))
#pragma GCC diagnostic ignored "-Wformat-truncation"
#endif

namespace ex1 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
//ex1
#include <stdio.h>

int main()
{
    puts("hello world");
}
//dex1
#pragma GCC diagnostic pop
} // namespace ex1

namespace ex2 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
//ex2
#include <iostream>

int main()
{
    std::cout << "hello world" << std::endl;
}
//dex2
#pragma GCC diagnostic pop
} // namespace ex2

namespace ex3 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
//ex3
#include <cassert>
#include <string>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int fdw = open("myfile.txt", O_WRONLY | O_CREAT | O_TRUNC);
    int fdr = open("myfile.txt", O_RDONLY);
    if (fdw == -1 || fdr == -1)
        return EXIT_FAILURE;

    write(fdw, "hello world", 11);
    lseek(fdw, 6, SEEK_SET);
    write(fdw, "neighbor", 8);

    std::string buffer(14, '\0');
    int b = read(fdr, buffer.data(), 14);
    assert(b == 14);
    assert(buffer == "hello neighbor");
    close(fdr);
    close(fdw);
}
//dex3
#pragma GCC diagnostic pop
} // namespace ex3

namespace ex4 {
//ex4
struct fcloser {
    void operator()(FILE *fp) const {
        fclose(fp);
    }

    static auto open(const char *name, const char *mode) {
        return std::unique_ptr<FILE, fcloser>(fopen(name, mode));
    }
};

void test() {
    auto f = fcloser::open("test.txt", "w");
    fprintf(f.get(), "hello world\n");
        // f will be closed automatically
}
//dex4
void test2() {
//ex5
    auto f = fcloser::open("test.txt", "w");
    std::shared_ptr<FILE> g1 = std::move(f);
        // now f is empty and g1's use-count is 1
    if (true) {
        std::shared_ptr<FILE> g2 = g1;
            // g1's use-count is now 2
        fprintf(g2.get(), "hello ");
            // g1's use-count drops back to 1
    }
    fprintf(g1.get(), "world\n");
        // g1's use-count drops to 0; the file is closed
//dex5
}
} // namespace ex4

namespace ex6 {
//ex6
long ftell(FILE *fp)
{
    int fd = fileno(fp);
    return lseek(fd, 0, SEEK_CUR);
}
//dex6
} // namespace ex6

namespace ex7 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
//ex7
struct LWC {
    int lines, words, chars;
};

LWC word_count(FILE *fp)
{
    LWC r {};
    bool in_space = true;
    while (true) {
        int ch = getc(fp);
        if (ch == EOF) break;
        r.lines += (ch == '\n');
        r.words += (in_space && !isspace(ch));
        r.chars += 1;
        in_space = isspace(ch);
    }
    return r;
}

int main(int argc, const char **argv)
{
    FILE *fp = (argc < 2) ? stdin : fopen(argv[1], "r");
    auto [lines, words, chars] = word_count(fp);
    printf("%8d %7d %7d\n", lines, words, chars);
}
//dex7
#pragma GCC diagnostic pop
} // namespace ex7

namespace ex8 {
void test() {
//ex8
    FILE *fp = fopen("myfile.txt", "w");
    int fd = fileno(fp);
    char buffer[150];
    setvbuf(fp, buffer, _IOFBF, 150);
        // setvbuf returns 0 on success, or EOF on failure.

    std::string AAAA(160, 'A');
    int bytes_written = fwrite(AAAA.data(), 1, 160, fp);
        // This fills the buffer with 150 bytes, flushes it,
        // and writes 10 more bytes into the buffer.

    assert(bytes_written == 160);
    assert(lseek(fd, 0, SEEK_CUR) == 150);
    assert(ftell(fp) == 160);
//dex8
//ex9
        // Flush the FILE's buffer by force.
    fflush(fp);
        // Now, fd and fp agree about the state of the file.
    assert(lseek(fd, 0, SEEK_CUR) == 160);
//dex9
}
} // namespace ex8

namespace ex10 {
void test() {
//ex10
    int tuners = 225;
    const char *where = "Chicago";
    printf("There are %d piano tuners in %s.\n", tuners, where);
//dex10
}
} // namespace ex10

namespace ex11 {
#if 0
//ex11
    tuners = 225
    where = "Chicago"
    print "There are %d piano tuners in %s." % (tuners, where)
//dex11
#endif
} // namespace ex11

namespace ex12 {
void test() {
    int tuners = 225;
    const char *where = "Chicago";
//ex12
    char buf[13];
    int needed = snprintf(
        buf, sizeof buf,
        "There are %d piano tuners in %s", tuners, where
    );
    assert(needed == 37);
    assert(std::string_view(buf) == "There are 22");
//dex12
}
} // namespace ex12

namespace ex13 {
int tuners = 225;
const char *where = "Chicago";
//ex13
template<class... Args>
std::string format(const char *fmt, const Args&... args)
{
    int needed = snprintf(nullptr, 0, fmt, args...);
    std::string s(needed + 1, '\0');
    snprintf(s.data(), s.size(), fmt, args...);
    s.pop_back();  // remove the written '\0'
    return s;
}

void test()
{
    std::string s = format("There are %d piano tuners in %s", tuners, where);
    assert(s == "There are 225 piano tuners in Chicago");
}
//dex13
} // namespace ex13

namespace ex14 {
std::string better_format(const char *fmt, ...)
        __attribute((format(printf, 1, 2)));
//ex14
std::string better_format(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int needed = vsnprintf(nullptr, 0, fmt, ap);
    va_end(ap);
    std::string s(needed + 1, '\0');
    va_start(ap, fmt);
    vsnprintf(s.data(), s.size(), fmt, ap);
    va_end(ap);
    s.pop_back();  // remove the written '\0'
    return s;
}
//dex14
void test() {
    int tuners = 225;
    const char *where = "Chicago";
    std::string s = better_format("There are %d piano tuners in %s", tuners, where);
    assert(s == "There are 225 piano tuners in Chicago");
}
} // namespace ex14

namespace ex15 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
//ex15
#include <cassert>
#include <fstream>
#include <string>

int main()
{
    std::fstream fsw("myfile.txt", std::ios_base::out);
    std::fstream fsr("myfile.txt", std::ios_base::in);
    if (fsw.fail() || fsr.fail())
        return EXIT_FAILURE;

    fsw.write("hello world", 11);
    fsw.seekp(6, std::ios_base::beg);
    fsw.write("neighbor", 8);
    fsw.flush();

    std::string buffer(14, '\0');
    fsr.read(buffer.data(), 14);
    assert(fsr.gcount() == 14 && buffer == "hello neighbor");
}
//dex15
#pragma GCC diagnostic pop
} // namespace ex15

namespace ex16 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
//ex16
#include <cassert>
#include <cstdio>
#include <string>

int main()
{
    FILE *fpw = fopen("myfile.txt", "w");
    FILE *fpr = fopen("myfile.txt", "r");
    if (fpw == nullptr || fpr == nullptr)
        return EXIT_FAILURE;

    fwrite("hello world", 1, 11, fpw);
    fseek(fpw, 6, SEEK_SET);
    fwrite("neighbor", 1, 8, fpw);
    fflush(fpw);

    std::string buffer(14, '\0');
    int b = fread(buffer.data(), 1, 14, fpr);
    assert(b == 14 && buffer == "hello neighbor");
    fclose(fpr);
    fclose(fpw);
}
//dex16
#pragma GCC diagnostic pop
} // namespace ex16

namespace ex17 {
void test() {
//ex17
    int tuners = 225;
    const char *where = "Chicago";
    std::cout << "There are " << tuners << " piano tuners in " << where << "\n";
//dex17
}
} // namespace ex17

namespace ex18 {
namespace std { using ::std::ostream; using ::std::string; }
//ex18
namespace std {
    ostream& operator<< (ostream& os, const string& s)
    {
        os.write(s.data(), s.size());
        return os;
    }
} // namespace std
//dex18
} // namespace ex18

namespace ex19 {
int computation_that_may_throw() { throw 42; }
//ex19
void test() {
    try {
        std::cout << "There are "
                  << computation_that_may_throw()
                  << "piano tuners here.\n";
    } catch (...) {
        std::cout << "An exception was thrown";
    }
}
//dex19
} // namespace ex19

namespace ex20 {

//ex20
void pad(std::ostream& os, size_t from, size_t to)
{
    char ch = os.fill();
    for (auto i = from; i < to; ++i) {
        os.write(&ch, 1);
    }
}

std::ostream& operator<< (std::ostream& os, const std::string& s)
{
    auto column_width = os.width();
    auto padding = os.flags() & std::ios_base::adjustfield;

    if (padding == std::ios_base::right) {
        pad(os, s.size(), column_width);
    }
    os.write(s.data(), s.size());
    if (padding == std::ios_base::left) {
        pad(os, s.size(), column_width);
    }
    os.width(0);  // reset "column width" to 0
    return os;
}
//dex20

//ex21
struct WidthSetter { int n; };

auto& operator<< (std::ostream& os, WidthSetter w)
{
    os.width(w.n);
    return os;
}

auto setw(int n) { return WidthSetter{n}; }
//dex21

//ex48
using Manip = std::ostream& (*)(std::ostream&);

auto& operator<< (std::ostream& os, Manip f) {
    return f(os);
}

std::ostream& flush(std::ostream& os) {
    return os.flush();
}

std::ostream& endl(std::ostream& os) {
    return os << '\n' << flush;
}
//dex48

void test() {
    int tuners = 225;
    const char *where = "Chicago";
//ex22
    printf("%-10s.%6x\n", where, tuners);
        // "Chicago .  e1"

    std::cout << std::setw(8) << std::left << where << "."
              << std::setw(4) << std::right << std::hex
              << tuners << "\n";
        // "Chicago .  e1"
//dex22

//ex23
    printf("%d\n", 42);  // "42"

    std::cout << 42 << "\n";  // "2a" -- oops!
//dex23
} // test
} // namespace ex20

namespace ex24 {
//ex24
void test() {
    std::ios old_state(nullptr);
    old_state.copyfmt(std::cout);
        std::cout << std::hex << 225;  // "e1"
    std::cout.copyfmt(old_state);

    std::cout << 42;  // "42"
}
//dex24
} // namespace ex24

namespace ex25 {
//ex25
void test() {
    std::ostream os(std::cout.rdbuf());
        os << std::hex << 225;  // "e1"

    std::cout << 42;  // "42"
}
//dex25
} // namespace ex25

namespace ex26 {
//ex26
template<class T>
std::string to_string(T&& t)
{
    std::ostringstream oss;
    oss << std::forward<T>(t);
    return oss.str();
}
//dex26
void test() {
    auto s = to_string(42);
    assert(s == "42");
}
} // namespace ex26

namespace ex27 {
//ex27
template<class... Ts>
std::string to_string(Ts&&... ts)
{
    std::ostringstream oss;
    (oss << ... << std::forward<Ts>(ts));
    return oss.str();
}
//dex27
void test() {
    auto a = to_string(42, " ", std::hex, 17);
    assert(a == "42 11");
//ex28
    std::setlocale(LC_ALL, "C.UTF-8");
    std::locale::global(std::locale("C.UTF-8"));

    auto json = to_string('[', 3.14, ']');
    assert(json == "[3.14]");  // Success!

    std::setlocale(LC_ALL, "en_DK.UTF-8");
    std::locale::global(std::locale("en_DK.UTF-8"));

    json = to_string('[', 3.14, ']');
    assert(json == "[3,14]");  // Silent, abject failure!
//dex28
}
} // namespace ex27

namespace ex29 {
//ex29
template<class InputIt, class OutputIt>
OutputIt do_quote(InputIt begin, InputIt end,
                  OutputIt dest)
{
    *dest++ = '"';
    while (begin != end) {
        auto ch = *begin++;
        if (ch == '"') {
            *dest++ = '\\';
        }
        *dest++ = ch;
    }
    *dest++ = '"';
    return dest;
}
//dex29
//ex30
struct quoted {
    std::string_view m_view;
    quoted(const char *s) : m_view(s) {}
    quoted(const std::string& s) : m_view(s) {}
};

auto& operator<< (std::ostream& os, const quoted& q)
{
    do_quote(
        q.m_view.begin(),
        q.m_view.end(),
        std::ostreambuf_iterator<char>(os)
    );
    return os;
}
//dex30
void test() {
//ex31
    std::cout << quoted("I said \"hello\".");
//dex31
    int tuners = 225;
    const char *where = "Chicago";
//ex32
    std::cout << boost::format("There are %d piano tuners in %s.") % tuners % where << std::endl;
//dex32
}
} // namespace ex29

namespace ex33 {
void test() {
//ex33
    std::ostringstream oss;
    std::string str;
    char buffer[100];
    int intvalue = 42;
    float floatvalue = 3.14;
    std::to_chars_result r;
//dex33
//ex34
    snprintf(buffer, sizeof buffer, "%d", intvalue);
        // available in <stdio.h>
        // locale-independent (%d is unaffected by locales)
        // non-allocating
        // bases 8, 10, 16 only

    oss << intvalue;
    str = oss.str();
        // available in <sstream>
        // locale-problematic (thousands separator may be inserted)
        // allocating; allocator-aware
        // bases 8, 10, 16 only

    str = std::to_string(intvalue);
        // available since C++11 in <string>
        // locale-independent (equivalent to %d)
        // allocating; NOT allocator-aware
        // base 10 only

    r = std::to_chars(buffer, std::end(buffer), intvalue, 10);
    *r.ptr = '\0';
        // available since C++17 in <charconv>
        // locale-independent by design
        // non-allocating
        // bases 2 through 36
//dex34
    if (false) {
        std::string body = "hello world";
//ex35
        std::string response =
            "Content-Length: " + std::to_string(body.size()) + "\r\n" +
            "\r\n" +
            body;
//dex35
        assert(response == "Content-Length: 11\r\n\r\nhello world");
    }
//ex37
    snprintf(buffer, sizeof buffer, "%.6e", floatvalue);
    snprintf(buffer, sizeof buffer, "%.6f", floatvalue);
    snprintf(buffer, sizeof buffer, "%.6g", floatvalue);
        // available in <stdio.h>
        // locale-problematic (decimal point)
        // non-allocating

    oss << floatvalue;
    str = oss.str();
        // available in <sstream>
        // locale-problematic (decimal point)
        // allocating; allocator-aware

    str = std::to_string(floatvalue);
        // available since C++11 in <string>
        // locale-problematic (equivalent to %f)
        // allocating; NOT allocator-aware
        // no way to adjust the formatting

    r = std::to_chars(buffer, std::end(buffer), floatvalue,
                      std::chars_format::scientific, 6);
    r = std::to_chars(buffer, std::end(buffer), floatvalue,
                      std::chars_format::fixed, 6);
    r = std::to_chars(buffer, std::end(buffer), floatvalue,
                      std::chars_format::general, 6);
    *r.ptr = '\0';
        // available since C++17 in <charconv>
        // locale-independent by design
        // non-allocating
//dex37
}

//ex36
char *write_string(char *p, char *end, const char *from)
{
    while (p != end && *from != '\0') *p++ = *from++;
    return p;
}

char *write_response_headers(char *p, char *end, std::string body)
{
    p = write_string(p, end, "Content-Length: ");
    p = std::to_chars(p, end, body.size(), 10).ptr;
    p = write_string(p, end, "\r\n\r\n");
    return p;
}
//dex36

} // namespace ex33

namespace ex39 {
void test() {
//ex38
    std::istringstream iss;
    std::string str = "42";
    char buffer[] = "42";
    int intvalue;
    float floatvalue;
    int rc;
    char *endptr;
    size_t endidx;
    std::from_chars_result r;
//dex38
//ex39
    intvalue = strtol(buffer, &endptr, 10);
        // saturates on overflow
        // sets global "errno" on most errors
        // sets endptr==buffer when input cannot be parsed
        // available in <stdlib.h>
        // locale-problematic, in theory
        // non-allocating
        // bases 0 and 2 through 36
        // always skips leading whitespace
        // skips leading 0x for base 16
        // recognizes upper and lower case

    rc = sscanf(buffer, "%d", &intvalue);
        // fails to detect overflow
        // returns 0 (instead of 1) when input cannot be parsed
        // available in <stdio.h>
        // locale-problematic (equivalent to strtol)
        // non-allocating
        // bases 0, 8, 10, 16 only
        // always skips leading whitespace
        // skips leading 0x for base 16
        // recognizes upper and lower case

    intvalue = std::stoi(str, &endidx, 10);
        // throws on overflow or error
        // available since C++11 in <string>
        // locale-problematic (equivalent to strtol)
        // NOT allocator-aware
        // bases 0 and 2 through 36
        // always skips leading whitespace
        // skips leading 0x for base 16
        // recognizes upper and lower case

    iss.str("42");
    iss >> intvalue;
        // saturates on overflow
        // sets iss.fail() on any error
        // available in <sstream>
        // locale-problematic
        // allocating; allocator-aware
        // bases 8, 10, 16 only
        // skips leading 0x for base 16
        // skips whitespace by default

    r = std::from_chars(buffer, buffer + 2, intvalue, 10);
        // sets r.ec != 0 on any error
        // available since C++17 in <charconv>
        // locale-independent by design
        // non-allocating
        // bases 2 through 36
        // always skips leading whitespace
        // recognizes lower case only
//dex39
    (void)rc;

//ex40
    floatvalue = strtof(buffer, &endptr);
        // saturates on overflow
        // sets global "errno" on most errors
        // sets endptr==buffer when input cannot be parsed
        // available in <stdlib.h>
        // locale-problematic
        // non-allocating
        // base 10 or 16, auto-detected
        // always skips leading whitespace

    rc = sscanf(buffer, "%f", &floatvalue);
        // fails to detect overflow
        // returns 0 (instead of 1) when input cannot be parsed
        // available in <stdio.h>
        // locale-problematic (equivalent to strtof)
        // non-allocating
        // base 10 or 16, auto-detected
        // always skips leading whitespace

    floatvalue = std::stof(str, &endidx);
        // throws on overflow or error
        // available since C++11 in <string>
        // locale-problematic (equivalent to strtol)
        // NOT allocator-aware
        // base 10 or 16, auto-detected
        // always skips leading whitespace

    iss.str("3.14");
    iss >> floatvalue;
        // saturates on overflow
        // sets iss.fail() on any error
        // available in <sstream>
        // locale-problematic
        // allocating; allocator-aware
        // base 10 or 16, auto-detected
        // skips whitespace by default
        // non-portable behavior on trailing text

    r = std::from_chars(buffer, buffer + 2, floatvalue,
                        std::chars_format::general);
        // sets r.ec != 0 on any error
        // available since C++17 in <charconv>
        // locale-independent by design
        // non-allocating
        // base 10 or 16, auto-detected
        // always skips leading whitespace
//dex40
}
} // namespace ex39

namespace ex41 {
void test() {
//ex41
    double d = 17;
    std::istringstream iss("42abc");
    iss >> d;
    if (iss.good() && d == 42) {
        puts("Your library vendor is libstdc++");
    } else if (iss.fail() && d == 0) {
        puts("Your library vendor is libc++");
    }
//dex41
}
} // namespace ex41

namespace ex42 {
void test() {
    std::locale::global(std::locale("C.UTF-8"));
//ex42
    std::setlocale(LC_ALL, "C.UTF-8");

    assert(std::stod("3.14") == 3.14);  // Success!

    std::setlocale(LC_ALL, "en_DK.UTF-8");

    assert(std::stod("3.14") == 3.00);  // Silent, abject failure!
//dex42
}
} // namespace ex42

namespace ex43 {
#if 0
//ex43
for line in sys.stdin:
    # preserves trailing newlines
    process(line)
//dex43
//ex44
while (<>) {
    # preserves trailing newlines
    process($_);
}
//dex44
#endif
void process(const std::string&) { puts("process"); }
void test() {
//ex45
    std::string line;
    while (std::getline(std::cin, line)) {
        // automatically chomps trailing newlines
        process(line);
    }
//dex45
}
} // namespace ex43

namespace ex46 {
void process(const std::string& w) { puts(w.data()); }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
//ex46
template<class T>
struct streamer {
    std::istream& m_in;
    explicit streamer(std::istream& in) : m_in(in) {}
    auto begin() const
        { return std::istream_iterator<T>(m_in); }
    auto end() const
        { return std::istream_iterator<T>{}; }
};

int main()
{
    for (auto word : streamer<std::string>(std::cin)) {
        process(word);
    }
}
//dex46
#pragma GCC diagnostic pop

void test()
{
//ex47
    // Double every int the user gives us
    for (auto value : streamer<int>(std::cin)) {
        printf("%d\n", 2*value);
    }
//dex47
}
} // namespace ex46

int main()
{
    FILE *fp = fopen("myfile.txt", "w");
    fclose(fp);
    ex1::main();
    ex2::main();
    ex3::main();
    ex4::test();
    ex4::test2();
    const char *argv[] = { "", "myfile.txt" };
    ex7::main(1, argv);
    ex8::test();
    ex10::test();
    ex12::test();
    ex13::test();
    ex14::test();
    ex20::test();
    ex24::test();
    ex25::test();
    ex26::test();
    ex27::test();
    ex29::test();
    ex33::test();
    ex39::test();
    ex41::test();
    ex42::test();
    ex46::test();
}
