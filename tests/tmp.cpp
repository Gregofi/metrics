

class foo
{
public:
    foo() : i(0){}
    int hello()
    {
       return ++i; 
    }
protected:
    int i;
};

class empty
{

};

class bar : public foo, public empty
{
public:
    bar() : foo(){i = 10;}
};

int main()
{
    auto f = foo();
    return f.hello();
}