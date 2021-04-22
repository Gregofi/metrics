#include "include/metrics/HalsteadVisitor.hpp"
#include "test/Asserts.hpp"
#include "test/TestToolRun.hpp"



#define GET_VAL(code) ConstructMetricsOneFunction<HalsteadVisitor>(code)

int main()
{
    auto vis0 = GET_VAL("");
    ASSERT_EQ(vis0.GetOperatorCount(), 0);
    ASSERT_EQ(vis0.GetOperandCount(), 0);

    const char *s1 = "int b; int a = 1 + 2;";
    auto vis1 = GET_VAL(s1);
    ASSERT_EQ(vis1.GetOperatorCount(), 4);
    ASSERT_EQ(vis1.GetOperandCount(), 4);

    const char *s2 = "int a = 1;"
                     "a += 3;"
                     "if(a < 2) a = 3;"
                     "return a;";
    auto vis2 = GET_VAL(s2);
    ASSERT_EQ(vis2.GetOperatorCount(), 7);
    ASSERT_EQ(vis2.GetOperandCount(), 9);

    const char *s3 = "int a = 1;"
                     "while(a == 1) { a ++; }"
                     "if(false) main();"
                     "for(int *i = &a; i; ++ (*i)) { i = nullptr; }";
    auto vis3 = GET_VAL(s3);
    ASSERT_EQ(vis3.GetOperatorCount(), 14);
    ASSERT_EQ(vis3.GetOperandCount(), 13);

}