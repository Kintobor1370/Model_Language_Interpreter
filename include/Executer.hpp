#pragma once

#include "Parser.hpp"


//___________________________________________MODEL LANGUAGE PROGRAM EXECUTER___________________________________________
class Executer
{
	stack<double> args;												// stack of values of int / real / bool arguements
    stack<std::string> strConstsStack;								// stack of string constants
	stack<lexemeType> typesStack;									// stack of lexeme types
	
	// Execution error processing
	void executionError(std::string errMessage);
	
	// Execution warning processing
	void executionWarning(std::string err);

public:
	Executer();

	// Execute program
	void executeProgram(std::vector<Lexeme> &RPNTable);
	
	// Execute 'write()' command
	void executeWrite();
};