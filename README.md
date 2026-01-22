# decaf-22-compiler

A C++ implementation of the compiler frontend for the Decaf-22 programming language. This project focuses on scanning, semantic analysis and syntax analysis. It takes source code to a validatesd Abstract Syntax Tree (AST).

## Language Specification
Decaf-22 is a strongly-typed, procedural language resembling a simplified subset of C/Java. It features strict type isolation and eager boolean evaluation. It supports standard control structures, recursion and the following five primitive types: int, bool, double, string, and void.

For a full language spec refer to the document located at: `doc/decafOverview.pdf`

## Project Structure
`doc` contains the language spec.
`samples` contains `.frag` and `.out` files. Each `.frag` represents a code snippet of the decaf 22 language. Each `.out` represents the expected compiler output.
`src` contains the actual source code of the compiler.


## Build Instructions
The project was developed using g++ and C++ standard 11.
```
cd workdir
./build.sh
```

## Run Instructions
```
cd workdir
./exec.sh <path to decaf-22 source code>
```

## Test Instructions
```
./buildAndTest.sh
```
