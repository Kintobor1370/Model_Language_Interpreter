# Model language interpreter

A C++ console application that parses and executes a program written in a model programming language.
Capable of detecting lexical, syntax, and semantic errors.

# Model language syntax
The model language can be described with the following EBNF:

Available operators and their precedence:
1. Logical inversion: ```not```
2. Multiplication, division: ```*```, ```/```
3. Addition, subtraction: ```+```, ```-```
4. Relational operators: ```==```, ```!=```, ```<```, ```>```, ```<=```, ```>=```
5. Conjunction: ```and```
6. Disjunction: ```or```
7. Assignment: ```=```

Available _string_ operators:
- Initialization and assignment: ```string str = "Hello, World!";```
- String concatenation: ```string c = a + b;``` (Here, ```a``` and ```b``` are string constants)
- Relational operators: ```>```, ```<```, ```==```, ```!=``` <br> E.g.: ```bool b = "Hello" == "Hello";``` will be true; ```bool b = "Hello" == "World"``` will be false

To compile the app, run the following command prompt:
```
g++ TestInterpreter.cpp -o Interpreter.exe
```
To run the app, run the following command prompt:
```
Interpreter.exe
```
