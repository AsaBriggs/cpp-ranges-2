C++ Ranges version 2

What is different from https://github.com/AsaBriggs/cpp-ranges ?
1) C++11 only library; use auto and decltype and standard library facilities where appropriate to make life easier
2) Create a non-extensible template class to cope with the different range types; this limits the problem to be solved.

Why the non-extensible template class? Too much time was being spent on a generic property mechanism without regard for how algorithms would interact with it.

To make the test program run "make" in this directory from the command prompt.