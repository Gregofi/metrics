
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "CyclomaticVisitor.hpp"

bool CyclomaticVisitor::VisitFunctionDecl(clang::FunctionDecl *decl)
{

    return true;
}
