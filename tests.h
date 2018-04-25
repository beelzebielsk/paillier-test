#ifndef TESTS_H
#define TESTS_H

namespace paillierTest {

#include <iostream>
    using std::endl;
    using std::cerr;
#include <vector>
    using std::vector;
#include <unordered_map>
    using std::unordered_map;
#include <utility>
    using std::pair;
#include <exception>

using std::string;

    template <typename T1, typename T2>
    using pair_list = vector<pair<T1, T2>>;

    template<typename Case, typename Result>
    int testResults(pair_list<Case, Result> cases, auto test) {
        bool allCorrect = true;
        for (auto pair : cases) {
            Case thisCase = pair.first;
            Result expected = pair.second;
            Result actual;
            try {
                actual = test(thisCase);
                if (actual != expected) {
                    cerr << "Case " << thisCase 
                        << " has incorrect result." << endl
                        << "Expected: " << expected << endl
                        << "Result: " << actual << endl;
                    allCorrect = false;
                }
            }
            catch(std::exception& error) {
                cerr << error.what() << endl;
                allCorrect = false;
            }
        }
        return allCorrect;
    }
}

template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& out, pair<T1, T2> p) {
    out << "("
        << "'" << p.first << "', '" << p.second << "')";
    return out;
}

template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& out,
        unordered_map<T1, T2> map) {
    out << "{";
    for (auto pair : map) {
        out << "'" << pair.first << "' : '" << pair.second << "', ";
    }
    out << "}";
    return out;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, vector<T> ts) {
    out << "(";
    for (T t : ts) {
        out << t << ", " ;
    }
    out << ")";
    return out;
}

#endif
