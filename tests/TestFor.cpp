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
	for (int i = 0; i < x; i++)
	{
		x = x - 2;
		y = y + 1;
		res += "Iteration " + to_string(i) + ": x = " + to_string(x) + ", y = " + to_string(y) + "\n";
	}
	res += "Final result: x = " + to_string(x) + ", y = " + to_string(y) + "\n";
    return res;
}

int main()
{
    string filename = "for_test.mdl";
    Interpreter interpreter(filename);
	ostringstream oss;
	streambuf* ogBuf = cout.rdbuf(oss.rdbuf());

	interpreter.interpret();
	cout.rdbuf(ogBuf);
	string output = oss.str();

    assert(output == expectedOutput());
	return 0;
}