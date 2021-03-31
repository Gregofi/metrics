# Metrics
Calculates software metrics for given source codes, targeted for C/C++ sources.  
Currently implemented metrics:  
* For each function, it calculates following:
    * Lines of code (for now, this also counts lines containing only 
        comments and empty lines :( )
    * Maximum statement depth 
    * Number of C/C++ statements
    * Cyclomatic complexity
    * NPath
    * Halstead token count
    * Fan-in and Fan-out

---

This tool is based on clang-libtooling, and requires it to be built.  

## How to build this (Works, but is currently outdated and there is much easier way to do it.)
You need to have the llvm and clang sources downloaded to build this.
1. Put this in the tools directory, eg. llvm-project/clang/tools
2. Add following to to llvm-project/clang/tools/CMakeLists.txt : "add_subdirectory(metrics)"
3. Go to folder where you want to build your llvm project.
4. Run "cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS=clang -G "Unix Makefiles" /path/to/llvm".
If you omit BUILD_TYPE, it will default to debug, which is approximately 60GB, release is about 4GB.
5. Run "make check-all"
6. Go make yourself a coffee, this will take some time.
7. Binary is then located at ./bin/metrics