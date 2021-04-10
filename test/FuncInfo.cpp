#include "test/Asserts.hpp"
#include "TestToolRun.hpp"
#include "include/metrics/FuncInfoVisitor.hpp"



const char *basic = R"(
int a;
)";

const char *sw = R"(
    int a;
    /* A switch statement */
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
    // And yet another switch statement
    switch(a)
        case 1:
            return 1;
)";

const char *ifstmt = R"(
    int a;
    if(a > 10)
        return 1;
    else if(int b = a * 10; b < 50)
        return 2;
)";

int main()
{
    auto vis = ConstructMetricsOneFunction<FuncInfoVisitor>(basic);
    ASSERT_EQ(vis.GetResult().statements, 1);
    ASSERT_EQ(vis.GetResult().depth, 1);
    vis = ConstructMetricsOneFunction<FuncInfoVisitor>(sw);
    ASSERT_EQ(vis.GetResult().statements, 14);
    ASSERT_EQ(vis.GetResult().depth, 3);
    vis = ConstructMetricsOneFunction<FuncInfoVisitor>(ifstmt);
    ASSERT_EQ(vis.GetResult().statements, 5);
    ASSERT_EQ(vis.GetResult().depth, 2);
}