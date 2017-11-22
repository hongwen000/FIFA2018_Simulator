#ifndef RANDLIB_H
#define RANDLIB_H
#include <random>
#include <utility>

class RandLib {
public:
    static int binomial_rand();
    static double normal_rand();
    static int uniform_rand();
private:
    static std::unique_ptr<std::default_random_engine> engine;
    static std::unique_ptr<std::uniform_int_distribution<int> > u;
    static std::unique_ptr<std::binomial_distribution<int> > b;
    static std::unique_ptr<std::normal_distribution<double> > n;
};

#endif // RANDLIB_H
