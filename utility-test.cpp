#include <iostream>
    using std::cout;
    using std::endl;
    using std::cerr;
#include <vector>
    using std::vector;
#include <utility>
    using std::pair;
#include <NTL/ZZ.h>
    using NTL::ZZ;
#include "tests.h"
    using paillierTest::pair_list;
    using paillierTest::testResults;

#include <paillier/utility.h>

#define TEST(testFunction) \
    cout << "Testing " << #testFunction << endl; \
    if (!( test ## testFunction() )) {\
        cerr << #testFunction << " test failed." << endl;\
    }

int testZZToBits();

int main()
{
    TEST(ZZToBits)
    return 0;
}

int testZZToBits() {
    auto makeTest = []() {
        // - A test case is really just the same number, over and over
        //   again.
        ZZ rand = NTL::RandomBnd((ZZ)1000);
        return std::pair<ZZ, ZZ>{rand, rand};
    };
    pair_list<ZZ, ZZ> cases;
    for (int i = 0; i < 10; i++) cases.push_back(makeTest());

    auto test = [](ZZ number) {
        vector<bool> bits = ZZToBits(number);
        ZZ value{0};
        for (int i = bits.size() - 1; i >= 0; i--) {
            bool bit = bits[i];
            value = value * 2 + (ZZ)(bit ? 1 : 0);
        }
        return value;
    };

    testResults(cases, test);
}
