#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include "Interpreter.hpp"

using namespace std;

string expectedOutput()
{
    return string("Logical addition:\n") +
		   string("  true OR true = true\n") +
           string("  true OR false = true\n") +
           string("  false OR true = true\n") +
           string("  false OR false = false\n") +
           "\n" +
           string("Logical multiplication:\n") +
           string("  true AND true = true\n") +
           string("  true AND false = false\n") +
           string("  false AND true = false\n") +
           string("  false AND false = false\n") +
           "\n" +
           string("Logical reverse:\n") +
           string("  not true = false\n") +
           string("  not false = true\n");
}

int main()
{
    string filename = "bool_test.mdl";
    Interpreter interpreter(filename);
	ostringstream oss;
	streambuf* ogBuf = cout.rdbuf(oss.rdbuf());

	interpreter.interpret();
	cout.rdbuf(ogBuf);
	string output = oss.str();

    assert(output == expectedOutput());
	return 0;
}