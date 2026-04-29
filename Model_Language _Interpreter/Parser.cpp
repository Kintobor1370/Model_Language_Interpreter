#include <iostream>
#include <stack>
#include "Scanner.cpp"

using namespace std;


/*____________________________________SYNTAX STRUCTURE OF A MODEL LANGUAGE PROGRAM_____________________________________
 * Legend:
 *	1) (A | B) = (A OR B)
 *  2) (A; [ B | C | D ]) = (A; B) OR (A; C) OR (A; D)
 * 	3) (A <; B>) = (A) OR (A; B)
 *  4) ({ A; B }) = code block of operations A and B
 *
 * 
 * Program header:			HEADER   	-->  program { CODE_BLOCK }
 *
 * Descriptions:			DESCS		--> DESC; DESCS | DESC; | eps
 * Description				DESC		--> [int | real | string | bool] VAR <, VAR>			
 * Variable					VAR	    	--> LEX_ID | LEX_ID = CONST														
 * Constant parameter		CONST   	--> INT | STR | BOOL
 *
 * Operations				CODE_BLOCK	--> <OP>
 * Operation				OP       	--> DESCS | OP_STMNT | { OPS } | if (STMNT) OP <else OP> | while (STMNT) OP | do OP while (STMNT); |
 * 										 	for ([STMNT]; [STMNT]; [STMNT]) OP | break; | goto LABEL; | read(ID); | write (STMNT <, STMNT>);
 * Statement operator		OP_STMNT 	--> STMNT | ID = STMNT;
 * Statement				STMNT    	--> ADD | ADD = STMNT | ADD [==|<|>|<=|>=|!=] ADD 
 * Additive state			ADD		 	--> MULTI | MULTI [+ | - | or] MULTI
 * Multiplicative state		MULTI	 	--> FIN | FIN [ * | / | and] FIN
 * Final state 				FIN		 	--> ID | LABEL: | ID++ | ID-- | ++ID | --ID | [+ | -] FIN | STR | BOOL | not FIN | STMNT
 */


//___________________________________________REVERSE POLISH NOTATION PARSER____________________________________________
class Parser
{
    Scanner scanner;                                                    // Lexical scanner
	vector<Lexeme> RPNTable;                                            // Reverse Polish Notation (RPN) table (vectorised)
    
    stack<lexemeType> lexStack;
    Lexeme lex;                                                         // Current lexeme
	lexemeType type;                                                    // Current lexeme's type
	int val;                                                            // Current lexeme's value
	
	bool loopState;														// Indicator that the program iscurrently in loop state
	int nestedLoopsCount;												// Number of nested loops (-1 : no loop state; 0 : no nested loops; >= 1 : >= 1 nested loops)
	bool isLvalue;														// Indicator that the current identifier is isLvalue
	
	struct breakStackItem
	{
		int nestedLoopNumber;
		int position;
	};
	stack<breakStackItem> breakStack;						    		// Stack for break operators (Stack item consists of label's position in RPN and number of a nested loop containing the break operator)
	
	stack<int> plusStack;
	stack<int> minusStack;
	stack<int> lvalueUncertainStack;
	int num;
	
	bool isAssignment;													// Indicator that current operation is an assignment

	OperationTable opTable;
	
	// Syntax actions
	void HEADER();														// Program's header

	void DESCS();														// Descriptions
	void DESC();														// Description		
	void VAR();															// Variable													
	void CONST();														// Constant parameter

	void CODE_BLOCK();													// Code block of the program
	void OP();															// Operator
	void OP_STMNT();													// Statement operator
	void STMNT(bool operand=true);										// Statement
	void ADD();															// Additive state
	void MULTI();														// Multiplicative state
	void FIN();															// Final state
	
	// Semantic actions
	void setVar();
	void idCheck(int value);
	void identReadCheck();
	void operationCheck();
	void unaryOperationCheck();
	void notCheck();
	void assignEqualTypeCheck();
	void conditionEqualTypeCheck();
	void breakControllerOn();
	void breakControllerOff();
	void breakCheck();
	void gotoCheck();
	
	// Convert unary operation to RPN
	void unaryOperationToRPN();
	
	// Get the next lexeme
	void getLexeme()
	{
		lex = scanner.getLexeme();          							// The scanner gets a lexeme
		type = lex.getType();					                		// Get type of the lexeme
		val = lex.getValue();					        		        // Get value of the lexeme
		//cout << lex << "\n";
	}
	
	// Syntax error processing
	void syntaxError(int errNumber, string err)
	{
		try
		{
			throw err;
		}
		catch(string s)
		{
			cout << "[SYNTAX ERROR #" << errNumber << "] " << s << "\nLexeme: " << lex << endl;
			exit(1);
		}
	}
	
	// Semantic error processing
	void semanticError(string err)
	{
		try
		{
			throw err;
		}
		catch(string s)
		{
			cout << "[SEMANTIC ERROR] " << s << endl;
			exit(1);
		}
	}
	
	// Semantic warning processing
	void semanticWarning(string err)
	{
		try
		{
			throw err;
		}
		catch(string s)
		{
			cerr << "[WARNING] " << s << endl << endl;
		}
	}
	
public:
	Parser(const string fileName): scanner(fileName), opTable()
	{
		loopState = 0;
		nestedLoopsCount = -1;
		isLvalue = true;
		isAssignment = false;
	}

    vector<Lexeme> getRPNTable()
    {
        return RPNTable;
    }
	
	void analyse();
};


template <class T1, class T2>

// Extract item from stack
void extract(T1& stack, T2& item)
{
	item = stack.top();
	stack.pop();
}


void Parser::analyse()
{
	clearTables();
	RPNTable.clear();

	getLexeme();
	HEADER();
	if (type != LEX_EOF)
	{
		syntaxError(													// Syntax error #1
			1,
			"No end of file found...wait, how is this even possible?"
		);
	}
	//cout << "No lexical, syntax or semantic issues. Your program is flawless." << '\n';
}

//.........................SYNTAX ANALYSIS
// Analyse code starting from the program's header
void Parser::HEADER()
{
	if (type == LEX_PROGRAM)								    		// if first lexeme is program's header:
	{
		getLexeme();													//   get next lexeme
		if (type == LEX_LEFT_BRACE)										// if next lexeme is left brace:
		{
			getLexeme();												//   get next lexeme
			CODE_BLOCK();												//   analyse code block
			gotoCheck();
		}
		else															// else:
		{
			syntaxError(3, "Did you forget '{'?");						// Syntax error #3
		}
	}
	else 																// else:
	{
		syntaxError(2, "Did you forget 'program'?");					// Syntax error #2
	}
}

// Descriptions analysis
void Parser::DESCS()
{
	DESC();
	if (type != LEX_SEMICOLON)						    				// if lexeme following the description is not semicolon:
	{
		syntaxError(4, "Did you forget ';'?");							// Syntax error #4
	}
	getLexeme();
}

// Single description analysis
void Parser::DESC()
{ 
	lexemeType identType = type;						    			// save the type of variable to be described further (to assing a value of the same type when required)
	lexStack.push(identType);			    							// push this type to lexemes stack
	getLexeme();
	VAR();																// analyse the variable
	while (type == LEX_COMMA)			    							// while next lexeme is "," (i.e. while variables of the same type are being declared)
	{
		lexStack.push(identType);		    							//   push said type to lexemes stack once again
		getLexeme();
		VAR();															//   analyse next variable of the same type
	}
}

// Variable analysis
void Parser::VAR()
{
	if (type == LEX_ID)										
	{
		setVar();														// assign this lexeme its type (recently saved in lexemes stack)
		RPNTable.push_back(Lexeme(RPN_ADDRESS, val));					// add the lexeme to the RPN table
		getLexeme();
		
		if(type == LEX_ASSIGN)											// if the identifier above is being assigned a constant value
		{
			isAssignment = true;
            getLexeme();
            CONST();													//   analyse the constant value
            RPNTable.push_back(LEX_ASSIGN);
        }
		else
		{
			RPNTable.pop_back();
		}

		if (type == LEX_COMMA || type == LEX_SEMICOLON)		            // if the next lexeme is comma or semicolon:
		{
			lexStack.pop(); 											//		remove variable type from the lexemes stack 
		}
		else
		{
			syntaxError(5, "Wrong deliminator. Only '=' is allowed");	// Syntax error #5
		}
	}
	else																// if the lexeme is NOT an identifier:		
	{
		syntaxError(6, "No identifier found");							// Syntax error #6
	}
}

// Constant value analysis
void Parser::CONST()
{
	auto currType = lexStack.top();
	if (currType == LEX_INT || currType == LEX_REAL || currType == LEX_STRING || currType == LEX_BOOL)
	{
        STMNT(false);
        assignEqualTypeCheck();
    }
	else
	{
		syntaxError(7, "No matching const type found");					// Syntax error #7
	}
}

// Analysis of multiple operators
void Parser::CODE_BLOCK()
{
	while (type != LEX_RIGHT_BRACE)
	{
		OP();
		if (type == LEX_EOF)
		{
			syntaxError(8, "Did you forget '}'?");						// Syntax error #8
		}
	}
	getLexeme();
}

// Analysis of a single operator
void Parser::OP()
{
	int pos0;
	int pos1;
	int pos2;
	int pos3;
	int pos4;
	
	switch (type)
	{
		case LEX_INT: case LEX_REAL: case LEX_BOOL: case LEX_STRING:	// Description of identifiers
			DESCS();
			break;
			
		case LEX_IF:													// if() operator
			getLexeme();

			if (type == LEX_LEFT_PAREN)
			{
				getLexeme();
				STMNT();
				conditionEqualTypeCheck();

				pos2 = RPNTable.size();
				RPNTable.push_back(Lexeme());
				RPNTable.push_back(Lexeme(RPN_FGO));
				
				if (type == LEX_RIGHT_PAREN)
				{
					getLexeme();
					OP();
					RPNTable[pos2] = Lexeme(RPN_LABEL, RPNTable.size());
					
					if (type == LEX_ELSE)
					{
						pos3 = RPNTable.size();
						RPNTable.push_back(Lexeme());
						RPNTable.push_back(Lexeme(RPN_GO));
						RPNTable[pos2] = Lexeme(RPN_LABEL, RPNTable.size());
						getLexeme();
						OP();
						RPNTable[pos3] = Lexeme(RPN_LABEL, RPNTable.size());
					}
				}
				else
				{
					syntaxError(										// Syntax error #10
						10,
						"'if' expression: did you forget ')' ?"
					);
				}
			}
			else
			{
				syntaxError(											// Syntax error #9
					9,
					"'if' expression: expected '(' after 'if'"
				);
			}
			break;
			/*
			if (type != LEX_LEFT_PAREN)
			{
				syntaxError(											// Syntax error #9
					9,
					"'if' expression: expected '(' after 'if'"
				);
			}	
			getLexeme();
			STMNT();
			conditionEqualTypeCheck();

			pos2 = RPNTable.size();
			RPNTable.push_back(Lexeme());
			RPNTable.push_back(Lexeme(RPN_FGO));
			
			if (type != LEX_RIGHT_PAREN)
			{
				syntaxError(											// Syntax error #10
					10,
					"'if' expression: did you forget ')' ?"
				);
			}
			getLexeme();
			OP();
			RPNTable[pos2] = Lexeme(RPN_LABEL, RPNTable.size());
			
			if (type == LEX_ELSE)
			{
				pos3 = RPNTable.size();
				RPNTable.push_back(Lexeme());
				RPNTable.push_back(Lexeme(RPN_GO));
				RPNTable[pos2] = Lexeme(RPN_LABEL, RPNTable.size());
                getLexeme();
				OP();
				RPNTable[pos3] = Lexeme(RPN_LABEL, RPNTable.size());
			}
			break;*/
		
		case LEX_WHILE:													// while() loop
			pos0 = RPNTable.size();
			getLexeme();

			if (type == LEX_LEFT_PAREN)
			{
				getLexeme();
				STMNT();
				conditionEqualTypeCheck();
				if (type == LEX_RIGHT_PAREN)
				{
					pos1 = RPNTable.size(); 
					RPNTable.push_back(Lexeme());
					RPNTable.push_back(Lexeme(RPN_FGO));
					
					breakControllerOn();								// break operators processing in RPN table is also done via breakController (0 = off, 1 = on)
					getLexeme();
					OP();
					
					RPNTable.push_back(Lexeme(RPN_LABEL, pos0));
					RPNTable.push_back(Lexeme(RPN_GO));
					RPNTable[pos1] = Lexeme(RPN_LABEL, RPNTable.size());
					
					breakControllerOff();
				}
				else
				{
					syntaxError(										// Syntax error #12
						12,
						"'while' expression: did you forget ')' ?"
					);
				}
			}
			else
			{
				syntaxError(											// Syntax error #11
					11,
					"'while' expression: expected '(' after 'while'"
				);
			}
			break;
		
		case LEX_DO:
			pos0 = RPNTable.size();
			breakControllerOn();	
			getLexeme();
			OP();
            breakControllerOff();

			if (type == LEX_WHILE)
			{
				getLexeme();
				if (type == LEX_LEFT_PAREN)
				{
					getLexeme();
					STMNT();
					conditionEqualTypeCheck();	
					if (type == LEX_RIGHT_PAREN)
					{
						getLexeme();
						if (type == LEX_SEMICOLON)
						{
							pos1 = RPNTable.size(); 
							RPNTable.push_back(Lexeme());
							RPNTable.push_back(Lexeme(RPN_FGO));
							RPNTable.push_back(Lexeme(RPN_LABEL, pos0));
							RPNTable.push_back(Lexeme(RPN_GO));
							RPNTable[pos1] = Lexeme(RPN_LABEL, RPNTable.size());
							getLexeme();
						}
						else
						{
							syntaxError(								// Syntax error #40
								40,
								"'do-while' expression: did you forget ';' ?"
							);
						}
					}
					else
					{
						syntaxError(									// Syntax error #39
							39,
							"'while' expression: did you forget ')' ?"
						);
					}
				}
				else
				{
					syntaxError(										// Syntax error #38
						38,
						"'do-while' expression: expected '(' after 'while'"
					);
				}
			}
			else
			{
				syntaxError(											// Syntax error #37
					37,
					"'do-while' expression: expected 'while' after the code block"
				);
			}
			break;

		case LEX_FOR:													// for loops
			getLexeme();

			if (type == LEX_LEFT_PAREN)
			{															// for(;;) loop
				getLexeme();

				// for(<analysing this part>; ...; ...)
				if (type != LEX_SEMICOLON)
				{
					if (type == LEX_INT || type == LEX_REAL || type == LEX_BOOL || type == LEX_STRING)
					{
						DESCS();										// the first part of 'for' loop initialisation can be either a variable declaration (always assigning it a cretain value)
					}
					else
					{
						OP_STMNT();										// or a statement operator
					}
				}
				else 
				{
					getLexeme();
				}
				pos3 = RPNTable.size();
				
				// for(...; <analysing this part>; ...)
				if (type != LEX_SEMICOLON)	
				{
					STMNT();
					conditionEqualTypeCheck();
					if (type != LEX_SEMICOLON)
					{
						syntaxError(										// Syntax error #14
							14, 
							"'for' expression: ';' between last two statements is missing"
						);
					}
				}
				else
				{
					RPNTable.push_back(Lexeme(LEX_TRUE, 1));
				} 
				
				pos1 = RPNTable.size();
				RPNTable.push_back(Lexeme());
				RPNTable.push_back(Lexeme(RPN_FGO));
				
				pos2 = RPNTable.size();
				RPNTable.push_back(Lexeme());
				RPNTable.push_back(Lexeme(RPN_GO));
				pos4 = RPNTable.size();
				
				//    for(...; ...; <analysing this part>)
				getLexeme();
				if (type != LEX_RIGHT_PAREN)
				{
					isAssignment = false;
					STMNT();
					if (type != LEX_RIGHT_PAREN)
					{
						syntaxError(										// Syntax error #15
							15, 
							"'for' expression: did you forget ')' ?"
						);
					}

					RPNTable.push_back(Lexeme(RPN_LABEL, pos3));
					RPNTable.push_back(Lexeme(RPN_GO));
				}
				getLexeme();
				RPNTable[pos2] = Lexeme(RPN_LABEL, RPNTable.size());

				breakControllerOn();
				OP();
				
				RPNTable.push_back(Lexeme(RPN_LABEL, pos4));
				RPNTable.push_back(Lexeme(RPN_GO));
				RPNTable[pos1] = Lexeme(RPN_LABEL, RPNTable.size());
				
				breakControllerOff();
			}
			else
			{																// for-step-until loop
				// for <analysing this part> step ... until ... do
				if (type != LEX_STEP)
				{
					if (type == LEX_INT || type == LEX_REAL || type == LEX_BOOL || type == LEX_STRING)
					{
						DESCS();											// the first part of a 'for' loop initialisation can be either a variable declaration
					}														// (always assigning it a cretain value)
					else
					{
						STMNT();											// or a statement
					}
					if (type == LEX_STEP)
					{
						getLexeme();
					}
					else
					{
						syntaxError(										// Syntax error #14
							14, 
							"'for' expression: did you forget 'step' ?"
						);
					}
				}
				pos0 = RPNTable.size();
				RPNTable.push_back(Lexeme());
				RPNTable.push_back(Lexeme(RPN_GO));
				pos1 = RPNTable.size();

				// for ... step <analysing this part> until ... do
				if (type == LEX_UNTIL)	
				{
					getLexeme();
				} 
				else
				{
					isAssignment = false;
					STMNT();
					if (type == LEX_UNTIL)
					{
						getLexeme();
					}
					else
					{
						syntaxError(										// Syntax error #14
							14, 
							"'for' expression: 'until' between last two statements is missing"
						);
					}
				}
				
				// for ... step ... until <analysing this part> do
				if (type == LEX_DO)
				{
					RPNTable.push_back(Lexeme(LEX_TRUE, 1));
					getLexeme();
				}
				else
				{
					isAssignment = false;
					STMNT();
					conditionEqualTypeCheck();
					if (type == LEX_DO)
					{
						getLexeme();
					}
					else
					{
						syntaxError(										// Syntax error #15
							15, 
							"'for' expression: did you forget 'do' ?"
						);
					}
				}

				pos2 = RPNTable.size();
				RPNTable.push_back(Lexeme());
				RPNTable.push_back(Lexeme(RPN_FGO));
				
				pos3 = RPNTable.size();
				RPNTable.push_back(Lexeme());
				RPNTable.push_back(Lexeme(RPN_GO));

				RPNTable[pos0] = Lexeme(RPN_LABEL, RPNTable.size());
				RPNTable[pos2] = Lexeme(RPN_LABEL, RPNTable.size());
				
				breakControllerOn();
				OP();
				
				RPNTable.push_back(Lexeme(RPN_LABEL, pos1));
				RPNTable.push_back(Lexeme(RPN_GO));
				RPNTable[pos3] = Lexeme(RPN_LABEL, RPNTable.size());
				
				breakControllerOff();
			}
			break;
		
		case LEX_BREAK:													// break operator
			breakCheck();
			getLexeme();
			if (type != LEX_SEMICOLON)
			{
				syntaxError(16, "did you forget ';' ?");				// Syntax error #16
			}
			getLexeme();
			break;
		
		case LEX_GOTO:													// goto operator
            getLexeme();
			if (type == LEX_ID)
			{
				if (!idTable[val].isLabel())				        	// if the identifier is not declared as label:
				{
					if (!idTable[val].isDeclared())	    				//   if the identifier is not declared at all:
					{
						idTable[val].setAsLabel();						//      set it as a label (implying this label was not present before in the code)
						idTable[val].setAddress(RPNTable.size());
						RPNTable.push_back(Lexeme());
						RPNTable.push_back(Lexeme(RPN_GO));
					}
					else 												//   else: the identifier is already declared as a variable => error
					{
						syntaxError(									// Syntax error #18
							18, 
							"the identifier has already been declared"
						);
					}
				}
				else 													// else: the identifier has already been declared as label
				{														//   i.e. this label was present in the code before
					int value = idTable[val].getValue();
					RPNTable.push_back(Lexeme(RPN_LABEL, value));
					RPNTable.push_back(Lexeme(RPN_GO));
				}
				getLexeme();
				if (type == LEX_SEMICOLON)
				{
					getLexeme();
				}
				else
				{
					syntaxError(										// Syntax error #19
						19, 
						"\"goto\" operator: did you forget ';' ?"
					);
				}
			}
			else
			{
				syntaxError(											// Syntax error #17
					17, 
					"expected label after \"goto\" operator"
				);
			}
			/*
			if (type != LEX_ID)
			{
				syntaxError(											// Syntax error #17
					17, 
					"expected label after \"goto\" operator"
				);
			}
			if (!idTable[val].isLabel())				        		// if the identifier is not declared as label:
			{
				if (!idTable[val].isDeclared())	    					//   if the identifier is not declared at all:
				{
					idTable[val].setAsLabel();							//      set it as a label (implying this label was not present before in the code)
					idTable[val].setAddress(RPNTable.size());
					RPNTable.push_back(Lexeme());
					RPNTable.push_back(Lexeme(RPN_GO));
				}
				else 													//   else: the identifier is already declared as a variable => error
				{
					syntaxError(										// Syntax error #18
						18, 
						"the identifier has already been declared"
					);
				}
			}
			else 														// else: the identifier has already been declared as label
			{															//   i.e. this label was present in the code before
				int value = idTable[val].getValue();
				RPNTable.push_back(Lexeme(RPN_LABEL, value));
				RPNTable.push_back(Lexeme(RPN_GO));
			}
			getLexeme();
			if (type != LEX_SEMICOLON)
			{
				syntaxError(											// Syntax error #19
					19, 
					"\"goto\" operator: did you forget ';' ?"
				);
			}
			getLexeme();
			*/
			break;
		
		case LEX_READ:													// read() operator
			getLexeme();
			if (type == LEX_LEFT_PAREN)
			{
				getLexeme();
				if (type == LEX_ID)
				{
					identReadCheck();
					RPNTable.push_back(Lexeme(RPN_ADDRESS, val));
					getLexeme();
					if (type == LEX_RIGHT_PAREN)
					{
						getLexeme();
						RPNTable.push_back(Lexeme(LEX_READ));
						if (type == LEX_SEMICOLON)
						{
							getLexeme();
						}
						else
						{
							syntaxError(23, "Did you forget ';' ?");	// Syntax error #23
						}
					}
					else
					{
						syntaxError(									// Syntax error #22
							22, 
							"'read' expression: did you forget ')' ?"
						);
					}
				}
				else
				{
					syntaxError(										// Syntax error #21
						21, 
						"'read' expression: identifier not found"
					);
				}
			}
			else
			{
				syntaxError(											// Syntax error #20
					20, 
					"'read' expression: expected '(' after 'read'"
				);
			}
			/*
			if (type != LEX_LEFT_PAREN)
			{
				syntaxError(											// Syntax error #20
					20, 
					"'read' expression: expected '(' after 'read'"
				);
			}
			getLexeme();
			if (type != LEX_ID)
			{
				syntaxError(											// Syntax error #21
					21, 
					"'read' expression: identifier not found"
				);
			}
			identReadCheck();
			RPNTable.push_back(Lexeme(RPN_ADDRESS, val));
			getLexeme();
			
			if (type != LEX_RIGHT_PAREN)
			{
			    syntaxError(											// Syntax error #22
					22, 
					"'read' expression: did you forget ')' ?"
				);
			}
			getLexeme();
			RPNTable.push_back(Lexeme(LEX_READ));
				
			if (type != LEX_SEMICOLON)
			{
				syntaxError(23, "Did you forget ';' ?");				// Syntax error #23
			}
			getLexeme();
			*/
			break;
		
		case LEX_WRITE:	case LEX_WRITELINE:								// write() and writeline() operators
		{
			lexemeType writeMode = type;
			getLexeme();
			if (type == LEX_LEFT_PAREN)
			{
				getLexeme();
				if (type != LEX_RIGHT_PAREN)
				{
					STMNT(false);
					while (type == LEX_COMMA)
					{
						getLexeme();
						STMNT(false);
					}

					if (type == LEX_RIGHT_PAREN)
					{
						getLexeme();
						RPNTable.push_back(Lexeme(writeMode));
						if (type == LEX_SEMICOLON)
						{
							getLexeme();
						}
						else
						{
							syntaxError(											// Syntax error #27
								27,
								"Did you forget ';' ?"
							);
						}
					}
					else
					{
						syntaxError(											// Syntax error #26
							26,
							"'write' expression: did you forget ')' ?"
						);
					}
				}
				else
				{
					syntaxError(											// Syntax error #25
						25,
						"'write' expression: identifier not found"
					);
				}
			}
			else
			{
				syntaxError(											// Syntax error #24
					24, 
					"'write' expression: expected '(' after 'write'"
				);
			}
			
			/*
			if (type != LEX_LEFT_PAREN)
			{
				syntaxError(											// Syntax error #24
					24, 
					"'write' expression: expected '(' after 'write'"
				);
			}
			getLexeme();
			if (type == LEX_RIGHT_PAREN)
			{
				syntaxError(											// Syntax error #25
					25,
					"'write' expression: identifier not found"
				);
			}
			STMNT(false);
			while (type == LEX_COMMA)
			{
				getLexeme();
				STMNT(false);
			}

			if (type != LEX_RIGHT_PAREN)
			{
				syntaxError(											// Syntax error #26
					26,
					"'write' expression: did you forget ')' ?"
				);
			}
			getLexeme();
			RPNTable.push_back(Lexeme(writeMode));
			if (type != LEX_SEMICOLON)
			{
				syntaxError(											// Syntax error #27
					27,
					"Did you forget ';' ?"
				);
			}
			getLexeme();
			*/
			break;
		}
		case LEX_LEFT_BRACE:											// Composite operator
			getLexeme();
			CODE_BLOCK();
			break;
		
		default:														// Statement operator
			OP_STMNT();
			break;
	}
}

// Statement operator analysis
void Parser::OP_STMNT()
{
	isAssignment = false;
	STMNT();
	if (type == LEX_SEMICOLON || type == LEX_COLON)
	{
		getLexeme();
	}
	else
	{
		syntaxError(28, "Did you forget ';' ?");						// Syntax error #28
	}
}

// Statement analysis
void Parser::STMNT(bool operand)
{
	isLvalue = operand;
	lexemeType assignedType = type;  									// save the type of lvalue lexeme (in case of assigning variable of a different type)
	ADD();

	if (type == LEX_ASSIGN)												// if assignment takes place:
	{
		if (assignedType == LEX_ID && isLvalue)							//   check that before assignment was lvalue statement identifier
		{
			isAssignment = true;
			int lvalueUncertain;
			extract(lvalueUncertainStack, lvalueUncertain);
			RPNTable[num] = Lexeme(RPN_ADDRESS, lvalueUncertain);
			getLexeme();
			STMNT();
			assignEqualTypeCheck();
			RPNTable.push_back(LEX_ASSIGN);
		}
		else
		{
			syntaxError(												// Syntax error #29
				29, 
				"Lvalue required as a left operand of assignment"
			);
		}
	}
	else if (type >= LEX_EQ && type <= LEX_NOT_EQ)
	{
		lexemeType compareType = type;
		
		isLvalue = false;
		lexStack.push(type); 
		getLexeme();
		ADD();
		operationCheck();
		RPNTable.push_back(Lexeme(compareType));
	}
	
	unaryOperationToRPN();
	if (!lvalueUncertainStack.empty())
	{
		int lvalueUncertain;
		extract(lvalueUncertainStack, lvalueUncertain);
		
		RPNTable[num] = isLvalue ? Lexeme(RPN_ADDRESS, lvalueUncertain) : Lexeme(LEX_ID, lvalueUncertain);
	}
}

void Parser::ADD()
{
	MULTI();
	while (type == LEX_PLUS || type == LEX_MINUS || type == LEX_OR)
	{
		lexemeType additionType = type;
		
		isLvalue = false;
		lexStack.push(type);
		getLexeme();
		MULTI();
		operationCheck();
		RPNTable.push_back(Lexeme(additionType));
	}
}

void Parser::MULTI()
{
	FIN();
	while (type == LEX_TIMES || type == LEX_SLASH || type == LEX_PERCENT || type == LEX_AND)
    {
		lexemeType multiplicationType = type;
		
		isLvalue = false;
		lexStack.push(type);
		getLexeme();
		FIN();
        operationCheck();
		RPNTable.push_back(Lexeme(multiplicationType));
	}
}

void Parser::FIN()
{
	switch (type)
	{
		case LEX_ID:
		{
			if (isLvalue)
			{
				lvalueUncertainStack.push(val);
				num = RPNTable.size();
				RPNTable.push_back(Lexeme());
			}
			else
			{
				RPNTable.push_back(Lexeme(LEX_ID, val));
			}

			int idIndex = val;
            getLexeme();
			if (type == LEX_COLON)					    				// if ':' goes after the identifier:
			{
				if (!isAssignment)
				{														//   it means that identifier is a label
					if (isLvalue)
					{
						lvalueUncertainStack.pop();
					}
					RPNTable.pop_back();
					
					if (idTable[idIndex].isLabel())						//   if an identifier was declared as label before:					
					{
						int pos = idTable[idIndex].getAddress();		//     pos - label's address in the code
						if (idTable[idIndex].getValue() != -1)			//	   if this label has already been assigned a value:
						{												//       the label was placed twice within the code => error
							syntaxError(								// Syntax error #30
								30, 
								"Label \"" + idTable[idIndex].getName() + "\" is declared twice"
							);
						}
						idTable[idIndex].setValue(RPNTable.size());		//     assign the location the label will lead to
						idTable[idIndex].setAssign();					//     confirm that label has been assigned a value
						RPNTable[pos] = Lexeme(RPN_LABEL, RPNTable.size());
					}
					else if (!idTable[idIndex].isDeclared())			//     if an identifier was not declared as label:		
					{
						idTable[idIndex].setAsLabel();					//       declare the identifier as label
						idTable[idIndex].setValue(RPNTable.size());		//       assign the location where label will lead to
						idTable[idIndex].setAssign();					//       confirm that label has been assigned a value
					}
					else
					{	
						syntaxError(									// Syntax error #31
							31,
							"Label \"" + idTable[idIndex].getName() + "\" is already declared as an identifier and cannot be used"
						);
					}
				}
				else
				{
					syntaxError(										// Syntax error #32
						32,
						"Wrong usage of label \"" + idTable[idIndex].getName() + "\""
					);
				}
			}
			else if (type == LEX_PLUS_PLUS || type == LEX_MINUS_MINUS)
			{
				if (!isAssignment)
				{
					RPNTable.pop_back();
					if (isLvalue)
					{
						lvalueUncertainStack.pop();
					}
				}
				isLvalue = false;
				type == LEX_PLUS_PLUS ? plusStack.push(idIndex) : minusStack.push(idIndex);
				getLexeme();
			}
			idCheck(idIndex);
			break;
		}	
		case LEX_INT_NUM:
			lexStack.push(LEX_INT);
			RPNTable.push_back(lex);
			getLexeme();
			break;
		
		case LEX_REAL_NUM:
			lexStack.push(LEX_REAL);
			RPNTable.push_back(lex);
			getLexeme();
			break;
		
		case LEX_PLUS:
			isLvalue = false;
			getLexeme();
			FIN();
			unaryOperationCheck();
			break;
		
		case LEX_MINUS:
			isLvalue = false;
			getLexeme();
			FIN();
			unaryOperationCheck();
			RPNTable.push_back(Lexeme(LEX_UNARY_MINUS));
			break;
			
		case LEX_PLUS_PLUS: case LEX_MINUS_MINUS:
			isLvalue = false;
			lexemeType unaryOpType;
			unaryOpType = type == LEX_PLUS_PLUS ? LEX_PP_PRE : LEX_MM_PRE;// save unary operation's type to add it to RPN table
			
			getLexeme();
			if (type == LEX_ID)
			{
				idCheck(val);
				unaryOperationCheck();
				if (!isAssignment)
				{
					RPNTable.push_back(Lexeme(RPN_ADDRESS, val));
					RPNTable.push_back(Lexeme(LEX_ID, val));
					RPNTable.push_back(Lexeme(LEX_INT_NUM, 1));
					
					unaryOpType == LEX_PP_PRE ?
						RPNTable.push_back(Lexeme(LEX_PLUS)) :
						RPNTable.push_back(Lexeme(LEX_MINUS));
					
					RPNTable.push_back(Lexeme(LEX_ASSIGN));
				}
				else
				{
					RPNTable.push_back(Lexeme(RPN_ADDRESS, val));
					RPNTable.push_back(Lexeme(unaryOpType));
				}
				getLexeme();
			}
			else
			{
				syntaxError(											// Syntax error #33
					33, 
					"Lvalue requied as an increment operand"
				);
			}
			break;
		
		case LEX_QUOTE:
			getLexeme();
			lexStack.push(LEX_STRING);
            RPNTable.push_back(lex);
			if (type != LEX_STR_CONST)
			{
				syntaxError(34, "No string constant found");			// Syntax error #34
			}
			getLexeme();												// get the finishing quote (if it is missing lexical error will be triggered)
			getLexeme();
			break;
		
		case LEX_TRUE: case LEX_FALSE:
			lexStack.push(LEX_BOOL);									// true and false are bool => put bool in the lexemes stack
			type == LEX_TRUE ? 
				RPNTable.push_back(Lexeme(LEX_TRUE, 1)) :
				RPNTable.push_back(Lexeme(LEX_FALSE, 0));
			getLexeme();
			break;
		
		case LEX_NOT:
			isLvalue = false;
			getLexeme();
			FIN();
			notCheck();
			RPNTable.push_back(Lexeme(LEX_NOT));
			break;
		
		case LEX_LEFT_PAREN:
			getLexeme();
			STMNT(false);
			if (type != LEX_RIGHT_PAREN)
			{
				syntaxError(35, "Did you forget ')' ?");				// Syntax error #35
			}
			getLexeme();
			break;
		
		default:
			cout << "LEXEME: " << lex << "\n";
			syntaxError(36, "No matching operand found");				// Syntax error #36
			break;
	}
}


//.........................SEMANTIC ANALYSIS
// Set the variable's type and check its declaration status
void Parser::setVar()
{
	if (idTable[val].isDeclared())							    		// if the variable has already been declared before:
	{
		semanticError(													//   semantic error
			"Variable \"" + idTable[val].getName() + "\" is declared twice"
		);
	}
	else 																// else:
	{
		idTable[val].setType(lexStack.top());		    				//   assign the variable its type (which is kept in the end of the lexemes stack)
		idTable[val].setDeclare();				    					//   confirm the variable has been declared
	}
}

// Check whether identifier was declared or not
void Parser::idCheck(int value)
{
	if(idTable[value].isDeclared())										// if declared:
	{
		lexStack.push(idTable[value].getType());						//   add it to the lexemes stack
	}	
	else																// else:
	{
		semanticError(													//   semantic error
			"Variable \"" + idTable[value].getName() + "\" has not been declared"
		);
	}
}

// Check the identifier's declaration in read()
void Parser::identReadCheck()
{
	if(!idTable[val].isDeclared())
	{
		semanticError(
			"in 'read()' function: Variable \"" + idTable[val].getName() + "\" has not been declared"
		);
	}
}

// Single operation check
void Parser::operationCheck()
{
	lexemeType opLeft;													// left operand 
	lexemeType opRight;													// right operand
	lexemeType oper;													// operator

	extract(lexStack, opRight);
	extract(lexStack, oper);
	extract(lexStack, opLeft);

	lexemeType resType = opTable.getResultType(opLeft, opRight, oper);	// operation result type
	if (resType == LEX_NULL)
	{
		cout << opLeft << " " << opRight << " " << oper << "\n";
		semanticError("Variable types in the operation do not match");
	}
	else
	{
		lexStack.push(resType);
	}
}

void Parser::unaryOperationCheck()
{
	int opType = lexStack.top();										// operand's type is kept at the end of the lexemes stack
	if(opType != LEX_INT)												// if operand's type is not integer:
	{
		semanticError("Wrong type for unary operation");				//   semantic error
	}
}

// 'not' operator check
void Parser::notCheck()
{
	lexemeType opType = lexStack.top();
	if(opType != LEX_BOOL)
	{
		semanticError("Wrong type in 'not' statement");
	}
}

// Check of equality of variable type and statement type before assignment
void Parser::assignEqualTypeCheck()
{
	lexemeType typeRight;												// statement (or right variable) type 
	extract(lexStack, typeRight);										// extract it from the lexemes stack
	if (																// NOTE: it is allowed to assign integer values to bool variables
		lexStack.top() != typeRight &&
		(lexStack.top() != LEX_BOOL || typeRight != LEX_INT) &&
		(lexStack.top() != LEX_REAL || typeRight != LEX_INT)
	) {
		semanticError("The types do not match " + to_string(lexStack.top()) + " " + to_string(typeRight));
	}
}

// Check of statement type in conditions of if() / while() / for(;;) / do-while()
void Parser::conditionEqualTypeCheck()
{
	if(lexStack.top() == LEX_BOOL)										// must be bool
	{
		lexStack.pop();
	}
	else
	{
		semanticError("The expression is not boolean");
	}
}

// Enable break controller
void Parser::breakControllerOn()
{
	loopState = 1;														//   the code is in loop state
    nestedLoopsCount++;													//   number of nested loops may also increase
}

void Parser::breakControllerOff()
{
	if (!nestedLoopsCount)												// if a standard loop was finished and not a nested one:
	{
		loopState = 0;											    	//   the code is out of loop state
	}
	breakStackItem item;												// if a loop (standard or nested) has a break operator in it, then break stack keeps the nested
	while (!breakStack.empty())											// loop number, from where break was called, and a position of its label in the RPN table
	{																	// thus, if break stack is not empty:
		extract(breakStack, item);										//   extract the number of nested loop and label's postion in RPN table
		if (item.nestedLoopNumber == nestedLoopsCount)
		{
			RPNTable[item.position] = Lexeme(RPN_LABEL, RPNTable.size());//	 assign end of the loop as a transfer location for this label
		}
		else
		{
            breakStack.push(item);
            break;
        }
	}
	nestedLoopsCount--;
}

// Checking break
void Parser::breakCheck()
{
	if(loopState)														// if the code is in loop state:
	{
		int pos = RPNTable.size();
		breakStackItem newItem {nestedLoopsCount, pos};
		breakStack.push(newItem);										//   push break's position in RPN into stack
		RPNTable.push_back(Lexeme());										//   add empty lexeme (will be assigned transfer location later) to RPN table
		RPNTable.push_back(Lexeme(RPN_GO));								    //   add transfer lexeme to RPN table
	}
	else																// else:
	{																	//    semantic error
		semanticError("'break' can only be used inside loops");
	}
}

// Checking goto operation
void Parser::gotoCheck()
{
	for (auto &id : idTable)
	{
		if(id.isLabel())
		{
			if(id.isAssigned() && id.getAddress() == -1)
			{
				semanticWarning("label \""+ id.getName() + "\" declared, but not used");
			}
			if(!id.isAssigned() && id.getAddress() != -1)
			{
				semanticError("label \""+ id.getName() + "\" used, but not declared");
			}
		}
	}
}

// Converting unary operation to RPN
void Parser::unaryOperationToRPN()
{
	int value;
	while (!plusStack.empty())
	{
		extract(plusStack, value);
	    RPNTable.push_back(Lexeme(RPN_ADDRESS, value));
		RPNTable.push_back(Lexeme(LEX_ID, value));
		RPNTable.push_back(Lexeme(LEX_INT_NUM, 1));
		RPNTable.push_back(Lexeme(LEX_PLUS));
		RPNTable.push_back(Lexeme(LEX_ASSIGN));
	}
	while (!minusStack.empty())
	{
		extract(minusStack, value);
		RPNTable.push_back(Lexeme(RPN_ADDRESS, value));
		RPNTable.push_back(Lexeme(LEX_ID, value));
		RPNTable.push_back(Lexeme(LEX_INT_NUM, 1));
		RPNTable.push_back(Lexeme(LEX_MINUS));
		RPNTable.push_back(Lexeme(LEX_ASSIGN));
	}
}