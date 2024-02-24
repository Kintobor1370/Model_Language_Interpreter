#include <iostream>
#include <stack>
#include "RPN_Generator.cpp"

using namespace std;


//___________________________________________MODEL LANGUAGE PROGRAM EXECUTER___________________________________________
class Executer
{
	Lexeme currLex;													// lexeme currently being executed
    
    stack<int> args;												// stack for int / bool arguement values
    stack<string> strConstsStack;									// stack for string constants
	stack<lexemeType> typesStack;									// stack for lexeme types
	
	// Execution error processing
	void executionError(string errMessage)
	{
		cerr << "EXECUTION ERROR: " << errMessage << endl;
		exit(1);
	}
	
	// Execution warning processing
	void executionWarning(string err)
	{
		try
		{
			throw err;
		}
		catch(string s)
		{
			cerr << "WARNING: " << s << endl << endl;
		}
	}
public:
	// Model program code execution
	void execute(vector<Lexeme>& RPNs);
	
	// Executing printing command
	void write()
	{
		if (!typesStack.empty())
		{
			int arg;
			string s;
			lexemeType currentType;
			extract(typesStack, currentType);
			switch (currentType)
			{
				case LEX_STRING:
					extract(strConstsStack, s);
					break;
				
				case LEX_INT: case LEX_BOOL:
					extract(args, arg);
					break;
				
				default:
					break;
			}
			write();
			switch (currentType)
			{
				case LEX_STRING:
					cout << s;
					break;
				
				case LEX_BOOL:
					arg ? cout << "true" : cout << "false";
					break;
				
				case LEX_INT:
					cout << arg;
					break;
				
				default:
					break;
			}
		}
	}
};

void Executer::execute(vector<Lexeme>& RPNs)
{
    int arg1;
	int arg2;
    string strConst1;
	string strConst2;
    
	int index = 0;
	int size = RPNs.size();
	
    cout << "Beginning execution...\n\n";
    
    while (index < size)
    {	
		currLex = RPNs[index];
        switch (currLex.getType())
        {
			case RPN_LABEL:
                args.push(currLex.getValue());
                break;
                
			case RPN_ADDRESS:
				args.push(currLex.getValue());
				typesStack.push(identTable[currLex.getValue()].getType());
				break;
				
			case LEX_NUM:
				args.push(currLex.getValue());
				typesStack.push(LEX_INT);
				break;
				
			case LEX_TRUE: case LEX_FALSE:
				args.push(currLex.getValue());
				typesStack.push(LEX_BOOL);
				break;
                
			case LEX_STR_CONST:
				strConstsStack.push(strConstTable[currLex.getValue()]);
				typesStack.push(LEX_STRING);
				break;
 
            case LEX_ID:
                arg1 = currLex.getValue();
                if (identTable[arg1].isAssigned())
                {
					typesStack.push(identTable[arg1].getType());
					if (identTable[arg1].getType() == LEX_STRING)
						strConstsStack.push(identTable[arg1].getStringValue());
					else
						args.push(identTable[arg1].getValue());
				}
                else
				{
					executionError(
						"the identificator \"" + identTable[arg1].getName() + "\" doesn't have a value"
					);
				}
				break;
				
            case LEX_NOT:
                extract(args, arg1);
                args.push(!arg1);
                break;
 
            case LEX_OR:
                extract(args, arg1); 
                extract(args, arg2);
                args.push(arg2 || arg1);
                typesStack.pop();
                break;
 
            case LEX_AND:
				extract(args, arg1);
                extract(args, arg2);
                args.push (arg2 && arg1);
                typesStack.pop();
                break;

			case LEX_PLUS:
				if (typesStack.top() == LEX_STRING)
				{
					extract(strConstsStack, strConst1);
					extract(strConstsStack, strConst2);
					strConstsStack.push(strConst2 + strConst1);
				}
				else
				{
					extract(args, arg1);
					extract(args, arg2);
					args.push(arg2 + arg1);
				}
				typesStack.pop(); 
				break;
				
			case LEX_MINUS:
				extract(args, arg1);
				extract(args, arg2);
				args.push(arg2 - arg1);
				typesStack.pop();
				break;
 
            case LEX_TIMES:
                extract(args, arg1);
                extract(args, arg2);
                args.push(arg2 * arg1);
                typesStack.pop();
                break;
				
            case LEX_SLASH:
                extract(args, arg1);
                extract(args, arg2);
                typesStack.pop();
                if (arg1)
					args.push(arg2 / arg1);
				else
					executionError("dividing by zero is illegal");
				break;
					
			case LEX_PERCENT:
                extract(args, arg1);
                extract(args, arg2);
                typesStack.pop();
                if (arg1)
					args.push(arg2 % arg1);
				else
					executionError("dividing by zero is illegal");
				break;
					
			case LEX_UNARY_MINUS:
				extract(args, arg1);
				args.push(-1 * arg1);
				break;
				
			case LEX_PP_PRE: case LEX_MM_PRE:
			{	
				extract(args, arg1);
				int argValue = identTable[arg1].getValue();
				int op = 1;
				if (currLex.getType() == LEX_MM_PRE)
					op = -1;
				args.push(argValue + op);
				identTable[arg1].setValue(argValue + op);
				break;
			}	
            case LEX_EQ:
				if (typesStack.top() == LEX_STRING)
				{
					extract(strConstsStack, strConst1);
					extract(strConstsStack, strConst2);
					args.push(strConst2 == strConst1);
				}
				else
                {
					extract(args, arg1);
					extract(args, arg2);
					args.push(arg2 == arg1);
				}
				typesStack.pop();
				typesStack.pop();
				typesStack.push(LEX_BOOL);
                break;
                
			case LEX_NOT_EQ:
                if (typesStack.top() == LEX_STRING)
				{
					extract(strConstsStack, strConst1);
					extract(strConstsStack, strConst2);
					args.push(strConst2 != strConst1);
				}
				else
                {
					extract(args, arg1);
					extract(args, arg2);
					args.push(arg2 != arg1);
				}
				typesStack.pop();
				typesStack.pop();
				typesStack.push(LEX_BOOL);
                break;
 
            case LEX_LESS:
				if (typesStack.top() == LEX_STRING)
				{
					extract(strConstsStack, strConst1);
					extract(strConstsStack, strConst2);
					args.push(strConst2 < strConst1);
					//cout << args.top();
				}
				else
                {
					extract(args, arg1);
					extract(args, arg2);
					args.push(arg2 < arg1);
				}
				typesStack.pop(); 
				typesStack.pop();
				typesStack.push(LEX_BOOL);
                break;
 
            case LEX_GREATER:
                if (typesStack.top() == LEX_STRING)
				{
					extract(strConstsStack, strConst1);
					extract(strConstsStack, strConst2);
					args.push(strConst2 > strConst1);
				}
				else
                {
					extract(args, arg1);
					extract(args, arg2);
					args.push(arg2 > arg1);
				}
				typesStack.pop(); 
				typesStack.pop();
				typesStack.push(LEX_BOOL);
                break;
 
            case LEX_LESS_EQ:
                extract(args, arg1);
                extract(args, arg2);
                args.push(arg2 <= arg1);
                typesStack.pop(); 
				typesStack.pop();
				typesStack.push(LEX_BOOL);
                break;
 
            case LEX_GREATER_EQ:
                extract(args, arg1);
                extract(args, arg2);
                args.push(arg2 >= arg1);
                typesStack.pop(); 
				typesStack.pop();
				typesStack.push(LEX_BOOL);
                break;
 
            case LEX_ASSIGN:
				typesStack.pop();
				switch (typesStack.top())
				{
					case LEX_STRING:
						extract(strConstsStack, strConst1);
						extract(args, arg2);
						identTable[arg2].setValue(strConst1);
						break;
					
					case LEX_BOOL:
						extract(args, arg1);
						extract(args, arg2);
						if (arg1)
							arg1 = 1;
						identTable[arg2].setValue(arg1);
						break;
					
					case LEX_INT:
						extract(args, arg1);
						extract(args, arg2);
						//cout << "\n\narg1 = " << arg1 << "   arg2 = " << identTable[arg2].getValue() << "\narg1 value = " << identTable[arg1].getValue() << "\n\n\n";
						identTable[arg2].setValue(arg1);
						break;
					
					default:
						break;
				}
				typesStack.pop();
				identTable[arg2].setAssign();
                break;
 
            case RPN_GO:
                extract(args, arg1);
                index = arg1 - 1;
                break;
 
            case RPN_FGO:
                extract(args, arg1);
                extract(args, arg2);
                typesStack.pop();
                if (!arg2)
					index = arg1 - 1;
                break;
 
            case LEX_WRITE:
				write();
				break;
				
			case LEX_WRITELINE:
				write();
				cout << endl;
				break;
 
			case LEX_READ:
				int inputValue;
				extract(args, arg1);
				switch (typesStack.top())
				{
					case LEX_INT:
						//cout << "\nEnter int value for " << identTable[arg1].getName() << ": ";
						cin >> inputValue;
						identTable[arg1].setValue(inputValue);
						break;
					case LEX_STRING:
						//cout << "\nEnter string value for " << identTable[arg1].getName() << ": ";
						cin >> strConst1;
						identTable[arg1].setValue(strConst1);
						break;
					case LEX_BOOL:
						//cout << "\nEnter bool value for " << identTable[arg1].getName() << ": ";
						cin >> strConst2;
						inputValue = 0;
						if (
							strConst2 == "true" ||
							isdigit(strConst2[0]) && strConst2[0] - '0' ||
							(strConst2[0] == '+' || strConst2[0] == '-') && isdigit(strConst2[1]) && strConst2[1] - '0'
						)
							inputValue = 1;
						identTable[arg1].setValue(inputValue);
						break;
					default:
						break;
				}
				typesStack.pop();
				identTable[arg1].setAssign();
				break;

			default:
				executionError("unknown element");
				break;
		}
		++index;
	}
	cout << "\nExecution complete!\n";
}