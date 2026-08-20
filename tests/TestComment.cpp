#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include "Interpreter.hpp"

using namespace std;

string expectedOutput()
{
    string res = "Uncommented section\nStart:\n";
	int x = 10;
	int y = 10;
	int z = 10;
	for (int i = 0; i < x; i++)
	{
		x = x - 2;
		y = y + 1;
	}
	res += "x = " + to_string(x) + "\n" +
		   "y = " + to_string(y) + "\n" +
		   "z = " + to_string(z) + "\n" +
		   "End\n" + "\n" +
		   "Commented section\nStart:\n";

//	res += "This is a commented section!\n";

    /*	
	if(x < 5)
		x = x + 1;
	else
		x = x - 1;
	res += "x = " + to_string(x) + "\n"
	       "y = " + to_string(y) + "\n"
		   "z = " + to_string(z) + "\n";
	*/
    res += "End\n";
    return res;
}

int main()
{
    string filename = "comment_test.mdl";
    Interpreter interpreter(filename);
	ostringstream oss;
	streambuf* ogBuf = cout.rdbuf(oss.rdbuf());

	interpreter.interpret();
	cout.rdbuf(ogBuf);
	string output = oss.str();

    assert(output == expectedOutput());
	return 0;
}
