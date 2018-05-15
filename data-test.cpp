#include "./tests.h"
    using paillierTest::pair_list;
    using paillierTest::testResults;
#include <iostream>
    using std::cout;
    using std::endl;
    using std::cerr;
#include <sstream>
    using std::stringstream;
#include <typeinfo>
    using std::bad_cast;
#include <tuple>
    using std::make_tuple;
#include <stdexcept>
    using std::domain_error;
    using std::logic_error;
using std::string;

#include <NTL/ZZ.h>
    using NTL::ZZ;
#include <paillier/paillier.h>
#include <paillier/data.h>
#include <paillier/ddlog.h>

#define TEST(testFunction) \
    cout << "Testing " << #testFunction << endl; \
    if (!( test ## testFunction() )) {\
        cerr << #testFunction << " test failed." << endl;\
    }

// n = 3 * 5
ZZ testN = ZZ(15);
//testLambda = lcm(2, 4) = 4
ZZ testLambda = ZZ(4);
Paillier testPaillier{testN, testLambda};

// Test runtime casting based on Value.type.
int testValue();
int testAddInputs();
int testNegateInput();
int testMultEncryptionMemory(); // TODO
int testMultInputMemory(); // TODO
int testValueOperations(); // TODO
int testShare();
//int testAddMemories(); // TODO
ZZ lcm(ZZ x, ZZ y){
  ZZ ans = (x * y) / NTL::GCD(x,y);
  return ans;
}

std::ostream& operator<<(std::ostream& o, Value::Type t);
std::ostream& operator<<(std::ostream& o, Input i);
std::ostream& operator<<(std::ostream& o, Memory m);
std::ostream& operator<<(std::ostream& o, Value& v) {
    if (v.type == Value::Type::Input) {
        return o << dynamic_cast<Input&>(v);
    } else if (v.type == Value::Type::Memory) {
        return o << dynamic_cast<Memory&>(v);
    }
}

/* Tests if two inputs are equal by decrypting all of their parts and
 * checking to see if the decryptions are the same.
 */
bool inputsEqual(Input, Input, Paillier);


int main()
{
    TEST(Value)
    TEST(AddInputs)
    TEST(NegateInput)
    TEST(Share)
    TEST(MultEncryptionMemory)
}

int testValue() {

    ZZ number{0};
    vector<ZZ> bits;
    // These are deliberately incorrect. The values don't matter.
    Input input{number, bits, number};
    Memory location1{number, number};
    Memory location2{number, number};

    pair_list<Value&, string> cases {
        {input, "cast to input"},
        {location1, "cast to memory"},
    };

    auto test = [](const Value& v) {
        if (v.type == Value::Type::Input) {
            try {
                dynamic_cast<const Input&>(v);
                return "cast to input";
            } catch (const bad_cast& e) {
                return "Did not cast to input";
            }
        } else if (v.type == Value::Type::Memory) {
            try {
                dynamic_cast<const Memory&>(v);
                return "cast to memory";
            } catch (const bad_cast& e) {
                return "Did not cast to memory";
            }
        }
    };

    return testResults(cases, test);
}

int testAddInputs() {
    using Case = std::pair<ZZ, ZZ>;
    using Result = ZZ;

    Paillier p{10};
    // This is the modulus, because the powers of the generator wrap
    // around modulo n. The order <g> is n (n = modulus).
    ZZ modulus = p.getModulus();
    auto makeTest = [&](ZZ value1, ZZ value2) {
        return std::pair<Case, Result> {
            {value1, value2}, (value1 + value2) % modulus
        };
    };

    pair_list<Case, Result> cases;
    for (int i = 0; i < 10; i++) {
        ZZ rand1 = NTL::RandomBnd(p.getModulus());
        ZZ rand2 = NTL::RandomBnd(p.getModulus());
        cases.push_back(makeTest(rand1, rand2));
    }

    auto test = [&](Case c) {
        ZZ v1 = c.first;
        ZZ v2 = c.second;
        Input i1{p.encrypt(v1), p.encryptBits(v1), p.getModulus()};
        Input i2{p.encrypt(v2), p.encryptBits(v2), p.getModulus()};
        Input sum = i1 + i2;
        return p.decrypt(sum.value);
    };

    return testResults(cases, test);
}

int testAddMemories() {
    using Case = std::pair<ZZ, ZZ>;
    using Result = ZZ;
}

int testNegateInput() {
    using Case = Input;
    using Result = std::tuple<ZZ, vector<ZZ>, ZZ>;
    Paillier p{20};

    auto makeTest = [&]() {
        ZZ rand = NTL::RandomBnd(p.getModulus());
        ZZ mod = p.getModulus();
        Input i{p.encrypt(rand), p.encryptBits(rand), p.getModulus()};
        vector<ZZ> bits;
        for (ZZ bit : p.encryptBits(-rand)) {
            bits.push_back(p.decrypt(bit));
        }
        Result result = make_tuple(-rand % mod, bits, mod);
        return std::pair<Case, Result>{i, result};
    };

    pair_list<Case, Result> cases;
    for (int i = 0; i < 10; i++) cases.push_back(makeTest());

    auto test = [&](Case c) {
        Input negated = -c;
        ZZ value = p.decrypt(negated.value);
        ZZ modulus = negated.modulus;
        vector<ZZ> bits;
        for (ZZ bit : negated.bits) {
            bits.push_back(p.decrypt(bit));
        }
        return make_tuple(value, bits, modulus);
    };

    return testResults(cases, test);
}

int testMultEncryptionMemory() {
    using Case = std::pair<ZZ, ZZ>;
    using Result = ZZ;
    Paillier p{20};

    auto makeTest = [&](ZZ threshold) {
        ZZ share = NTL::RandomBnd(threshold);
        ZZ input = NTL::RandomBnd(threshold);
        return std::pair<Case, Result>{
            {share, input}, share * input
        };
    };

    pair_list<Case, Result> cases;
    for (int i = 0; i < 10000; i++) cases.push_back(makeTest((ZZ)2));

    auto test = [&](Case c) {
        ZZ toShare = c.first;
        ZZ toInput = c.second;
        ZZ lambda = p.getLambda();
        ZZ modulus = p.getModulus();
        ZZ cipherModulus = modulus * modulus;
        ZZ generator = modulus + 1;

        std::pair<ZZ, ZZ> values = share(toShare, modulus);
        std::pair<ZZ, ZZ> secrets = share(toShare * lambda, modulus);
        Memory mem1{values.first, secrets.first};
        Memory mem2{values.second, secrets.second};
        ZZ encryption = p.encrypt(toInput);

        cout << "Information about test case:" << endl;
        cout << "\ttoShare: " << toShare << endl;
        cout << "\ttoInput: " << toInput << endl;
        cout << "\tlambda: " << lambda << endl;
        cout << "\tgenerator: " << generator << endl;
        cout << "\tmodulus: " << modulus << endl;
        cout << "\tcipherModulus: " << cipherModulus << endl;

        cout << "Information about shares and encrypted number:" << endl;
        cout << "\tvalues: " << values << endl;
        cout << "\tsecrets: " << secrets << endl;
        cout << "\tSum values: " << values.first + values.second << endl;
        cout << "\tSum secrets: " << secrets.first + secrets.second 
            << endl;
        cout << "\tdecryption Works? "
            << (p.decrypt(encryption) == toInput)
            << endl;


        // First test: The different "multiplicative shares" of each
        // number should be "toShare * toInput" g-steps away from each
        // other. So the following should always hold true:
        // multShare1 = g ** {toShare * toInput} * multShare2 ** (-1)

        ZZ multShare1 = NTL::PowerMod(
                encryption, mem1.secret, cipherModulus);
        ZZ multShare2 = NTL::PowerMod(
                encryption, mem2.secret, cipherModulus);
        ZZ productShares = (multShare1 * multShare2) % cipherModulus;
        ZZ alternateProductShares = NTL::PowerMod(
                encryption, mem1.secret + mem2.secret, cipherModulus);
        ZZ steps = NTL::PowerMod(
                generator, toShare * toInput, cipherModulus);

        cout << "Important Computations: " << endl;
        cout << "\tencrypted value ** lambda: " << 
            NTL::PowerMod(encryption, lambda, cipherModulus) << endl;
        cout << "\tencrypted value ** (lambda ** 2): " << 
            NTL::PowerMod(encryption, NTL::power(lambda, 2),
                          cipherModulus) 
            << endl;
        cout << "\tencrypted value ** (lambda ** 3): " << 
            NTL::PowerMod(encryption, NTL::power(lambda, 3),
                          cipherModulus) 
            << endl;
        cout << "\tgenerator ** (lambda - 1): " << 
            NTL::PowerMod(generator, lambda - 1, cipherModulus) 
            << endl;
        cout << "\tgenerator ** (toInput * toShare): " << steps << endl;
        cout << "\tmultShare1: " << multShare1 << endl;
        cout << "\tmultShare2: " << multShare2 << endl;
        cout << "\tmultShare1 * multShare2: " << productShares << endl;
        cout << "\tgenerator ** (secret1 + secret2): " 
            << alternateProductShares << endl;

        if (productShares != steps) {
            throw logic_error("Invariant broken. The two multiplicative shares are not multiplicative shares of generator to product of (toInput * toShare).");
        }
        if (multShare1 != ((steps * NTL::InvMod(multShare2, cipherModulus)) % cipherModulus)) {
            throw logic_error("Invariant broken. The two multiplicative shares are not the correct number of steps apart.");
        }

        // Check to see if the procedure messes up because both
        // multiplicative shares are on opposite sides of a special
        // point.
        ZZ specialPoint1 = multShare1;
        ZZ specialPoint2 = NTL::InvMod(multShare2, cipherModulus);
        while (!DDLog::isSpecialPoint(specialPoint1))
            NTL::MulMod(specialPoint1, specialPoint1, generator,
                    cipherModulus);
        while (!DDLog::isSpecialPoint(specialPoint2))
            NTL::MulMod(specialPoint2, specialPoint2, generator,
                    cipherModulus);

        if (specialPoint1 != specialPoint2) {
            cerr << "Special points not same, failure expected." << endl;
        }

        ZZ newShare1 = multMemoryEncryption(
                encryption, mem1, modulus, false);
        ZZ newShare2 = multMemoryEncryption(
                // Why did this work? When one number was 0?
                // encryption, mem1, modulus, true);
                encryption, mem2, modulus, true);
        return newShare1 + newShare2;
    };

    return testResults(cases, test);
}

int testShare() {
    using Case = std::pair<ZZ, ZZ>;
    using Result = ZZ;
    auto makeTest = [](ZZ modulus) {
        ZZ rand = NTL::RandomBnd(NTL::power(modulus, 2));
        return std::pair<Case, Result>{{rand, modulus}, rand};
    };
    Paillier p{20};

    pair_list<Case, Result> cases;

    for (int i = 0; i < 10; i++) cases.push_back(makeTest(p.getModulus()));

    auto test = [](Case c) {
        ZZ number = c.first;
        ZZ modulus = c.second;
        ZZ maximum = NTL::power(modulus, 3);
        ZZ minimum = -maximum;
        std::pair<ZZ, ZZ> s = share(number, modulus);
        stringstream errorMessage;
        if (s.first < minimum) {
            errorMessage << "First share '"
                << s.first << "' "
                << "is too small. Less than '"
                << minimum << "'.";
            throw domain_error(errorMessage.str());
        } else if (s.first > maximum) {
            errorMessage << "First share '"
                << s.first << "' "
                << "is too large. More than '"
                << maximum << "'.";
                throw domain_error(errorMessage.str());
        } else if (s.second < minimum) {
            errorMessage << "Second share '"
                << s.second << "' "
                << "is too small. Less than '"
                << minimum << "'.";
            throw domain_error(errorMessage.str());
        } else if (s.second > maximum) {
            errorMessage << "First share '"
                << s.second << "' "
                << "is too large. More than '"
                << maximum << "'.";
            throw domain_error(errorMessage.str());
        }
        return s.first + s.second;
    };

    return testResults(cases, test);
}

std::ostream& operator<<(std::ostream& o, Value::Type t) {
    if (t == Value::Type::Input) {
        return o << "Value::Input";
    } else if (t == Value::Type::Memory) {
        return o << "Value::Memory";
    }
}
std::ostream& operator<<(std::ostream& o, Input i) {
    return o << "["
        << i.type << ", "
        << i.value
        << "]";
}
std::ostream& operator<<(std::ostream& o, Memory m) {
    return o << "["
        << m.type << ", "
        << m.value
        << "]";
}
template <typename T1, typename T2, typename T3>
std::ostream& operator<<(std::ostream& o, std::tuple<T1, T2, T3> t) {
    return o << "("
        << std::get<0>(t) << ","
        << std::get<1>(t) << ","
        << std::get<2>(t) << ")";
}

bool inputsEqual(Input left, Input right, Paillier p) {
    // Encrypted with different keys.
    if (left.bits.size() != right.bits.size())
        return false;
    if (p.decrypt(left.value) != p.decrypt(right.value))
        return false;
    for (int i = 0; i < left.bits.size(); i++) {
        if (p.decrypt(left.bits[i]) != p.decrypt(right.bits[i]))
            return false;
    }
    return true;
}


/* A case that failed.
        toShare: 1
        toInput: 1
        lambda: 370525207536
        generator: 627048
        modulus: 627047
        cipherModulus: 393187940209
Information about shares and encrypted number:
        values: ('132201574553291580', '-132201574553291579')
        secrets: ('-111604440578952239', '111604811104159775')
        Sum values: 1
        Sum secrets: 370525207536
        decryption Works? 1
Important Computations:
        encrypted value ** lambda: 627048
        encrypted value ** (lambda ** 2): 627048
        encrypted value ** (lambda ** 3): 627048
        generator ** (lambda - 1): 1
        generator ** (toInput * toShare): 627048
        multShare1: 157849218035
        multShare2: 133560950653
        multShare1 * multShare2: 627048
        generator ** (secret1 + secret2): 627048
 */
