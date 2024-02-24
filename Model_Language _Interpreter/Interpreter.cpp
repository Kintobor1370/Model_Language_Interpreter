#include "Executer.cpp"

using namespace std;


//_________________________________________MODEL LANGUAGE PROGRAM INTERPRETER__________________________________________
class Interpreter
{
	Parser parser;
	Executer executer;

public:
	Interpreter(const string fileName): parser(fileName) {}
	
	void interpret()
	{
        parser.analyse();                                           // Conduct lexical, syntax and semantic analysis of the code. Retreive RPN vector
		auto RPNs = parser.getRPNs();								// Retreive RPN table of the analysed code
		executer.execute(RPNs);	                    				// Execute the analysed code
	}
};