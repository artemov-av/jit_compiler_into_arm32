#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <sys/mman.h>
#include "jit_compiler/symbol.h"
#include "jit_compiler/tokens/tokens.h"
#include "jit_compiler/arm32_instructions_encoder.h"
#include "jit_compiler/tokenizer.h"
#include "jit_compiler/translator_into_rpn.h"
#include "jit_compiler/jit_compiler_into_arm32.h"
#include "gtest/gtest.h"

// available functions to be used within JIT-compiled code
static int my_div(int a, int b) { return a / b; }

static int my_mod(int a, int b) { return a % b; }

static int my_inc(int a) { return ++a; }

static int my_dec(int a) { return --a; }

static int my_sum(int a, int b) { return a + b; }

////////////////////////////////////////////////////////////////////////////////

//Token tests

TEST(RPNTokensTest, MinusRPNTokenTest) {
    MinusRPNToken token;
    EXPECT_EQ(token.GetTypeOfToken(), TokenType::OP_MINUS);
}

TEST(RPNTokensTest, PlusRPNTokenTest) {
    PlusRPNToken token;
    EXPECT_EQ(token.GetTypeOfToken(), TokenType::OP_PLUS);
}

TEST(RPNTokensTest, MultRPNTokenTest) {
    MultRPNToken token;
    EXPECT_EQ(token.GetTypeOfToken(), TokenType::OP_MULT);
}

TEST(RPNTokensTest, LiteralRPNTokenTest) {
    LiteralRPNToken token1(5);
    EXPECT_EQ(token1.GetTypeOfToken(), TokenType::LITERAL);
    EXPECT_EQ(token1.GetValue(), 5);
    LiteralRPNToken token2(-1);
    EXPECT_EQ(token2.GetValue(), -1);
    LiteralRPNToken token3(std::numeric_limits<int>::max());
    EXPECT_EQ(token3.GetValue(), std::numeric_limits<int>::max());
}

TEST(RPNTokensTest, VariableRPNTokenTest) {
    int x;
    auto address = reinterpret_cast<void *>(&x);
    VariableRPNToken token(address);
    EXPECT_EQ(token.GetTypeOfToken(), TokenType::VARIABLE);
    EXPECT_EQ(token.GetAddress(), address);
}

TEST(RPNTokensTest, FunctionRPNTokenTest) {
    auto address = reinterpret_cast<void *>(&my_div);
    FunctionRPNToken token(reinterpret_cast<void *>(&my_div), 2);
    EXPECT_EQ(token.GetTypeOfToken(), TokenType::FUNCTION);
    EXPECT_EQ(token.GetAddress(), address);
    EXPECT_EQ(token.GetArgsCount(), 2);
}


////////////////////////////////////////////////////////////////////////////////

//Tokenizer tests

TEST(TokenizerTest, TestWithOperatorsLiteralsAndVariables) {
    std::string expr = "1+23*a";
    std::vector<std::string> ans = {"1", "+", "23", "*", "a"};
    Tokenizer tokenizer;
    auto result = tokenizer(expr);
    ASSERT_EQ(ans.size(), result.size());
    for (int i = 0; i < ans.size(); ++i) {
        EXPECT_EQ(ans[i], result[i]);
    }
}

TEST(TokenizerTest, TestWithFunctionsBracketsAndCommas) {
    std::string expr = "(a*bc)+func(5,g())";
    std::vector<std::string> ans = {"(", "a", "*", "bc", ")", "+",
                                    "func", "(", "5", ",", "g", "(", ")", ")"};
    Tokenizer tokenizer;
    auto result = tokenizer(expr);
    ASSERT_EQ(ans.size(), result.size());
    for (int i = 0; i < ans.size(); ++i) {
        EXPECT_EQ(ans[i], result[i]);
    }
}

TEST(TokenizerTest, TestWithSpaces) {
    std::string expr = "   1  - x* yz  +   (5  -6) - 8 ";
    std::vector<std::string> ans = {"1", "-", "x", "*", "yz", "+", "(", "5",
                                    "-", "6", ")", "-", "8"};
    Tokenizer tokenizer;
    auto result = tokenizer(expr);
    ASSERT_EQ(ans.size(), result.size());
    for (int i = 0; i < ans.size(); ++i) {
        EXPECT_EQ(ans[i], result[i]);
    }
}

TEST(TokenizerTest, TestWithNegativeLiterals) {
    std::string expr = "( -5  +8) -  4 * -6";
    std::vector<std::string> ans = {"(", "-5", "+", "8", ")", "-", "4", "*", "-6"};
    Tokenizer tokenizer;
    auto result = tokenizer(expr);
    ASSERT_EQ(ans.size(), result.size());
    for (int i = 0; i < ans.size(); ++i) {
        EXPECT_EQ(ans[i], result[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////

//JIT compiler tests

static const int CODE_SIZE = 4096;

static void *InitProgramCodeBuffer() {
    void *result = mmap(0,
                        CODE_SIZE,
                        PROT_READ | PROT_WRITE | PROT_EXEC,
                        MAP_PRIVATE | MAP_ANONYMOUS,
                        0,
                        0);
    if (!result) {
        perror("Can't mmap: ");
        exit(2);
    }
    return result;
}

static void FreeProgramCodeBuffer(void *addr) {
    munmap(addr, CODE_SIZE);
}

static int CallFunction(void *addr) {
    typedef int (*jited_function_t)();
    jited_function_t function = reinterpret_cast<int (*)()>(addr);
    return function();
}

static void InitSymbolsWithFunctions(std::vector<Symbol> &symbols) {
    Symbol symbol;
    symbol.pointer = reinterpret_cast<void *>(&my_div);
    symbol.name = "div";
    symbols.push_back(symbol);
    symbol.pointer = reinterpret_cast<void *>(&my_sum);
    symbol.name = "sum";
    symbols.push_back(symbol);
    symbol.pointer = reinterpret_cast<void *>(&my_mod);
    symbol.name = "mod";
    symbols.push_back(symbol);
    symbol.pointer = reinterpret_cast<void *>(&my_inc);
    symbol.name = "inc";
    symbols.push_back(symbol);
    symbol.pointer = reinterpret_cast<void *>(&my_dec);
    symbol.name = "dec";
    symbols.push_back(symbol);
}

class CompilerTestWithFixtures : public ::testing::Test {
public:
    void TearDown() override {
        while (symbols.size() > 5) {
            symbols.pop_back();
        }
    }

    static void SetUpTestCase() {
        buffer = InitProgramCodeBuffer();
        InitSymbolsWithFunctions(symbols);
    }

    static void TearDownTestCase() {
        FreeProgramCodeBuffer(buffer);
        vars_cnt = 0;
    }

    static void *GetBuffer() {
        return buffer;
    }

    static std::vector<Symbol> GetSymbols() {
        return symbols;
    }

    static void AddVariable(const char *name, int value) {
        Symbol symbol;
        vars[vars_cnt] = value;
        symbol.pointer = vars + vars_cnt;
        symbol.name = name;
        symbols.push_back(symbol);
        ++vars_cnt;
    }

private:
    static void *buffer;
    static int vars[100];
    static int vars_cnt;
    static std::vector<Symbol> symbols;
};

void *CompilerTestWithFixtures::buffer;
std::vector<Symbol> CompilerTestWithFixtures::symbols;
int CompilerTestWithFixtures::vars[100];
int CompilerTestWithFixtures::vars_cnt;

TEST_F(CompilerTestWithFixtures, ExpressionWithLiteralsTest) {
    JITCompilerIntoARM32 compiler;
    std::string expression = "-1+(2+2)*5-7";
    compiler.CompileIntoAllocatedMemory(expression,
                                        CompilerTestWithFixtures::GetSymbols(), CompilerTestWithFixtures::GetBuffer());
    EXPECT_EQ(CallFunction(CompilerTestWithFixtures::GetBuffer()), 12);
}

TEST_F(CompilerTestWithFixtures, ExpressionWithVariablesTest) {
    JITCompilerIntoARM32 compiler;
    std::string expression = "(a-10)*xyz-6";
    CompilerTestWithFixtures::AddVariable("a", 3);
    CompilerTestWithFixtures::AddVariable("xyz", -2);

    compiler.CompileIntoAllocatedMemory(expression,
                                        CompilerTestWithFixtures::GetSymbols(), CompilerTestWithFixtures::GetBuffer());
    EXPECT_EQ(CallFunction(CompilerTestWithFixtures::GetBuffer()), 8);
}

TEST_F(CompilerTestWithFixtures, ExpressionWithFunctionsTest) {
    JITCompilerIntoARM32 compiler;
    std::string expression = "mod(inc(div(div(66, bb), 5))*7, de)";
    CompilerTestWithFixtures::AddVariable("bb", 3);
    CompilerTestWithFixtures::AddVariable("de", 6);

    compiler.CompileIntoAllocatedMemory(expression,
                                        CompilerTestWithFixtures::GetSymbols(), CompilerTestWithFixtures::GetBuffer());
    EXPECT_EQ(CallFunction(CompilerTestWithFixtures::GetBuffer()), 5);
}

TEST_F(CompilerTestWithFixtures, BigNumbersTest) {
    JITCompilerIntoARM32 compiler;
    std::string expression = "div(ffe+100000000-f, 1000)+3493";
    CompilerTestWithFixtures::AddVariable("f", 1000000000);
    CompilerTestWithFixtures::AddVariable("ffe", 2000000000);

    compiler.CompileIntoAllocatedMemory(expression,
                                        CompilerTestWithFixtures::GetSymbols(), CompilerTestWithFixtures::GetBuffer());
    EXPECT_EQ(CallFunction(CompilerTestWithFixtures::GetBuffer()), 1103493);
}

/////////////////////////////////////////////////////////////////////////////

int
main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
