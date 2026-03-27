# Model language interpreter

A C++ console application that parses and executes programs written in a custom programming language. The interpreter performs lexical, analysis, syntax parsing, and semantic validation, and executes code using Reverse Polish Notation (RPN).

__Supported data types:__
- Integer: `int`
- String: `string`
- Boolean: `bool`

__Operators and their precedence:__
1. Logical NOT: `not`
2. Increment / Decrement: `++`, `--`
3. Multiplication / Division: `*`, `/`
4. Logical AND: `and`
5. Addition / Subtraction: `+`, `-`
6. Logical NOT: `or`
7. Relational: `==`, `!=`, `<`, `>`, `<=`, `>=`
9. Assignment: `=`

__Control flow:__
- Conditional statements: `if()`, `else`
- Loops: `while()`, `for()`
- Nested loops
- Jump statements: `break`, `goto`

The semantics of the aforementioned features are similar to those in the C programming language.

__Input and Output:__
- `read()`: Reads a __single__ variable <br> _Example:_ `int x; read(x);`
- `write()`: Prints one or more expressions

__String operators:__
- Declaration and assignment: `string str = "Hello, World!";`
- Concatenation: ```string c = a + b;``` (Here, `a` and `b` are string constants)
- Comparison: `>`, `<`, `==`, `!=` <br> _Example:_ `bool b = "Hello" == "Hello"; // true`

Test cases are included in the _tests_ folder.

# To build and run the interpreter on Windows:
1. Run the following command prompt:
```
g++ TestInterpreter.cpp -o Interpreter.exe
Interpreter.exe
```
2. Pick a test from the _tests_ folder, enter its file name and press Enter <br> Example: <br> <img width="379" height="74" alt="image" src="https://github.com/user-attachments/assets/0eafaa70-296a-4191-8d88-0cec6cc6092f" /> <br> <img width="821" height="417" alt="image" src="https://github.com/user-attachments/assets/d5fec917-49c1-4c03-af3a-c47a72ada396" />
3. You may also write your own code file. To execute it, place it in the _Model_Language_Interpreter_ folder.


