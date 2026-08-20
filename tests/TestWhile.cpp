#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include "Interpreter.hpp"

using namespace std;

string expectedOutput()
{
    string res = "";
    int x = 10;
	int y = 10;
	while ((x <= 30) || (y > 5))
	{
		x = x + 2;
		y = y - 1;
	}
	res = "x = " + to_string(x) + "\n" +
		  "y = " + to_string(y) + "\n" +
		  " \n";

	while ((x >= 5) && (y < 20))
	{
		x = x - 2;
		y = y + 1;
	}
	res += "x = " + to_string(x) + "\n" +
		   "y = " + to_string(y) + "\n";
    
    return res;
}

int main()
{
    string filename = "while_test.mdl";
    Interpreter interpreter(filename);
	ostringstream oss;
	streambuf* ogBuf = cout.rdbuf(oss.rdbuf());

	interpreter.interpret();
	cout.rdbuf(ogBuf);
	string output = oss.str();

    assert(output == expectedOutput());
	return 0;
}