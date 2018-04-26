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
 * - Test out the full multiplication procedure: multiply an encrypted
 *   number by some other number which has been split into shares,
 *   perform the multiplication, and see if the two resulting shares
 *   sum up to the multiplied number. We can do this either by
 *   - Pre-specifying the shares of the number and the input to
 *     multiply with.
 *   - Specifying the numbers to multiply, then making one number the
 *     share, the other the input, creating the shares, encrypting the
 *     input, performing the multiplication, then doing DDLOG on the
 *     shares, then summing the result of the multiplication, then
 *     decrypting it, and comparing that to the expected result.
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

#include "tests.h"
    using paillierTest::pair_list;
    using paillierTest::testResults;

#include <paillier/paillier.h>


#include <NTL/ZZ.h>
    using NTL::ZZ;

int testSamplePointProbabilities(); // TODO
int testEncryption(); // TODO
int testDecryption(); // TODO
int testEncryptionDecryption(); // TODO
int testDDLOG(); // TODO
int testMultiplyNumbers(); // TODO

#define TEST(testFunction) \
    cout << "Testing " << #testFunction << endl; \
    if (!( test ## testFunction() )) {\
        cerr << #testFunction << " test failed." << endl;\
    }

int main() {
    TEST(EncryptionDecryption)

    return 0;
}

int testEncryptionDecryption() {
    pair_list<ZZ, ZZ> cases {
        {(ZZ)10, (ZZ)10},
        //{(ZZ)20, (ZZ)20},
        //{(ZZ)30, (ZZ)30},
        //{(ZZ)40, (ZZ)40},
        //{(ZZ)50, (ZZ)50},
        //{(ZZ)60, (ZZ)60},
        //{(ZZ)70, (ZZ)70},
        //{(ZZ)80, (ZZ)80},
        //{(ZZ)90, (ZZ)90},
    };

    auto test = [](ZZ message) {
        cout << "Start test" << endl;
        Paillier paillier{};
        cout << "Construct paillier object" << endl;
        ZZ cipher = paillier.encrypt(message);
        ZZ plain = paillier.decrypt(cipher);
        return plain;
    };

    return testResults(cases, test);
    //return 1;
}
