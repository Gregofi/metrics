# Testing
A small testing framework is provided. You need to specify expected values of the metrics.
It is not required to provide expected values for all classes and all metrics in given test case.
If there are some values missing, tester will ignore them and will only match the existing ones.

## Tests format

Tests have the following structure:

```
/integration_tests
  /01
    /in
      foo.cpp
      bar.cpp
      foo.h
      ...
    expected.yaml
    conf.yaml
  /02
  ...

```

Files starting with `/` are folders. Each test is in its own folder. Each folder then contains the `in` directory, where the measured files are. You then need to specify following in the `config.yaml`:
- Which files are to be measured. If not specified then all files in the folder will be measured.
- How many functions should be measured.
- How many classes should be measured.

The last two values are not meant as a limit for the measurer but rather if it should be tested that the measurer measured exactly `x` functions or classes. No more, no less. This is to check that it didn't go overboard and not measured things for example for standard library.

### Expected format

Format of the expected output is quite simple:

```
functions:
  - foo:
    - lines: 20
    - cyclomatic: 4
    - npath: 16
  - bar:
    - lines: 30
  - baz:
classes:
  - A:
    - nofpub: 5
    - nofpri: 3
```

In this example, `foo` will be tested that it exists, it has 20 lines of code, 4 cyclomatic complexity and 16 NPATH. `bar` will only be tested for existance and that it has 30 lines of code.
`baz` will only be tested for existance.
It will also be checked that class `A` exists with 5 public methods and 3 private methods.

Use following keys for function metrics
- Cyclomatic complexity: cyclomatic
- Lines of code: lines
- Number of statements: statements
- NPath: npath
- fan-in: fan_in
- fan-out: fan_out

And for classes
- Number of public methods: methodpub
- Number of private methods: methodpriv
- TODO

The keys are the same as in the XML export.