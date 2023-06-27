#ifndef __RANDOM_UTILS__
#define __RANDOM_UTILS__

#include <random>

class Random {
    public:
        static double Double(double min = 0.0, double max = 1.0) {
            std::uniform_real_distribution<double> distribution(min, max);
            std::mt19937 generator((unsigned int)time(nullptr));
            return distribution(generator);
        }

        static int Int(int min = 0, int max = 255) {
            std::uniform_int_distribution<int> distribution(min, max);
            std::mt19937 generator((unsigned int)time(nullptr));
            return distribution(generator);
        }
};

#endif // __RANDOM_UTILS__
