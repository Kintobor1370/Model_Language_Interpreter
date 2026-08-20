#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include "Interpreter.hpp"

using namespace std;

string expectedOutput()
{
	string res = "";
    int x = 5;
	int y = 10;		
	if (x > y)
	    res = to_string(x) + " is greater than " + to_string(y) + "\n";
	else
		res = to_string(x) + " is less or equal than " + to_string(y) + "\n";
			
	x = 10;
	if (x > y)
		res += to_string(x) + " is greater than " + to_string(y) + "\n";
	else if (x < y)
		res += to_string(x) + " is less than " + to_string(y) + "\n";
	else
		res += to_string(x) + " and " + to_string(y) + " are equal\n";

    return res;
}

int main()
{
    string filename = "if_test.mdl";
    Interpreter interpreter(filename);
	ostringstream oss;
	streambuf* ogBuf = cout.rdbuf(oss.rdbuf());

	interpreter.interpret();
	cout.rdbuf(ogBuf);
	string output = oss.str();

    assert(output == expectedOutput());
	return 0;
}
