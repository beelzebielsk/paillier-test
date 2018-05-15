/* Tests:
 * - Test if the chance of getting a special point really is what the
 *   user asked for. Do a few thousand trials or so, figure out the
 *   empircal chance of a special point, calculate the difference
 *   between that and the stated chance, delta.
 * - Test out encryption of some small numbers.
 * - Test out decryption of some small numbers.
 * - Test out round-trip encryption/decryption of various numbers.
 * - Test out DDLOG: see if the shares actually fulfill their roles
 *   such that their sum equals some number.
 *   - Sample a special point.
 *   - Look for the next and previous special points (next in terms of
 *     sucessive multiplications of g).
 *   - Take a point somewhere nearby one of the special points (maybe
 *     even directly on the special point). Call this the test point.
 *   - Create shares out of the exponent of this test point by
 *     creating two numbers that sum to the exponent.
 *   - If the two numbers are both before the test point then doing
 *     ddlog on both numbers should yield shares that sum up to the
 *     test point's exponent.
 *   - If the two numbers are both after the test point, then doing
 *     ddlog on both numbers should yield shares that sum up to the
 *     exponent of the special point after the test point.
 *   - If they're on opposite sides, then it shouldn't work.
 */

#include <iostream>
    using std::cout;
    using std::endl;
    using std::cerr;
#include <sstream>
    using std::stringstream;
#include <vector>
    using std::vector;
#include <unordered_map>
    using std::unordered_map;
#include <utility>
    using std::pair;
#include <tuple>
    using std::tuple;
#include <NTL/ZZ.h>
    using NTL::ZZ;
#include <stdexcept>
    using std::domain_error;

#include "tests.h"
    using paillierTest::pair_list;
    using paillierTest::testResults;

#include <paillier/paillier.h>
#include <paillier/utility.h>

ZZ lcm(ZZ x, ZZ y){
  ZZ ans = (x * y) / NTL::GCD(x,y);
  return ans;
}

using std::string;

int testSamplePointProbabilities(); // TODO
int testEncryption(); // TODO
int testDecryption(); // TODO
int testEncryptionDecryption();
int testDDLOG(); // TODO
int testEncryptBits();
ZZ strToZZ(string num);

#define TEST(testFunction) \
    cout << "Testing " << #testFunction << endl; \
    if (!( test ## testFunction() )) {\
        cerr << #testFunction << " test failed." << endl;\
    }

int main() {
    TEST(EncryptionDecryption)
    TEST(EncryptBits)

    return 0;
}

int testEncryptionDecryption() {
    pair_list<ZZ, ZZ> cases {
        {(ZZ)0, (ZZ)0},
        {(ZZ)1, (ZZ)1},
        {(ZZ)10, (ZZ)10},
        {(ZZ)20, (ZZ)20},
        {(ZZ)30, (ZZ)30},
        {(ZZ)40, (ZZ)40},
        {(ZZ)50, (ZZ)50},
        {(ZZ)60, (ZZ)60},
        {(ZZ)70, (ZZ)70},
        {(ZZ)80, (ZZ)80},
        {(ZZ)90, (ZZ)90},
    };
    Paillier paillier{12};

    auto makeTest = [&]() {
        ZZ message = NTL::RandomBnd(paillier.getModulus());
        return std::pair<ZZ, ZZ>{message, message};
    };

    for (int i = 0; i < 100; i++) cases.push_back(makeTest());

    auto test = [&](ZZ message) {
        ZZ cipher = paillier.encrypt(message);
        ZZ plain = paillier.decrypt(cipher);
        return plain;
    };

    return testResults(cases, test);
    //return 1;
}

int testEncryptBits() {
    using Case = std::vector<ZZ>;
    using Result = vector<ZZ>;
    auto makeTest = [](bool placeMessage = false) {
        Paillier p{512};
        ZZ modulus = p.getModulus();
        ZZ lambda = p.getLambda();
        vector<ZZ> bits;

        if (placeMessage) {
            ZZ message = NTL::RandomBnd(modulus);
            for (bool bit : ZZToBits(lambda)) {
                bits.push_back((ZZ)bit * message);
            }
            return std::pair<Case, Result>{
                {modulus, lambda, message}, bits
            };
        } else {
            for (bool bit : ZZToBits(lambda)) {
                bits.push_back((ZZ)bit);
            }
            return std::pair<Case, Result>{
                {modulus, lambda}, bits
            };
        }
    };

    pair_list<Case, Result> cases;
    for (int i = 0; i < 100; i++) cases.push_back(makeTest());
    for (int i = 0; i < 100; i++) cases.push_back(makeTest(true));

    auto test = [](Case keys) {
        ZZ modulus = keys[0];
        ZZ lambda = keys[1];
        vector<ZZ> bits;
        Paillier p{modulus, lambda};
        if (keys.size() == 3) {
            bits = p.encryptBits(keys[2]);
        } else {
            bits = p.encryptBits();
        }
        vector<ZZ> decrypted;
        for (ZZ bit : bits) {
            decrypted.push_back(p.decrypt(bit));
        }
        return decrypted;
    };
    return testResults(cases, test);
}

int testEncryption() {
    struct Case {
        NTL::ZZ p, q, r, m;
        Case(NTL::ZZ p, NTL::ZZ q, NTL::ZZ r, NTL::ZZ m) 
            : p(p), q(q), r(r), m(m) {}
    };

    vector<ZZ> primes = {
        (ZZ)19,
        (ZZ)31,
        (ZZ)31,
        (ZZ)17,
        (ZZ)29,
        (ZZ)23,
        (ZZ)29,
        (ZZ)19,
        (ZZ)29,
        (ZZ)31,
        (ZZ)17,
        (ZZ)23,
        (ZZ)41,
        (ZZ)37,
        (ZZ)43,
        (ZZ)59,
        (ZZ)37,
        (ZZ)47,
        (ZZ)41,
        (ZZ)59,
        (ZZ)59,
        (ZZ)47,
        (ZZ)37,
        (ZZ)61,
        (ZZ)61,
        (ZZ)47,
        (ZZ)37,
        (ZZ)43,
    };

    auto makeCaseFrom = [](vector<ZZ> list, int i) {
        return Case(list[i], list[i+1], list[i+2], list[i+3]);
    };


    /* XXX
    pair_list<Case, ZZ> cases {
        { makeCaseFrom(primes, 0), 0},
        { makeCaseFrom(primes, 4), 0},
        { makeCaseFrom(primes, 8), 0},
        { makeCaseFrom(primes, 12), 0},
        { makeCaseFrom(primes, 16), 0},
        { makeCaseFrom(primes, 20), 0},
        { makeCaseFrom(primes, 24), 0},
    };
    */
}

ZZ strToZZ(string num) {
    // Value starts at 0.
    ZZ ret;
    for (char c : num) {
        ret = ret * 10 + (ZZ)(c - '0');
    }
    return ret;
}
