# Model Language Interpreter

A C++ console application that parses and executes programs written in a custom programming language. The interpreter performs lexical analysis, syntax parsing, and semantic validation, and executes code using Reverse Polish Notation (RPN).

## Supported data types

- Integer: `int`
- Real: `real`
- String: `string`
- Boolean: `bool`

## Operators and their precedence

1. Logical NOT: `not`
2. Increment / Decrement: `++`, `--`
3. Multiplication / Division: `*`, `/`
4. Logical AND: `and`
5. Addition / Subtraction: `+`, `-`
6. Logical OR: `or`
7. Relational: `==`, `!=`, `<`, `>`, `<=`, `>=`
9. Assignment: `=`

## Control flow

- Conditional statements: `if()`, `else`
- Loops: `while()`, `do {...} while()`, `for()`
- Nested loops
- Jump statements: `break`, `goto`

The semantics of the aforementioned features are similar to those in the C programming language.

## Input and Output

- `read()`: Reads a __single__ variable <br> _Example:_ `int x; read(x);`
- `write()`: Prints one or more expressions
- `writeline()`: Prints one or more expressions, followed by the current line terminator

## String operators

- Declaration and assignment: `string str = "Hello, World!";`
- Concatenation: ```string c = a + b;``` (Here, `a` and `b` are string constants)
- Comparison: `>`, `<`, `==`, `!=` <br> _Example:_ `bool b = "Hello" == "Hello"; // true`

## Comments

- Both one-line and multi-line comments are supported
- One-line comment example: ```// string str = "One-line comment";```
- Multi-line comment example:
  ```
  /*
  string substr1 = "Multi-";
  string substr2 = "line ";
  string substr3 = "comment";
  string str = substr1 + substr2 + substr3;
  */
  ```

Test cases are included in the _tests_ folder.

# To build and run the interpreter on Windows:
1. Run the following command prompt:
```
g++ TestInterpreter.cpp -o Interpreter.exe
Interpreter.exe
```
2. Pick a test from the _tests_ folder, enter its file name and press Enter <br> Example: <br> <img width="409" height="105" alt="image" src="https://github.com/user-attachments/assets/f74e1af2-cc16-47a3-8cb8-b09f046df328" /> <br> <img width="815" height="230" alt="image" src="https://github.com/user-attachments/assets/a8912526-f641-47b5-b3b0-834e9655cecd" />
3. You may also write your own code file. The file __must__ have _.mdl_ extension. To execute it, place it in the _Model_Language_Interpreter_ folder.


