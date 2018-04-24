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

#include "tests.h"
    using paillierTest::pair_list;
    using paillierTest::testResults;

using std::string;

int testTokenizer();
int testIsInstruction();
int testInstructionType();
int testIsDeclarationType();
int testDeclarationType();
int testCreateInstructionList();
int testExecute();

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
        },
        {
            {
                R"(
                    add dest five ten
                    mult dest dest ten
                    mult dest dest ten
                    mult dest dest ten
                )",
                { {"five", 5}, {"ten", 10} }
            },
            { {"dest", 15000}, {"five", 5}, {"ten", 10} }
        },
    };

    auto test = [](pair<string, ProgramState> program) {
        stringstream s(program.first);
        vector<Token> t = tokenizeProgram(s);
        vector<Instruction> i = createInstructionList(t);
        return execute(i, program.second);
    };

    return testResults(cases, test);
}
