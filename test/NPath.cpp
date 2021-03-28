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

#include "include/NPathVisitor.hpp"

int IfStmt()
{
    ASSERT_EQ(ConstructMetricsOneFunction<NPathVisitor>("")[0].val, 1);
    ASSERT_EQ(ConstructMetricsOneFunction<NPathVisitor>("if(true) {}")[0].val, 2);
    ASSERT_EQ(ConstructMetricsOneFunction<NPathVisitor>("if(true) {} if(true) {}")[0].val, 4);
    ASSERT_EQ(ConstructMetricsOneFunction<NPathVisitor>("if(true) { if(true) {} } if(true) {}")[0].val, 6);

    ASSERT_EQ(ConstructMetricsOneFunction<NPathVisitor>("if(true) { int a; int b; int c; return a + b + c;}")[0].val, 2);
    ASSERT_EQ(ConstructMetricsOneFunction<NPathVisitor>("if(true) { if(true) {int a;} int b;} if(true) {int c;}")[0].val, 6);

    return 0;
}

int IfElseStmt()
{

}

int main()
{
    TEST(IfStmt);

}
