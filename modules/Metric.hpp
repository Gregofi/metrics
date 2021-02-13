//
// Created by filip on 2/13/21.
//

#ifndef METRICS_METRIC_HPP
#define METRICS_METRIC_HPP

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
    std::string getName() const noexcept { return name; }
    std::vector<Metric> getMetrics() const noexcept { return metrics; }
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
