#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include "Interpreter.hpp"

using namespace std;

string expectedOutput()
{
    string res = "";
    int x = 1;
	x++; 
	res += "x = 1, x++ = " + to_string(x) + "\n";
	x--;
	res += "x = 2, x-- = " + to_string(x) + "\n";

	++x;
	res += "x = 1, ++x = " + to_string(x) + "\n";
	--x;
	res += "x = 2, --x = " + to_string(x) + "\n";
	res += " \n";

	int y = 5 + x++;
	res += "x = 1, (5 + x++) = " + to_string(y) + "\n";
	y = 5 + x--;
	res += "x = 2, (5 + x--) = " + to_string(y) + "\n";

	y = 5 + ++x;
	res += "x = 1, (5 + ++x) = " + to_string(y) + "\n";
	y = 5 + --x;
	res += "x = 2, (5 + --x) = " + to_string(y) + "\n";
    
    return res;
}

int main()
{
    string filename = "unary_test.mdl";
    Interpreter interpreter(filename);
	ostringstream oss;
	streambuf* ogBuf = cout.rdbuf(oss.rdbuf());

	interpreter.interpret();
	cout.rdbuf(ogBuf);
	string output = oss.str();

    assert(output == expectedOutput());
	return 0;
}
