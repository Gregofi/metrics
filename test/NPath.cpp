#include <iostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

#include "test/Asserts.hpp"
#include "test/TestToolRun.hpp"

#include "include/metrics/NPathVisitor.hpp"

#define GET_VAL(code) ConstructMetricsOneFunction<NPathVisitor>(code).GetValue()

int NoConditionsTest()
{
    ASSERT_EQ(GET_VAL(""), 1);
    ASSERT_EQ(GET_VAL("return 1;"), 1);
    ASSERT_EQ(GET_VAL("return 1 | 2;"), 1);
    ASSERT_EQ(GET_VAL("int a = 1 & 2; return a;"), 1);
    ASSERT_EQ(GET_VAL("int a; {int b; int c = a + b;} return 0;"), 1);
    return 0;
}

int LogicalOperatorsTest()
{
    ASSERT_EQ(GET_VAL("int a, b, c; return a && b || c;"), 3);
    return 0;
}

int IfStmtTest()
{
    ASSERT_EQ(GET_VAL("if(true) {}"), 2);
    ASSERT_EQ(GET_VAL("if(true) {} if(true) {}"), 4);
    ASSERT_EQ(GET_VAL("if(true) { if(true) {} } if(true) {}"), 6);

    ASSERT_EQ(GET_VAL("if(true) { int a; int b; int c; return a + b + c;}"), 2);
    ASSERT_EQ(GET_VAL("if(true) { if(true) {int a;} int b;} if(true) {int c;}"), 6);

    ASSERT_EQ(GET_VAL("if(int a = 1; a < 2) {}"), 2);

    ASSERT_EQ(GET_VAL("int a, b; if(a || b) { if(a && b) {} } else {}"), 5);
    return 0;
}

int IfElseStmtTest()
{
    ASSERT_EQ(GET_VAL("if(true) {int a;int b;} else {int a;}"), 2);
    ASSERT_EQ(GET_VAL("if(true) { if(true) {} else {} } else {int a = 3; a += 2; return a;}"), 3);
    ASSERT_EQ(GET_VAL("if(true) { if(true) {} else if(true) {} else {} } else { if(true) {} }"), 5);
    return 0;
}

int ForStmtTest()
{
    ASSERT_EQ(GET_VAL("for(;;){}"), 2);
    ASSERT_EQ(GET_VAL("for(int a = 0; a < 10; ++ a) {int b = 2;}"), 2);
    ASSERT_EQ(GET_VAL("for(int a = 0; a < 10; ++ a) { if(a > 5) break; }"), 3);
    ASSERT_EQ(GET_VAL("for(int a = 0; a < 10 && a > -10; a += -2*a) { if(a > 5) break; }"), 4);
    return 0;
}

int WhileStmtTest()
{
    ASSERT_EQ(GET_VAL("int a; int b; while(a < b) { a ++; }"), 2);
    ASSERT_EQ(GET_VAL("int a; int b; while(a < b) { if(a + 10 < b) {} }"), 3);
    ASSERT_EQ(GET_VAL("int a; int b; while(a < b) {return -1;} while(a > b) {return 1;} return 0;"), 4);
    return 0;
}

int DoStmtTest()
{
    ASSERT_EQ(GET_VAL("int a; int b; do{a++;}while(a < b);"), 2);
    ASSERT_EQ(GET_VAL("int a; int b; do{ if(a + 10 < b) do {} while(a + 10 < b); }while(a < b);"), 4);
    return 0;
}

int ExprTest()
{
    ASSERT_EQ(GET_VAL("1 && 2;"), 2);
    ASSERT_EQ(GET_VAL("if(true) { int a = 1 ? 0 : 2;} "), 3);
    ASSERT_EQ(GET_VAL("if(true) { int a = 1 && 1 == true ? 0 : 1; } if(true) { 1 && 2;}"), 4*3);
    return 0;
}

int main()
{
    TEST(NoConditionsTest);
    TEST(LogicalOperatorsTest);
    TEST(IfStmtTest);
    TEST(IfElseStmtTest);
    TEST(ForStmtTest);
    TEST(WhileStmtTest);
    TEST(DoStmtTest);
    TEST(ExprTest);
}
