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
	int y = 7;
	int z = 8;
	goto Label;

Label:
	x++;
	y--;
	res += "x = " + to_string(x) + "\n" +
		   "y = " + to_string(y) + "\n" +
		   " \n";
	x++;
	if (x < 10)
		goto Label;	
    return res;
}

int main()
{
    string filename = "goto_test.mdl";
    Interpreter interpreter(filename);
	ostringstream oss;
	streambuf* ogBuf = cout.rdbuf(oss.rdbuf());

	interpreter.interpret();
	cout.rdbuf(ogBuf);
	string output = oss.str();

    assert(output == expectedOutput());
	return 0;
}