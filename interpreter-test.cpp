#include <interpreter/parser.h>
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

using std::string;

template <typename T1, typename T2>
using pair_list = vector<pair<T1, T2>>;

int testTokenizer();
int testIsInstruction();
int testInstructionType();
int testIsDeclarationType();
int testDeclarationType();
int testCreateInstructionList(); // TODO
int testExecute(); // TODO

std::ostream& operator<<(std::ostream&, Instruction::Type);

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
        catch(InterpreterError& error) {
            cout << error << endl;
            allCorrect = false;
        }
    }
    return allCorrect;
}

#define TEST(testFunction) \
    cout << "Testing " << #testFunction << endl; \
    if (!( test ## testFunction() )) {\
        cerr << #testFunction << " test failed." << endl;\
    }
    

int main() {
    TEST(Tokenizer)
    TEST(IsInstruction)
    TEST(InstructionType)
    TEST(IsDeclarationType)
    TEST(DeclarationType)
    TEST(CreateInstructionList)
    TEST(Execute)
}

int testTokenizer() {
    pair_list<string, vector<Token>> cases = {
        {
            R"(
                add one two
                mult three four
                load five
                output six
            )",
            vector<Token> {
                "add", "one", "two",
                "mult", "three", "four",
                "load", "five",
                "output", "six"
            }
        }
    };

    auto test = [](string program) {
        // So that s can be passed by reference.
        stringstream s = stringstream(program);
        return tokenizeProgram(s);
    };

    return testResults(cases, test);
}

int testIsInstruction() {
    pair_list<string, bool> cases = {
        {"add", true},
        {"mult", true},
        {"load", true},
        {"output", true}
    };

    auto test = [](string name) { 
        return Instruction::isInstructionName(name);
    };

    return testResults(cases, test);
}

int testInstructionType() {
    pair_list<string, Instruction::Type> cases = {
        {"add", Instruction::Type::add},
        {"mult", Instruction::Type::mult},
        {"load", Instruction::Type::load},
        {"output", Instruction::Type::output}
    };

    auto test = [](string Case) {
        return Instruction::which(Case);
    };

    return testResults(cases, test);
}

int testIsDeclarationType() {
    pair_list<string, bool> cases = {
        {"input", true},
        {"share", true}
    };

    auto test = [](string type) { 
        return Declaration::isDeclarationType(type);
    };

    return testResults(cases, test);
}

int testDeclarationType() {
    pair_list<string, Declaration::Type> cases = {
        {"input", Declaration::Type::input},
        {"share", Declaration::Type::share}
    };

    auto test = [](string type) { 
        return Declaration::which(type);
    };

    return testResults(cases, test);
}

int testCreateInstructionList() {
    // /*
    pair_list<string, vector<Instruction>> cases = {
        {
            R"(
                add one two three
                mult three four five
                load six seven
                output eight nine
            )",
            {
                Instruction("add", "one", "two", "three"),
                Instruction("mult", "three", "four", "five"),
                Instruction("load", "six", "seven"),
                Instruction("output", "eight", "nine")
            }
        }
    };

    auto test = [](string program) { 
        stringstream s = stringstream(program);
        return createInstructionList(tokenizeProgram(s));
    };

    return testResults(cases, test);
}

int testExecute() {
    pair_list< pair<string, ProgramState>, ProgramState > cases = {
        {
            {
                "add dest two three",
                { {"two", 2}, {"three", 3} }
            },
            { {"dest", 5}, {"two", 2}, {"three", 3} }
        }
    };

    auto test = [](pair<string, ProgramState> program) {
        stringstream s(program.first);
        vector<Token> t = tokenizeProgram(s);
        vector<Instruction> i = createInstructionList(t);
        return execute(i, program.second);
    };

    return testResults(cases, test);
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

template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& out, pair<T1, T2> p) {
    out << "("
        << "'" << p.first << "', '" << p.second << "')";
    return out;
}
