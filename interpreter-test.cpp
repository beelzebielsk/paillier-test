#include <interpreter/parser.h>
#include <iostream>
#include <sstream>
#include <vector>

#include <unordered_map>

using std::stringstream;
using std::cout;
using std::endl;
using std::cerr;
using std::unordered_map;
using std::string;

int testTokenizer();
int testIsInstruction();
int testInstructionType(); // Not working with testResults.
int testIsDeclarationType();
int testDeclarationType(); // TODO
int testCreateInstructionList();
int testExecute(); // TODO

std::ostream& operator<<(std::ostream&, Instruction::Type);

template<typename Case, typename Result>
int testResults(unordered_map<Case, Result> cases, auto test) {
    bool allCorrect = true;
    for (auto pair : cases) {
        Case thisCase = pair.first;
        Result expected = pair.second;
        Result actual = test(thisCase);
        if (actual != expected) {
            cerr << "Case " << thisCase << " has incorrect result."
                << endl
                << "Expected: " << expected << endl
                << "Result: " << actual << endl;
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
}

int testTokenizer() {
    stringstream file(R"(
        add one two
        mult three four
        load five
        output six
    )");

    vector<Token> expectedTokens = {
        "add", "one", "two",
        "mult", "three", "four",
        "load", "five",
        "output", "six",
    };

    vector<Token> tokens = tokenizeProgram(file);
    if (expectedTokens != tokens) {
        cout << "Tokenizer failed!" << endl;
        cout << "Tokens found:" << endl;
        for (auto token : tokens) { cout << token << endl; }
        cout << "Tokens expected:" << endl;
        for (auto token : expectedTokens) { cout << token << endl; }
        return false;
    }

    return true;

}

int testIsInstruction() {
    string add = "add";
    string mult = "mult";
    string load = "load";
    string output = "output";

    bool isAdd = Instruction::isInstructionName(add);
    bool isMult = Instruction::isInstructionName(mult);
    bool isLoad = Instruction::isInstructionName(load);
    bool isOutput = Instruction::isInstructionName(output);

    if (!isAdd) {
        cerr << "Add is NOT interpreted as instruction." << endl;
    }
    if (!isMult) {
        cerr << "Mult is NOT interpreted as instruction." << endl;
    }
    if (!isLoad) {
        cerr << "Load is NOT interpreted as instruction." << endl;
    }
    if (!isOutput) {
        cerr << "Output is NOT interpreted as instruction." << endl;
    }

    return isAdd && isMult && isLoad && isOutput;
}

int testInstructionType() {
    /*
    string add = "add";
    string mult = "mult";
    string load = "load";
    string output = "output";

    Instruction::Type typeAdd = Instruction::which(add);
    Instruction::Type typeMult = Instruction::which(mult);
    Instruction::Type typeLoad = Instruction::which(load);
    Instruction::Type typeOutput = Instruction::which(output);

    bool addCorrect = typeAdd == Instruction::add;
    bool multCorrect = typeMult == Instruction::mult;
    bool loadCorrect = typeLoad == Instruction::load;
    bool outputCorrect = typeOutput == Instruction::output;

    if (!addCorrect) {
        cerr << "add is interpreted incorrectly as " << typeAdd
            << endl;
    }
    if (!multCorrect) {
        cerr << "mult is interpreted incorrectly as " << typeMult
            << endl;
    }
    if (!loadCorrect) {
        cerr << "load is interpreted incorrectly as " << typeLoad
            << endl;
    }
    if (!outputCorrect) {
        cerr << "output is interpreted incorrectly as " << typeOutput
            << endl;
    }

    return addCorrect && multCorrect && loadCorrect && outputCorrect;
    */

    unordered_map<string, Instruction::Type> cases = {
        {"add", Instruction::add},
        {"mult", Instruction::mult},
        {"load", Instruction::load},
        {"output", Instruction::output}
    };

    auto test = [](string Case) {
        return Instruction::which(Case);
    };

    return testResults(cases, test);
    //Instruction::Type type = Instruction::add;
    //return true;
}

int testIsDeclarationType() {
    unordered_map<string, bool> cases = {
        {"input", true},
        {"share", true}
    };

    auto test = [](string type) { 
        return Declaration::isDeclarationType(type);
    };

    return testResults(cases, test);
}

int testDeclarationType() {
}

int testCreateInstructionList() {
}

int testExecute() {
}

std::ostream& operator<<(std::ostream& out, Instruction::Type type) {
    if (type == Instruction::add) {
        out << "Instruction::add";
    } else if (type == Instruction::mult) {
        out << "Instruction::mult";
    } else if (type == Instruction::load) {
        out << "Instruction::load";
    } else if (type == Instruction::output) {
        out << "Instruction::output";
    }

    return out;
}
