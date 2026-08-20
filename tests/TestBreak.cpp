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
	int i, j;
	for (i = 0; i <= 9999; i++)
	{
		x = x * 2;
		if (i >= 5)
			break;
	}
	res += "x = " + to_string(x) + "\n" +
		   "i = " + to_string(i) + "\n" +
		   " \n";

	x = 0; 
	for (j = 0; j < 10; j++)
	{
		x = x + 2;
		for (i = 0; i <= 9999; i++)
		{
			x = x + 2;
			if (i >= 5)
				break;
		}
	}
	res += "x = " + to_string(x) + "\n" +
		   "i = " + to_string(i) + "\n" +
		   "j = " + to_string(j) + "\n";
    return res;
}

int main()
{
    string filename = "break_test.mdl";
    Interpreter interpreter(filename);
	ostringstream oss;
	streambuf* ogBuf = cout.rdbuf(oss.rdbuf());

	interpreter.interpret();
	cout.rdbuf(ogBuf);
	string output = oss.str();

    assert(output == expectedOutput());
	return 0;
}
