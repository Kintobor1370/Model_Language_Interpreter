#pragma once

#include "Executer.hpp"

//_________________________________________MODEL LANGUAGE PROGRAM INTERPRETER__________________________________________
class Interpreter
{
	Scanner scanner;
	Executer executer;

public:
    // Constructor: pass the program file name to the interpreter
	Interpreter(const string fileName);
	
    // Interpret the source code of the program
	void interpret();
};