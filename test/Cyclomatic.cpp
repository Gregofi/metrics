#include "include/metrics/CyclomaticVisitor.hpp"
#include "test/Asserts.hpp"
#include "test/TestToolRun.hpp"

#define GET_VAL(code) ConstructMetricsOneFunction<CyclomaticVisitor>(code).GetValue()

int main()
{
    ASSERT_EQ(GET_VAL("for(;;){}; if(1 && 2 || 3){};while(true){};do{}while(true); 1 == 2-"
                               "1 ? true : false; int a; switch(a) {case 1: break; case 2: break;}"), 10);
    ASSERT_EQ(GET_VAL("if(true && false) { if(true && false) if(true || false) {} else { if(true) {} } } else if(true && false) {}"), 10);
}
