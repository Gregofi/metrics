#include <cstdio>


template <typename T>
class A
{
public:
    A(T gamma) : gamma(gamma) {}
    T foo(T t1, T t2)
    {
        return t1*t2;
    }
    T bar(T t) const
    {
        return t*gamma;
    }
private:
    T gamma;
};


int main(void)
{
    int a;
    scanf("%d", &a);
    A<int> c1(a);
    A<char> c2('a' + a);
    A<double> c3(1.2 + a);
    return c1.bar(2) * c2.bar('a') * c3.bar(3.7);
}