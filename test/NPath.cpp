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

int main()
{
    ASSERT_EQ(ConstructMetricsOneFunction<NPathVisitor>("")[0].val, 1);
    ASSERT_EQ(ConstructMetricsOneFunction<NPathVisitor>("if(true) {}")[0].val, 2);
    ASSERT_EQ(ConstructMetricsOneFunction<NPathVisitor>("if(true) {} if(true) {}")[0].val, 4);
    ASSERT_EQ(ConstructMetricsOneFunction<NPathVisitor>("if(true) { if(true) {} } if(true) {}")[0].val, 6);
}
