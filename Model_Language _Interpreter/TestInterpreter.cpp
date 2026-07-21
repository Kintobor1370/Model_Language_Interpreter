#include <fstream>
#include <conio.h>
#include "Interpreter.cpp"

using namespace std;

string getFileName()
{ 
	string fileName;
	cout << "Enter code file name: ";
	cin >> fileName;
	string ext = fileName.substr(fileName.find(".") + 1);
	while (ext != "mdl")
	{
		cout << "Unsupported file extension \"." << ext << "\". Please try a different file.\n" << endl;
		cout << "Enter code file name: ";
		cin >> fileName;
		ext = fileName.substr(fileName.find(".") + 1);
	}
	ifstream f(fileName);
	if (!f.good())
	{
		cout << "Cannot open file \'" << fileName << "\'. Please try again.\n" << endl;
		fileName = getFileName();
	}
	transform(fileName.begin(), fileName.end(), fileName.begin(), [](char c){
		if (c == '/')
		{
			return '\\';
		}
		return c;
	});
	return fileName;
}

void getExpectedResults(int testNumber)
{
	int x, y, z, i, j;
	double d;

	//cout << "No lexical, syntax or semantic issues. Your program is flawless.\n";
	//cout << "Beginning execution...\n\n";
	switch (testNumber)
	{
		case 0:
			cout << "Hello, World!" << '\n';
			break;
		
		case 1:
			cout << "Enter any integer value: *your int here*" << "\n\n"
				 << "Enter any real value: *your real here*" << "\n\n"
				 << "Enter any string value: *your string here*" << "\n\n"
				 << "Enter any boolean value ('true' or 'false'): *your bool here*" << "\n\n"
				 << "Your integer value: *your int here*" << '\n'
				 << "Your real value: *your real here*" << '\n'
				 << "Your string value: *your string here*" << '\n'
				 << "Your boolean value: *your bool here*" << '\n';
			break;

		case 2:
			cout << "Logical addition:\n";
			cout << "  " << true << " OR " << true << " = " << (true || true) << "\n";
			cout << "  " << true << " OR " << false << " = " << (true || false) << "\n";
			cout << "  " << false << " OR " << true << " = " << (false || true) << "\n";
			cout << "  " << false << " OR " << false << " = " << (false || false) << "\n";

			cout << "\nLogical multiplication:\n";
			cout << "  " << true << " AND " << true << " = " << (true && true) << "\n";
			cout << "  " << true << " AND " << false << " = " << (true && false) << "\n";
			cout << "  " << false << " AND " << true << " = " << (false && true) << "\n";
			cout << "  " << false << " AND " << false << " = " << (false && false) << "\n";
			
			cout << "\nLogical reverse:\n";
			cout << "  not " << true << " = " << !true << '\n';
			cout <<"  not " << false << " = " << !false << '\n';
			break;
		
		case 3:
			x = 5;
			y = 10;
			
			if (x > y)
				cout << x << " is greater than " << y << '\n';
			else
				cout << x << " is less or equal than " << y << '\n';
			
			x = 10;
			if (x > y)
				cout << x << " is greater than " << y << '\n';
			else if (x < y)
				cout << x << " is less than " << y << '\n';
			else
				cout << x << " and " << y << " are equal" << '\n';
			break;

		case 4:
			for (x = 1; x <= 10; x++)
			{
				cout << "x = " << x << "\nCheck switch statement:\n   x ";
				switch(x)
				{
					case 1:
						cout << "= 1\n\n";
						break;
					
					case 2:
						cout << "= 2\n\n";
						break;
					
					case 3:
						cout << "= 3\n\n";
						break;
					
					case 4:
						cout << "= 4\n\n";
						break;
					
					case 5:
						cout << "= 5\n\n";
						break;
					
					default:
						cout << "> 5\n\n";
						break;
				}
			}
			break;

		case 5:
			x = 10;
			y = 10;
			while ((x <= 30) || (y > 5))
			{
				x = x + 2;
				y = y - 1;
			}
			cout << "x = " << x << "\ny = " << y << "\n";
			cout << "\n";

			while ((x >= 5) && (y < 20))
			{
				x = x - 2;
				y = y + 1;
			}
			cout << "x = " << x << "\ny = " << y << '\n';
			break;
		
		case 6:
			x = 10;
			y = 10;
			do
			{
				x = x + 2;
				y = y - 1;
			}
			while ((x <= 30) || (y > 5));
			cout << "x = " << x << "\ny = " << y << "\n";
			cout << "\n";

			do
			{
				x = x - 2;
				y = y + 1;
			}
			while ((x >= 5) && (y < 20));
			cout << "x = " << x << "\ny = " << y << '\n';
			break;

		case 7:
			x = 10;
			y = 10;
			for(int i=0; i<x; i++)
			{
				x = x - 2;
				y = y + 1;
				cout << "Iteration " << i << ": x = " << x << ", y = " << y << "\n";
			}
			cout << "Final result: x = " << x << ", y = " << y << "\n";
			break;

		case 8:
			x = 10;
			y = 10;
			i = 0;
			while (i < x)
			{
				x = x - 2;
				y = y + 1;
				cout << "Iteration " << i << ": x = " << x << ", y = " << y << "\n";
				++i;
			}
			cout << "Final result: x = " << x << ", y = " << y << "\n";
			break;

		case 9:
			x = 5;
			for (i=0; i<=9999; i++)
			{
				x = x * 2;
				if (i >= 5)
					break;
			}
			cout << "x = " << x << "\n";
			cout << "i = " << i << "\n";
			cout << "\n";

			x = 0; 
			for (j=0; j<10; j++)
			{
				x = x + 2;
				for (i=0; i<=9999; i++)
				{
					x = x + 2;
					if (i >= 5)
						break;
				}
			}
			cout << "x = " << x << "\n";
			cout << "i = " << i << "\n";
			cout << "j = " << j << "\n";
			break;

		case 10:
			for (int i = 0; i < 10; i++)
			{
				cout << "   i = " << i << "\n";
				if (i > 5)
				{
					continue;
				}
				i++;
			}
			break;

		case 11:
			x = 5;
			y = 7;
			z = 8;
			goto Label;

		Label:
			x++;
			y--;
			cout << "x = " << x << "\ny = " << y << "\n\n";
			x++;
			if (x < 10)
				goto Label;	
			break;

		case 12:
			cout << "Uncommented section\nStart:\n";
			x = 10;
			y = 10;
			z = 10;
			for(int i=0; i<x; i++)
			{
				x = x - 2;
				y = y + 1;
			}
			cout << "x = " << x << "\ny = " << y << "\nz = " << z << '\n';
			cout << "End\n\n";
			cout << "Commented section\nStart:\n";

			//	cout << "This is a commented section!\n";

			/*	
			if(x < 5)
				x = x + 1;
			else
				x = x - 1;
			cout << "x = " << x << "\ny = " << y << "\nz = " << z << '\n';
			*/

			cout << "End\n";
			break;

		case 13:
			x = 1;
			x++; 
			cout << "x = 1, x++ = "<< x << "\n";
			x--;
			cout << "x = 2, x-- = "<< x << "\n";

			++x;
			cout << "x = 1, ++x = "<< x << "\n";
			--x;
			cout << "x = 2, --x = "<< x << "\n";
			cout << "\n";

			y = 5 + x++;
			cout << "x = 1, (5 + x++) = "<< y << "\n";
			y = 5 + x--;
			cout << "x = 2, (5 + x--) = "<< y << "\n";

			y = 5 + ++x;
			cout << "x = 1, (5 + ++x) = "<< y << "\n";
			y = 5 + --x;
			cout << "x = 2, (5 + --x) = "<< y << "\n";
			break;

		default:
			break;
	}
}


int main()
{
	string programName;
	vector<string> tests = {
		"tests\\write_test.mdl",
		"tests\\read_test.mdl",
		"tests\\bool_test.mdl",
		"tests\\if_test.mdl",
		"tests\\switch_test.mdl",
		"tests\\while_test.mdl",
		"tests\\do_while_test.mdl",
		"tests\\for_test.mdl",
		"tests\\for_step_test.mdl",
		"tests\\break_test.mdl",
		"tests\\continue_test.mdl",
		"tests\\goto_test.mdl",
		"tests\\comment_test.mdl",
		"tests\\unary_test.mdl"
	};
	bool isTest = false;
	int testIndex;
	char input;
	
	system("cls");
	programName = getFileName();
	auto it = find(tests.begin(), tests.end(), programName);
	if (it != tests.end())
	{
		isTest = true;
		testIndex = distance(tests.begin(), it);
	}
	system("cls");
	if (isTest)
	{
		cout << "==========================================TEST " << testIndex << "=========================================\n\n";
		cout << ".....................................Expexted result.....................................\n";
		
		getExpectedResults(testIndex);
		
		cout << "\n......................................Actual result......................................\n";
		
		Interpreter interpreter(programName);
		interpreter.interpret();
		
		cout << "\n=========================================================================================\n\n";
	}
	else
	{
		Interpreter interpreter(programName);
		interpreter.interpret();
	}
	cout << "Press any key to finish";
	input = getch();
	system("cls");
	return 0;
}
