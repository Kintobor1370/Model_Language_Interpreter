#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include "Interpreter.hpp"

using namespace std;

string expectedOutput()
{
    string res = "";
    int x;
    for (x = 1; x <= 10; x++)
	{
		res += "x = " + to_string(x) + 
				"\nCheck switch statement:\n";
		if (x <= 5)
		{
			res += "   x = " + to_string(x) + "\n";
		}
		else
		{
    		res += "   x > 5\n";
		}
		res += " \n";
	}
    return res;
}

int main()
{
    string filename = "switch_test.mdl";
    Interpreter interpreter(filename);
	ostringstream oss;
	streambuf* ogBuf = cout.rdbuf(oss.rdbuf());

	interpreter.interpret();
	cout.rdbuf(ogBuf);
	string output = oss.str();

    assert(output == expectedOutput());
	return 0;
}