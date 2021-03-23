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

#include "CyclomaticVisitor.hpp"
#include "MetricVisitor.hpp"
#include "FuncInfoVisitor.hpp"
#include "Metric.hpp"
#include "AbstractVisitor.hpp"


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

    std::vector<std::unique_ptr<AbstractVisitor> > visitors;
    visitors.emplace_back(std::make_unique<FuncInfoVisitor>(context));
    visitors.emplace_back(std::make_unique<CyclomaticVisitor>(context));
    for(const auto & x : visitors)
    {
        x->CalcMetrics(decl);
        x->Export(std::cout);
    }
    std::cout << std::endl;

    return true;
}




