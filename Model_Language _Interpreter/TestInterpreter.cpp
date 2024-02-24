#include <iostream>
#include <conio.h>
#include "Interpreter.cpp"

using namespace std;

void getExpectedResults(int testNumber)
{
	int x, y, z, i, j;

	cout << "No lexical, syntax or semantic issues. Your program is flawless.\n";
	cout << "Beginning execution...\n\n";
	switch (testNumber)
	{
		case 0:
			cout << "Hello, World!" << '\n';
			break;
		
		case 1:
			cout << "Enter any integer value: *your int here*" << "\n\n"
				 << "Enter any string value: *your string here*" << "\n\n"
				 << "Enter any boolean value: *your bool here*" << "\n\n"
				 << "Your integer value: *your int here*" << '\n'
				 << "Your string value: *your string here*" << '\n'
				 << "Your boolean value: *your bool here*" << '\n';
			break;

		case 2:
			cout << "Logical addition:\n";
			cout << "  " << true << " OR " << true << " = " << true || true << '\n';
			cout << "  " << true << " OR " << false << " = " << true || false << '\n';
			cout << "  " << false << " OR " << true << " = " << false || true << '\n';
			cout << "  " << false << " OR " << false << " = " << false || false << '\n';

			cout << "\nLogical multiplication:\n";
			cout << "  " << true << " AND " << true << " = " << true && true << '\n';
			cout << "  " << true << " AND " << false << " = " << true && false << '\n';
			cout << "  " << false << " AND " << true << " = " << false && true << '\n';
			cout << "  " << false << " AND " << false << " = " << false && false << '\n';
			
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
			x = 10;
			y = 10;
			while ((x <= 30) || (y > 5))
			{
				x = x + 2;
				y = y - 1;
			}
			cout << "x = " << x << "\ny = " << y << "\n\n";

			while ((x >= 5) && (y < 20))
			{
				x = x - 2;
				y = y + 1;
			}
			cout << "x = " << x << "\ny = " << y << '\n';
			break;
		
		case 5:
			x = 10;
			y = 10;
			for (int i=0; i<x; i++)
			{
				x = x - 2;
				y = y + 1;
			}
			cout << "x = " << x << "\ny = " << y << '\n';
			break;

		case 6:
			x = 5;
			i = 1;
			for (; i<=9999; i++)
			{
				x = x * 2;
				if (x > 100)
					break;
			}
			cout << "x = " << x << "\ni = " << i << '\n';
			break;

		case 7:
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

		case 8:
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

		default:
			break;
	}
	cout << "\nExecution complete!\n";
}


int main()
{
	string tests[] = {
		"tests\\write_test",
		"tests\\read_test",
		"tests\\bool_test",
		"tests\\if_test",
		"tests\\while_test",
		"tests\\for_test",
		"tests\\break_test",
		"tests\\goto_test",
		"tests\\comment_test"
	};
	int amount = 9;
	char input;

//=====================================EXPECTED RESULT=====================================
//======================================ACTUAL RESULT======================================

	for (int i=0; i<amount; i++)
	{
		system("cls");
		cout << "==========================================TEST " << i + 1 << "=========================================\n\n";
		cout << ".....................................Expexted result.....................................\n";
		
		getExpectedResults(i);
		
		cout << "\n......................................Actual result......................................\n";
		
		string testProgramName = tests[i];
		Interpreter interpreter(testProgramName);
		interpreter.interpret();
		
		cout << "\n=========================================================================================\n\n";
		cout << "Press any key to continue";
		input = getch();
	}
	system("cls");
	cout << "ALL TESTS COMPLETE\n\n";
	return 0;
}
