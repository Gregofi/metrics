#include "test/Asserts.hpp"
#include "TestToolRun.hpp"
#include "include/metrics/FuncInfoVisitor.hpp"

const char *sw = R"(
    int a;
    switch(a)
    {
        int b;
        case 1:
            b = 2;
            a = 3;
            break;
        case 2:
            a = 2;
            b = 3;
            break;
    }

    switch(a)
        case 1:
            return 1;
)";

const char *ifstmt = R"(
    int a;
    if(a > 10)
        return 1;
    if(int b = a * 10; b < 50)
        return 2;
)";

int main()
{
    auto vis = ConstructMetricsOneFunction<FuncInfoVisitor>(sw);
    ASSERT_EQ(vis.GetResult().statements, 14);
    vis = ConstructMetricsOneFunction<FuncInfoVisitor>(ifstmt);
    ASSERT_EQ(vis.GetResult().statements, 5);
}