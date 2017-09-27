#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <random>
#include <string>
#include <vector>

namespace ex1 {
//ex1
template<class T>
class SimplePRNG {
    uint32_t state = 1;
public:
    static constexpr T min() { return 0; }
    static constexpr T max() { return 0x7FFF; }

    T operator()() {
        state = state * 1103515245 + 12345;
        return (state >> 16) & 0x7FFF;
    }
};
//dex1
void test() {
    SimplePRNG<int> g;
    assert(g() == 16838);
    assert(g() == 5758);
    assert(g() == 10113);
}
} // namespace ex1

namespace ex2 {
//ex2
#include <stdlib.h>

int randint0(int x) {
    return rand() % x;
}
//dex2
void test() {
    for (int i=10; i < 100; ++i) {
        int x = randint0(i);
        assert(0 <= x && x < i);
    }
}
} // namespace ex2

namespace ex3 {
#define DEBUG_LOG(x)
//ex3
int heads(int n) {
    DEBUG_LOG("heads");
    int result = 0;
    for (int i = 0; i < n; ++i) {
        result += (rand() % 2);
    }
    return result;
}

void test_heads() {
    srand(17);  // nail down the seed
    int result = heads(42);
    assert(result == 27);
}
//dex3
#undef DEBUG_LOG
} // namespace ex3

namespace ex4 {
void test() {
//ex4
    std::random_device rd;
    unsigned int seed = rd();
    assert(rd.min() <= seed && seed <= rd.max());
//dex4
}
} // namespace ex4

namespace ex5 {
void test() {
//ex5
    std::mt19937 g;
    assert(g.min() == 0 && g.max() == 4294967295);

    assert(g() == 3499211612);
    assert(g() == 581869302);
    assert(g() == 3890346734);
//dex5
}
} // namespace ex5

namespace ex6 {

//ex7
template<class It>
struct SeedSeq {
    It begin_;
    It end_;
public:
    SeedSeq(It begin, It end) : begin_(begin), end_(end) {}

    template<class It2>
    void generate(It2 b, It2 e) {
        assert((e - b) <= (end_ - begin_));
        std::copy(begin_, begin_ + (e - b), b);
    }
};
//dex7

void test() {
//ex6
    std::random_device rd;

    uint32_t numbers[624];
    std::generate(numbers, std::end(numbers), std::ref(rd));
        // Generate initial state.

    SeedSeq sseq(numbers, std::end(numbers));
        // Copy our state into a heap-allocated "seed sequence".

    std::mt19937 g(sseq);
        // Initialize a mt19937 generator with our state.
//dex6
}
} // namespace ex6

namespace ex8 {
void test() {
//ex8
    std::random_device rd;

    std::mt19937 g(rd());
        // 32 bits of randomness ought to be enough for anyone!
        // ...Right?
//dex8
}
} // namespace ex8

namespace ex9 {
void test() {
//ex9
    std::vector<uint32_t> raw(10), filtered(10);

    std::discard_block_engine<std::mt19937, 3, 2> g2;
    std::mt19937 g1 = g2.base();

    std::generate(raw.begin(), raw.end(), g1);
    std::generate(filtered.begin(), filtered.end(), g2);

    assert(raw[0] == filtered[0]);
    assert(raw[1] == filtered[1]);
    // raw[2] doesn't appear in filtered[]
    assert(raw[3] == filtered[2]);
    assert(raw[4] == filtered[3]);
    // raw[5] doesn't appear in filtered[]
//dex9
}
} // namespace ex9

namespace ex10 {
//ex10
using knuth_b = std::shuffle_order_engine<
    std::linear_congruential_engine<
        uint_fast32_t, 16807, 0, 2147483647
    >,
    256
>;
//dex10
} // namespace ex10

namespace ex11 {
void test() {
//ex11
    std::independent_bits_engine<
        std::mt19937, 40, uint64_t> g2;
    std::mt19937 g1 = g2.base();

    assert(g1() == 0xd09'1bb5c);  // Take "1bb5c"...
    assert(g1() == 0x22a'e9ef6);  // and "e9ef6"...
    assert(g2() == 0x1bb5c'e9ef6); // Paste and serve!
//dex11
//ex12
    using coinflipper = std::independent_bits_engine<
        std::mt19937, 1, uint8_t>;

    coinflipper onecoin;
    std::array<int, 64> results;
    std::generate(results.begin(), results.end(), onecoin);
    assert((results == std::array<int, 64>{{
        0,0,0,1, 0,1,1,1, 0,1,1,1, 0,0,1,0,
        1,0,1,0, 1,1,1,1, 0,0,0,1, 0,1,0,1,
        1,0,0,1, 1,1,1,0, 0,0,1,0, 1,0,1,0,
        1,0,0,1, 0,0,0,0, 0,1,0,0, 1,1,0,0,
    }}));

    std::independent_bits_engine<coinflipper, 32, uint32_t> manycoins;
    assert(manycoins() == 0x1772af15);
    assert(manycoins() == 0x9e2a904c);
//dex12
}
} // namespace ex11

namespace ex13 {
//ex13
template<class Int>
class uniform_int_distribution {
    using UInt = std::make_unsigned_t<Int>;
    UInt m_min, m_max;
public:
    uniform_int_distribution(Int a, Int b) :
        m_min(a), m_max(b) {}

    template<class Gen>
    Int operator()(Gen& g) {
        UInt range = (m_max - m_min);
        assert(g.max() - g.min() >= range);
        while (true) {
            UInt r = g() - g.min();
            if (r <= range) {
                return Int(m_min + r);
            }
        }
    }
};
//dex13
void test() {
    std::mt19937 g;
    uniform_int_distribution<short> d(576, 4979);
    for (int i=0; i < 100; ++i) {
        assert(576 <= d(g) && d(g) <= 4979);
    }
}
namespace std { using ex13::uniform_int_distribution; using ::std::mt19937; }
//ex14
int randint0(int x) {
    static std::mt19937 g;
    std::uniform_int_distribution<int> dist(0, x-1);
    return dist(g);
}
//dex14
#define randint0 randint1
//ex15
int randint0(int x) {
    static std::mt19937 g;
    return std::uniform_int_distribution<int>(0, x-1)(g);
}
//dex15
#undef randint0
} // namespace ex13

namespace ex16 {
void test() {
//ex16
    double mean = 161.8;
    double stddev = 6.8;
    std::normal_distribution<double> dist(mean, stddev);

        // Initialize our generator.
    std::mt19937 g(std::random_device{}());

        // Fill a vector with 10,000 samples.
    std::vector<double> v;
    for (int i=0; i < 10000; ++i) {
        v.push_back( dist(g) );
    }
    std::sort(v.begin(), v.end());

        // Compare expectations with reality.
    auto square = [](auto x) { return x*x; };
    double mean_of_values = std::accumulate(v.begin(), v.end(), 0.0) / v.size();
    double mean_of_squares = std::inner_product(v.begin(), v.end(), v.begin(), 0.0) / v.size();
    double actual_stddev = std::sqrt(mean_of_squares - square(mean_of_values));
    printf("Expected mean and stddev: %g, %g\n", mean, stddev);
    printf("Actual mean and stddev: %g, %g\n", mean_of_values, actual_stddev);
//dex16
}
} // namespace ex16

namespace ex17 {
//ex17
template<class Values, class Weights, class Gen>
auto weighted_choice(const Values& v, const Weights& w, Gen& g)
{
    auto dist = std::discrete_distribution<int>(
        std::begin(w), std::end(w));
    int index = dist(g);
    return v[index];
}

void test() {
    auto g = std::mt19937(std::random_device{}());
    std::vector<std::string> choices =
        { "quick", "brown", "fox" };
    std::vector<int> weights = { 1, 7, 2 };
    std::string word = weighted_choice(choices, weights, g);
        // 7/10 of the time, we expect word=="brown".
}
//dex17
} // namespace ex17

namespace ex18 {
void test() {
//ex18
    int w[] = { 1, 0, 2, 1 };
    std::discrete_distribution<int> dist(w, w+4);
    std::vector<double> v = dist.probabilities();
    assert((v == std::vector{ 0.25, 0.0, 0.50, 0.25 }));
//dex18
}
} // namespace ex18

namespace ex19 {
//ex19
template<class Values, class Gen>
auto weighted_choice(
    const Values& v, const std::vector<int>& w,
    Gen& g)
{
    int sum = std::accumulate(w.begin(), w.end(), 0);
    int cutoff = std::uniform_int_distribution<int>(0, sum - 1)(g);
    auto vi = v.begin();
    auto wi = w.begin();
    while (cutoff > *wi) {
        cutoff -= *wi++;
        ++vi;
    }
    return *vi;
}
//dex19
void test() {
    std::vector<std::string> choices =
        { "quick", "brown", "fox" };
    std::vector<int> weights = { 1, 7, 2 };
    std::mt19937 g;
    std::string word = weighted_choice(choices, weights, g);
        // 7/10 of the time, we expect word=="brown".
}
} // namespace ex19

namespace ex20 {
void test() {
//ex20
    std::vector<int> deck(52);
    std::iota(deck.begin(), deck.end(), 1);
        // deck now contains ints from 1 to 52.

    std::mt19937 g(std::random_device{}());
    std::shuffle(deck.begin(), deck.end(), g);
        // The deck is now randomly shuffled.
//dex20
//ex21
    std::random_device rd;
    std::shuffle(deck.begin(), deck.end(), rd);
        // The deck is now TRULY randomly shuffled.
//dex21
}
} // namespace ex20

int main()
{
    ex1::test();
    ex2::test();
    ex3::test_heads();
    ex4::test();
    ex5::test();
    ex6::test();
    ex8::test();
    ex9::test();
    ex11::test();
    ex13::test();
    ex16::test();
    ex18::test();
    ex19::test();
    ex20::test();
}
