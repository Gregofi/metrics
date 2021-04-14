# Metrics
Calculates software metrics for given source codes, targeted for C/C++ sources.  
Currently implemented metrics:  
* For each function, it calculates following:
    * Lines of code (Including comments and empty lines)
    * Maximum statement depth 
    * Number of C/C++ statements
    * Cyclomatic complexity
    * NPath
    * Halstead token count
    * Fan-in and Fan-out
* For each class, it calculates following:
    * Number of immediate children
    * Length of inheritance chain
    * Lack of cohesion
    * Coupling
    * Attribute and method count
    * Number of overriden methods

---

## Metric calculation
Here, you can brief description of said metrics.
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
number of linearly independent paths through program.

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
#### Fan-in, Fan-out
Counts how many times each function has been called (Fan-out) and how much functions one function calls (Fan-in).
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

## How to build.
This was tested on Ubuntu 20.04.2 LTS and Arch linux.
You need following packages:
- cmake
- clang
- g++
- llvm-dev
- libclang-dev

You need to have the llvm and clang sources downloaded to build this.
1. Clone this repository.
2. Create a build folder. Run `cmake` from it, as argument give it the path to the cloned repository.
3. Run `make`.
4. Optionally run unit tests via `make test`.