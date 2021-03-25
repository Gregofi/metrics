
int foo()
{
    int a;
    int b = 3;
    int c{3};
    // 5 operators and 5 operands

    int d = a + b * c;
    // 4 operators and 4 operands

    if(a < b) // 2 operators and 2 operands
        return 4 + 4; // 2 operators and 2 operands
    return 5; // 1 operator and 1 operand

    // summary : 14 operators and 14 operands
}