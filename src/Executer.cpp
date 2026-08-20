#include <iostream>
#include "Tables.hpp"
#include "Executer.hpp"

using namespace std;


//___________________________________________MODEL LANGUAGE PROGRAM EXECUTER___________________________________________
void Executer::executionError(string errMessage)
{
	cout << "[EXECUTION ERROR] " << errMessage << endl;
	exit(1);
}
	
void Executer::executionWarning(string err)
{
	try
	{
		throw err;
	}
	catch(string s)
	{
		cout << "[EXECUTION WARNING] " << s << endl;
	}
}

Executer::Executer() {}
	
void Executer::executeWrite()
{
	if (!typesStack.empty())
	{
		double arg;
		string strConst;
		lexemeType currType;
		extract(typesStack, currType);
		switch (currType)
		{
			case LEX_STRING:
				extract(strConstsStack, strConst);
				break;
				
			case LEX_INT: case LEX_REAL: case LEX_BOOL:
				extract(args, arg);
				break;
				
			default:
				break;
		}
		executeWrite();
		switch (currType)
		{
			case LEX_STRING:
				cout << strConst;
				break;
			
			case LEX_BOOL:
				arg ? cout << "true" : cout << "false";
				break;
			
			case LEX_INT: case LEX_REAL:
				cout << arg;
				break;
				
			default:
				break;
		}
	}
}

void Executer::executeProgram(vector<Lexeme> &RPNTable)
{
    int arg1;
	int arg2;
	double numValue1;
	double numValue2;
    string strConst1;
	string strConst2;

	/*
	cout << "\nRPN Table:\n";
	int i = 0;
	for (auto & rpn : RPNTable)
	{
		cout << i << ": " << rpn << "\n";
		i++;
	}
	*/
	
	int tableIndex = 0;
    while (tableIndex < RPNTable.size())
    {	
		Lexeme currLex = RPNTable[tableIndex];
        switch (currLex.getType())
        {
			case RPN_LABEL:
                args.push(currLex.getValue());
                break;

			case RPN_SWITCH:
			{
				arg1 = args.top();
				args.pop();
				typesStack.pop();
				vector<pair<int, int>> switchLabelVec = switchLabelTable.at(currLex.getValue());
				auto it = find_if(switchLabelVec.begin() + 1, switchLabelVec.end(), [arg1](pair<int, int> valAndLabel){
					return valAndLabel.first == idTable[arg1].getValue();
				});
				if (it == switchLabelVec.end())
				{
					it = switchLabelVec.begin();
				}
				args.push(it->second);
				break;
			}
                
			case RPN_ADDRESS:
				args.push(currLex.getValue());
				typesStack.push(idTable[currLex.getValue()].getType());
				break;
				
			case LEX_INT_NUM:
				args.push(currLex.getValue());
				typesStack.push(LEX_INT);
				break;

			case LEX_REAL_NUM:
				args.push(currLex.getValue());
				typesStack.push(LEX_REAL);
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
                if (idTable[arg1].isAssigned())
                {
					typesStack.push(idTable[arg1].getType());
					if (idTable[arg1].getType() == LEX_STRING)
					{
						strConstsStack.push(idTable[arg1].getStringValue());
					}
					else
					{
						args.push(idTable[arg1].getValue());
					}
				}
                else
				{
					executionError(
						"identifier \"" + idTable[arg1].getName() + "\" does not contain a value"
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
					extract(args, numValue1);
					extract(args, numValue2);
					args.push(numValue2 + numValue1);
				}
				typesStack.pop(); 
				break;
				
			case LEX_MINUS:
				extract(args, numValue1);
				extract(args, numValue2);
				args.push(numValue2 - numValue1);
				typesStack.pop();
				break;
 
            case LEX_TIMES:
                extract(args, numValue1);
                extract(args, numValue2);
                args.push(numValue2 * numValue1);
                typesStack.pop();
                break;
				
            case LEX_SLASH:
                extract(args, numValue1);
                extract(args, numValue2);
                typesStack.pop();
                if (numValue1 != 0)
				{
					args.push(numValue2 / numValue1);
				}
				else
				{
					executionError("dividing by zero is illegal");
				}
				break;
					
			case LEX_PERCENT:
                extract(args, arg1);
                extract(args, arg2);
                typesStack.pop();
                if (arg1 != 0)
				{
					args.push(arg2 % arg1);
				}
				else
				{
					executionError("dividing by zero is illegal");
				}
				break;
					
			case LEX_UNARY_MINUS:
				extract(args, numValue1);
				args.push(-1 * numValue1);
				break;
				
			case LEX_PP_PRE: case LEX_MM_PRE:
			{	
				extract(args, arg1);
				int argValue = idTable[arg1].getValue();
				int op = 1;
				if (currLex.getType() == LEX_MM_PRE)
				{
					op = -1;
				}
				args.push(argValue + op);
				idTable[arg1].setValue(argValue + op);
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
					extract(args, numValue1);
					extract(args, numValue2);
					args.push(numValue2 == numValue1);
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
					extract(args, numValue1);
					extract(args, numValue2);
					args.push(numValue2 != numValue1);
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
				}
				else
                {
					extract(args, numValue1);
					extract(args, numValue2);
					args.push(numValue2 < numValue1);
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
					extract(args, numValue1);
					extract(args, numValue2);
					args.push(numValue2 > numValue1);
				}
				typesStack.pop(); 
				typesStack.pop();
				typesStack.push(LEX_BOOL);
                break;
 
            case LEX_LESS_EQ:
                extract(args, numValue1);
				extract(args, numValue2);
				args.push(numValue2 <= numValue1);
                typesStack.pop(); 
				typesStack.pop();
				typesStack.push(LEX_BOOL);
                break;
 
            case LEX_GREATER_EQ:
                extract(args, numValue1);
				extract(args, numValue2);
				args.push(numValue2 >= numValue1);
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
						idTable[arg2].setValue(strConst1);
						break;
					
					case LEX_BOOL:
						extract(args, arg1);
						extract(args, arg2);
						if (arg1)
						{
							arg1 = 1;
						}
						idTable[arg2].setValue(arg1);
						break;
					
					case LEX_INT: case LEX_REAL:
						extract(args, numValue1);
						extract(args, arg2);
						//cout << "\n\narg1 = " << arg1 << "   arg2 = " << idTable[arg2].getValue() << "\narg1 value = " << idTable[arg1].getValue() << "\n\n\n";
						idTable[arg2].setValue(numValue1);
						break;
					
					default:
						break;
				}
				typesStack.pop();
				idTable[arg2].setAssign();
                break;
 
            case RPN_GO:
                extract(args, arg1);
                tableIndex = arg1 - 1;
                break;
 
            case RPN_GO_FALSE:
                extract(args, arg1);
                extract(args, arg2);
                typesStack.pop();
                if (!arg2)
				{
					tableIndex = arg1 - 1;
				}
				break;
 
            case LEX_WRITE:
				executeWrite();
				break;
				
			case LEX_WRITELINE:
				executeWrite();
				cout << endl;
				break;
 
			case LEX_READ:
				int inputIntVal;
				double inputRealVal;
				extract(args, arg1);
				switch (typesStack.top())
				{
					case LEX_INT:
						cin >> inputIntVal;
						idTable[arg1].setValue(inputIntVal);
						break;
					case LEX_REAL:
						cin >> inputRealVal;
						idTable[arg1].setValue(inputRealVal);
						break;
					case LEX_STRING:
						cin >> strConst1;
						idTable[arg1].setValue(strConst1);
						break;
					case LEX_BOOL:
						cin >> strConst2;
						if (
							strConst2 == "true" ||
							isdigit(strConst2[0]) && strConst2[0] - '0' ||
							(strConst2[0] == '+' || strConst2[0] == '-') && isdigit(strConst2[1]) && strConst2[1] - '0'
						) {
							idTable[arg1].setValue(1);
						}
						else
						{
							idTable[arg1].setValue(0);
						}
						break;
					default:
						break;
				}
				typesStack.pop();
				idTable[arg1].setAssign();
				break;

			default:
				executionError("unknown element");
				break;
		}
		tableIndex++;
	}
}