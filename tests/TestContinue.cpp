#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include "Interpreter.hpp"

using namespace std;

string expectedOutput()
{
    string res = "";
    for (int i = 0; i < 10; i++)
	{
		res += "   i = " + to_string(i) + "\n";
		if (i > 5)
			continue;
		i++;
	}
    return res;
}

int main()
{
    string filename = "continue_test.mdl";
    Interpreter interpreter(filename);
	ostringstream oss;
	streambuf* ogBuf = cout.rdbuf(oss.rdbuf());

	interpreter.interpret();
	cout.rdbuf(ogBuf);
	string output = oss.str();

    assert(output == expectedOutput());
	return 0;
}