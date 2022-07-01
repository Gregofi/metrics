class Foo {
    public:
        Foo(int m) : m(m) {}
        int bar() { return m; }
    private:
        int m;
};

int main() {
    auto f = Foo(5);
    return f.bar() * 2;
}