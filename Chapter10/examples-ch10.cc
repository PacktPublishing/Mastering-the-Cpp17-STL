#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <iterator>
#include <list>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

namespace ex1 {
/*
//ex1
    ^ $ \ . * + ? ( ) [ ] { } |
//dex1
*/
}//namespace ex1

namespace ex2 {
//ex2
void print_help() {
    puts(R"(The regex special characters are:
    \   - escaping
    |   - separating alternatives
    .   - match any character
    []  - character class or set
    ()  - capturing parentheses, or lookahead
    ?*+ - "zero or one", "zero or more", "one or more"
    {}  - "exactly N" or "M to N" repetitions
    ^$  - beginning and end of a "line"
    \b  - word boundary
    \d \s \w - digit, space, and word
    (?=foo) (?!foo) - lookahead; negative lookahead
)");
}
//dex2
} // namespace ex2

namespace ex3 {
void process_command(const std::string&) {}
void test() {
//ex3
    std::regex rx("(left|right) ([0-9]+)");
        // Construct the regex object "rx" outside the loop.
    std::string line;
    while (std::getline(std::cin, line)) {
        // Inside the loop, use the same "rx" over and over.
        if (std::regex_match(line, rx)) {
            process_command(line);
        } else {
            puts("Unrecognized command.");
        }
    }
//dex3
}
} // namespace ex3

namespace ex4 {
void test() {
//ex4
    std::string s = "abcde";
    std::smatch m;
    std::regex_match(s, m, std::regex(".*([bcd].*)e"));
    assert(m[1] == "d");
    std::regex_match(s, m, std::regex(".*?([bcd].*)e"));
    assert(m[1] == "bcd");
//dex4
}
} // namespace ex4

namespace ex5 {
void test() {
//ex5
    std::string s = "abcde";
    std::smatch m;
    std::regex_match(s, m, std::regex("(a|b)*(.*)e"));
    assert(m.size() == 3 && m[2] == "cd");
    std::regex_match(s, m, std::regex("(?:a|b)*(.*)e"));
    assert(m.size() == 2 && m[1] == "cd");
//dex5
}
} // namespace ex5

namespace ex6 {
void test() {
#if 0 // multiline isn't in libstdc++ or libc++ yet
//ex6
    std::string s = "ab\ncd";
    std::regex rx("^ab$[^]^cd$", std::regex::multiline);

    assert(std::regex_match(s, rx));
//dex6
#endif
}
} // namespace ex6

namespace ex7 {
void process_command(const std::string&) {}
void test() {
//ex7
    std::regex rx("(left|right) ([0-9]+)");
    std::string line;
    while (std::getline(std::cin, line)) {
        if (std::regex_match(line, rx)) {
            process_command(line);
        } else {
            printf("Unrecognized command '%s'.\n",
                   line.c_str());
        }
    }
//dex7
}
} // namespace ex7

namespace ex8 {
static int count = 0;
void process_command(const char *, const char *) {
    ++count;
}
//ex8
void parse(const char *p, const char *end)
{
    static std::regex rx("(left|right) ([0-9]+)");
    if (std::regex_match(p, end, rx)) {
        process_command(p, end);
    } else {
        printf("Unrecognized command '%.*s'.\n",
               int(end - p), p);
    }
}
//dex8
void test() {
    char buf[] = "left 20";
    parse(buf, buf + 7);
    assert(count == 1);
}

} // namespace ex8

namespace ex9 {
//ex9
std::pair<std::string, std::string>
parse_command(const std::string& line)
{
    static std::regex rx("(left|right) ([0-9]+)");
    std::smatch m;
    if (std::regex_match(line, m, rx)) {
        return { m[1], m[2] };
    } else {
        throw "Unrecognized command!";
    }
}

void test() {
    auto [dir, dist] = parse_command("right 4");
    assert(dir == "right" && dist == "4");
}
//dex9
} // namespace ex9

namespace ex10 {
//ex10
std::pair<std::string, std::string>
parse_command(const char *p, const char *end)
{
    static std::regex rx("(left|right) ([0-9]+)");
    std::cmatch m;
    if (std::regex_match(p, end, m, rx)) {
        return { m[1], m[2] };
    } else {
        throw "Unrecognized command!";
    }
}

void test() {
    char buf[] = "left 20";
    auto [dir, dist] = parse_command(buf, buf + 7);
    assert(dir == "left" && dist == "20");
}
//dex10
} // namespace ex10

namespace ex11 {
void bad() {
//ex11
    static std::regex rx("(left|right) ([0-9]+)");
    std::string line = "left 20";
    std::smatch m;
    std::regex_match(line, m, rx);
        // m[1] now holds iterators into line
    line = "hello world";
        // reallocate line's underlying buffer
    std::string oops = m[1];
        // this invokes undefined behavior because
        // of iterator invalidation
//dex11
    (void)oops;
}
void worse() {
#if 0  // this overload is deleted, fortunately
//ex12
    static std::regex rx("(left|right) ([0-9]+)");
    std::smatch m;
    std::regex_match("left 20", m, rx);
        // m[1] would hold iterators into a temporary
        // string, so they would ALREADY be invalid.
        // Fortunately this overload is deleted.
//dex12
#endif
}
} // namespace ex11

namespace ex13 {
//ex13
template<class Iter>
std::pair<std::string, std::string>
parse_command(Iter begin, Iter end)
{
    static std::regex rx("(left|right) ([0-9]+)");
    std::match_results<Iter> m;
    if (std::regex_match(begin, end, m, rx)) {
        return { m.str(1), m.str(2) };
    } else {
        throw "Unrecognized command!";
    }
}

void test() {
    char buf[] = "left 20";
    std::list<char> lst(buf, buf + 7);
    auto [dir, dist] = parse_command(lst.begin(), lst.end());
    assert(dir == "left" && dist == "20");
}
//dex13
} // namespace ex13

namespace ex14 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
//ex14
int main()
{
    std::regex rx("(left|right) ([0-9]+)");
    int pos = 0;
    std::string line;
    while (std::getline(std::cin, line)) {
        try {
            std::smatch m;
            if (!std::regex_match(line, m, rx)) {
                throw std::runtime_error("Failed to lex");
            }
            int how_far = std::stoi(m.str(2));
            int direction = (m[1] == "left") ? -1 : 1;
            pos += how_far * direction;
            printf("Robot is now at %d.\n", pos);
        } catch (const std::exception& e) {
            puts(e.what());
            printf("Robot is still at %d.\n", pos);
        }
    }
}
//dex14
#pragma GCC diagnostic pop
} // namespace ex14

namespace ex15 {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
//ex15
int main()
{
    std::regex rx("((left)|right) (-?[0-9]+)", std::regex::icase);
    int pos = 0;
    std::string line;
    while (std::getline(std::cin, line)) {
        try {
            std::smatch m;
            if (!std::regex_match(line, m, rx)) {
                throw std::runtime_error("Failed to lex");
            }
            int how_far = std::stoi(m.str(3));
            int direction = m[2].matched ? -1 : 1;
            pos += how_far * direction;
            printf("Robot is now at %d.\n", pos);
        } catch (const std::exception& e) {
            puts(e.what());
            printf("Robot is still at %d.\n", pos);
        }
    }
}
//dex15
#pragma GCC diagnostic pop
} // namespace ex15

namespace ex16 {
//ex16
auto get_all_matches(
    const char *begin, const char *end,
    const std::regex& rx,
    bool be_correct)
{
    auto flags = be_correct ?
        std::regex_constants::match_prev_avail :
        std::regex_constants::match_default;
    std::vector<std::string> result;
    std::cmatch m;
    std::regex_search(begin, end, m, rx);
    while (!m.empty()) {
        result.push_back(m[0]);
        begin = m[0].second;
        std::regex_search(begin, end, m, rx, flags);
    }
    return result;
}

void test() {
    char buf[] = "baby";
    std::regex rx("\\bb.");
      // get the first 2 letters of each word starting with "b"
    auto v = get_all_matches(buf, buf+4, rx, false);
    assert(v.size() == 2);
      // oops, "by" is considered to start on a word boundary!

    v = get_all_matches(buf, buf+4, rx, true);
    assert(v.size() == 1);
      // "by" is correctly seen as part of the word "baby"
}
//dex16
} // namespace ex16

namespace ex17 {
auto get_all_matches(
    const char *begin, const char *end,
    const std::regex& rx,
    bool be_correct)
{
    auto flags = be_correct ?
        std::regex_constants::match_prev_avail :
        std::regex_constants::match_default;
    std::vector<std::string> result;
    std::cmatch m;
    std::regex_search(begin, end, m, rx);
//ex17
    while (!m.empty()) {
        result.push_back(m[0]);
        begin = m[0].second;
        if (begin == end) break;
        if (m[0].length() == 0) ++begin;
        if (begin == end) break;
        std::regex_search(begin, end, m, rx, flags);
    }
//dex17
    return result;
}

void test() {
    char buf[] = "baby";
    std::regex rx("\\bb.");
      // retrieve the first 2 letters of each word starting with "b"
    auto v = get_all_matches(buf, buf+4, rx, false);
    assert(v.size() == 2);
      // oops, "by" is considered to start on a word boundary!

    v = get_all_matches(buf, buf+4, rx, true);
    assert(v.size() == 1);
      // "by" is correctly seen as part of the word "baby"
}
} // namespace ex17

namespace ex18 {
//ex18
auto get_all_matches(
    const char *begin, const char *end,
    const std::regex& rx)
{
    std::vector<std::string> result;
    using It = std::cregex_iterator;
    for (It it(begin, end, rx); it != It{}; ++it) {
        auto m = *it;
        result.push_back(m[0]);
    }
    return result;
}
//dex18
void test() {
    char buf[] = "baby";
    std::regex rx("\\bb.");
      // retrieve the first 2 letters of each word starting with "b"
    auto v = get_all_matches(buf, buf+4, rx);
    assert(v.size() == 1);
      // "by" is correctly seen as part of the word "baby"
}
} // namespace ex18

namespace ex19 {
//ex19
auto get_tokens(const char *begin, const char *end,
                const std::regex& rx)
{
    std::vector<std::string> result;
    using It = std::cregex_iterator;
    std::optional<std::csub_match> opt_suffix;
    for (It it(begin, end, rx); it != It{}; ++it) {
        auto m = *it;
        std::csub_match nonmatching_part = m.prefix();
        result.push_back(nonmatching_part);
        std::csub_match matching_part = m[0];
        result.push_back(matching_part);
        opt_suffix = m.suffix();
    }
    if (opt_suffix.has_value()) {
        result.push_back(opt_suffix.value());
    }
    return result;
}
//dex19
} // namespace ex19

namespace ex20 {
//ex20
auto get_tokens(const char *begin, const char *end,
                const std::regex& rx)
{
    std::vector<std::string> result;
    using TokIt = std::cregex_token_iterator;
    for (TokIt it(begin, end, rx, {-1, 0}); it != TokIt{}; ++it) {
        std::csub_match some_part = *it;
        result.push_back(some_part);
    }
    return result;
}
//dex20
} // namespace ex20

namespace ex21 {
void test()
{
//ex21
    std::string input = "abc123...456...";
    std::vector<std::ssub_match> v;
    std::regex rx("([0-9]+)|([a-z]+)");
    using TokIt = std::sregex_token_iterator;
    std::copy(
        TokIt(input.begin(), input.end(), rx, {1, 2}),
        TokIt(),
        std::back_inserter(v)
    );
    assert(!v[0].matched); assert(v[1] == "abc");
    assert(v[2] == "123"); assert(!v[3].matched);
    assert(v[4] == "456"); assert(!v[5].matched);
//dex21
}
} // namespace ex21

namespace ex22 {
void test()
{
//ex22
    std::string s = "apples and bananas";
    std::string t = std::regex_replace(s, std::regex("a"), "e");
    assert(t == "epples end benenes");
    std::string u = std::regex_replace(s, std::regex("[ae]"), "u");
    assert(u == "upplus und bununus");
//dex22
}
} // namespace ex22

namespace ex23 {
void test()
{
//ex23
    auto s = "std::sort(std::begin(v), std::end(v))";
    auto t = std::regex_replace(s, std::regex("\\bstd::(\\w+)"), "$1");
    assert(t == "sort(begin(v), end(v))");
    auto u = std::regex_replace(s, std::regex("\\bstd::(\\w+)"), "my::$1");
    assert(u == "my::sort(my::begin(v), my::end(v))");
//dex23
}
} // namespace ex23

namespace ex24 {
//ex24
template<class F>
std::string regex_replace(std::string_view haystack,
                          const std::regex& rx, const F& f)
{
    std::string result;
    const char *begin = haystack.data();
    const char *end = begin + haystack.size();
    std::cmatch m, lastm;
    if (!std::regex_search(begin, end, m, rx)) {
        return std::string(haystack);
    }
    do {
        lastm = m;
        result.append(m.prefix());
        result.append(f(m));
        begin = m[0].second;
        begin += (begin != end && m[0].length() == 0);
        if (begin == end) break;
    } while (std::regex_search(begin, end, m, rx,
        std::regex_constants::match_prev_avail));
    result.append(lastm.suffix());
    return result;
}

void test()
{
    auto s = "std::sort(std::begin(v), std::end(v))";
    auto t = regex_replace(s, std::regex("\\bstd::(\\w+)"),
        [](auto&& m) {
            std::string result = m[1].str();
            std::transform(m[1].first, m[1].second,
                           begin(result), ::toupper);
            return result;
        });
    assert(t == "SORT(BEGIN(v), END(v))");
}
//dex24
} // namespace ex24

int main()
{
    ex2::print_help();
    ex4::test();
    ex6::test();
    ex8::test();
    ex9::test();
    ex10::test();
    ex13::test();
    ex16::test();
    ex17::test();
    ex18::test();
    ex21::test();
    ex22::test();
    ex23::test();
    ex24::test();
}
