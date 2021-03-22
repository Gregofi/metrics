//
// Created by filip on 3/22/21.
//

#ifndef METRICS_HALSTEADVISITOR_HPP
#define METRICS_HALSTEADVISITOR_HPP


#include <map>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "MetricVisitor.hpp"
#include "Metric.hpp"

class HalsteadVisitor : public RecursiveASTVisitor<HalsteadVisitor>
{

};


#endif //METRICS_HALSTEADVISITOR_HPP
