#include <filesystem>
#include "Executer.cpp"

using namespace std;


//_________________________________________MODEL LANGUAGE PROGRAM INTERPRETER__________________________________________
class Interpreter
{
	Scanner scanner;
	Executer executer;

public:
	Interpreter(const string fileName): scanner(fileName) {}
	
	void interpret()
	{
		vector<Lexeme> sourceCode = scanner.getLexemesList();		// Converts source code into a list of lexical tokens
		Parser parser(sourceCode);
        parser.validateProgram();									// Conduct lexical, syntactic and semantic analysis of the program. Build the RPN table
		auto RPNs = parser.getRPNTable();							// Retreive RPN table of the parsed program
		executer.executeProgram(RPNs);								// Execute the parsed program
	}
};