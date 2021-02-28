//
// Created by filip on 2/25/21.
//

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "CyclomaticVisitor.hpp"
#include "MetricVisitor.hpp"
#include "FuncInfoVisitor.hpp"
#include "Metric.hpp"



bool MetricVisitor::VisitFunctionDecl(clang::FunctionDecl *decl)
{
    clang::SourceManager &sm(context->getSourceManager());
    /* Don't calc if source code is not in main file. */
    if(!sm.isInMainFile(decl->getLocation())
        || !decl->isThisDeclarationADefinition()
        || !decl->hasBody()){
        return true;
    }

    llvm::outs() << decl->getQualifiedNameAsString() << "\n";
    FuncInfoVisitor v1(context);
    CyclomaticVisitor v2(context);
    for(const auto & x : v1.calcMetric(decl))
        llvm::outs() << x.name << " " << x.val << "\n";
    for(const auto & x : v2.calcMetric(decl))
        llvm::outs() << x.name << " " << x.val << "\n";

    llvm::outs() << "-----------\n";
    return true;
}




