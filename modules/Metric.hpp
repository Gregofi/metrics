
#ifndef METRICS_METRIC_HPP
#define METRICS_METRIC_HPP

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

struct Metric
{
    std::string name;
    int val;
};

class Function
{
public:
    Function(std::string name) noexcept : name(std::move(name)){}
    Function(std::string name, std::vector<Metric> &&metrics) noexcept : name(std::move(name)), metrics(std::move(metrics)){}
    Function() = default;
    std::string getName() const noexcept { return name; }
    std::vector<Metric> getMetrics() const noexcept { return metrics; }
    /**
     * Appends given metrics to already existing metrics in this function
     * @param metrics
     * @return
     */
    Function& insert(std::initializer_list<Metric> &&metrics)
    {
        this->metrics.insert(this->metrics.end(), metrics);
        return *this;
    }
    auto begin()
    {
        return metrics.begin();
    }
    auto end()
    {
        return metrics.end();
    }
private:
    std::string name;
    std::vector<Metric> metrics;
};

#endif //METRICS_METRIC_HPP
