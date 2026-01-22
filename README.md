# decaf-22-compiler
This project was a final project for my Compilers and Programming Languages class.
I implemented a scanner, semantic analyzer and syntax analyzer.

`doc/decafOverview.pdf` contains the language spec for the decaf 22 language.

`samples/semantic_analyzer` and `samples/syntax_analyzer` contain `.frag` and `.out` files. Each `.frag` represents a code snippet of the decaf 22 language. Each `.out` represents the expected compiler output.

## Build Instructions

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
