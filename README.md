# :straight_ruler: Metrics
Software based on Clang Libtooling API. It calculates software metrics for given C and C++ source codes.

Currently implemented metrics:
* Function metrics
    * Lines of code (Including comments and empty lines)
    * Maximum statement depth
    * Number of C/C++ statements
    * Cyclomatic complexity
    * NPath
    * Halstead token count
    * Fan-in and Fan-out
* Class metrics
    * Number of immediate children
    * Length of inheritance chain
    * Lack of cohesion
    * Coupling
    * Attribute and method count
    * Number of overriden methods

---

## Metrics description
Here, you can find brief description of measured metrics.
### Function metrics

#### Lines of code - LOC
Calculates raw amount of lines that function body has. This also includes empty lines and comments.

#### Number of statements - NOS
Calculates number of statements that function body has.
This should work as somewhat more intelligent LOC count. Consider this:
```c++
if(a > b) { foo(); bar(); }
```
This will be one LOC, but three NOS.

#### Cyclomatic complexity
Calculates number of branching statements (`if`, `for`, `while`...). This should be equal to
number of linearly independent paths through program. Lambdas are considered as function body.

#### NPATH
Calculates number of acyclic paths through program. Consider following code
```c++
if(a)
    foo();
if(b)
{
    if(c)
        foo();
    bar();
}
```
This code has 6 acyclic paths through it. `if(c)` has two possible paths, either `c` is true or not.
`if(b)` itself has also two paths, that gives us three paths total.
```
    if(b)
    /   \
   /    if(c)
  /     /   \
```
`if(a)` has two paths. In total that is 3*2 = 6. Note the difference from Cyclomatic complexity, which would be 4.

Lambdas NPATH complexity in functions is then added to function complexity.
#### Fan-in, Fan-out
Counts how many times each function has been called (Fan-out) and how many functions are called by one currently measured function (Fan-in).
Function call, member function and overloaded operator call are all considered, constructor and destructor
calls are not.

### OOP metrics
These metrics measure values for every class.
#### Number of children
Number of classes that directly inherit from this class.
#### Inheritance chain length
Maximum length of all inheritance chains for this class. For example
```c++
class A;
class B : A;

class D;

class X : B, D;
```
Inheritance tree for these classes would look like this
```$xslt
A   D
|   |
B   X
|
X
```
So, for class `X` it would be 2.
#### Lack of cohesion
#### Coupling

## :hammer: How to build
### Linux
This was tested on Ubuntu 20.04.2 LTS and Arch linux.
You need following packages:
- cmake
- clang
- g++
- llvm-dev
- libclang-dev

For debian, you might need libclang-cpp-dev

Follow these steps
1. Clone this repository.
2. Create a build folder. Run `cmake` from it, as argument give it path to the cloned repository.
3. Run `make`.
4. Optionally run unit tests via `make test`.

### OSX
You need following packages:
- cmake
- llvm

The build steps are the same, hovewer cmake also needs the folder of llvm and clang,
which needs to be exported as env variable, for example:
```
export CLANG_DIR=/opt/homebrew/Cellar/llvm/<version>/lib/cmake/clang
export LLVM_DIR=/opt/homebrew/Cellar/llvm/<version>/lib/cmake/llvm
```

### Run
The app is command line based. It accepts as argument the path to files you want to measure.

If you want to measure a project that has more complicated build system, give it a path to
folder containing compile_commands.json generated via cmake (most often the build folder)
with switch -p. For example ``metrics -p build `find . -type f -name "*.cpp"``.