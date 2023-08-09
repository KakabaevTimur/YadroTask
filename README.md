# Gaming room task

Build:
```cmd
cmake . -B build
cmake --build build
```
Build is tested on Windows with MSVC 14.36.32532 and on Linux with GCC 13.2.
Code utilizes C++20.

To start program:
```cmd
.\build\Debug\prog.exe path\to\file\input.txt
```

To run tests:
```cmd
.\build\tests\Debug\test.exe
```
Tests are written with GTest.
