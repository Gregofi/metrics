//
// Created by filip on 2/25/21.
//

#include <vector>
#include <memory>
#include <iostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "include/metrics/CyclomaticVisitor.hpp"
#include "include/MetricVisitor.hpp"
#include "include/metrics/FuncInfoVisitor.hpp"
#include "include/metrics/HalsteadVisitor.hpp"
#include "include/metrics/NPathVisitor.hpp"

#include "include/FunctionVisitor.hpp"


bool MetricVisitor::VisitFunctionDecl(clang::FunctionDecl *decl)
{
    clang::SourceManager &sm(context->getSourceManager());

    /* Don't calc if source code is not in main file. */
    if(!sm.isInMainFile(decl->getLocation())
        || !decl->isThisDeclarationADefinition()
        || !decl->hasBody()){
        return true;
    }

    std::cout << decl->getQualifiedNameAsString() << "\n";

    std::vector<std::unique_ptr<FunctionVisitor> > visitors;
    visitors.emplace_back(std::make_unique<FuncInfoVisitor>(context));
    visitors.emplace_back(std::make_unique<CyclomaticVisitor>(context));
    visitors.emplace_back(std::make_unique<HalsteadVisitor>(context));
    visitors.emplace_back(std::make_unique<NPathVisitor>(context));

    for(const auto & x : visitors)
    {
        x->CalcMetrics(decl);
        x->Export(std::cout);
    }
    std::cout << std::endl;

    return true;
}





