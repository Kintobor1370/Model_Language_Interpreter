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
 * Operation				OP       	--> DESCS | OP_STMNT | { CODE_BLOCK } | if (STMNT) OP <else OP> | switch (ID) { case VAL: CODE_BLOCK default: CODE_BLOCK } | 
 											while (STMNT) OP | do OP while (STMNT); | for ([STMNT]; [STMNT]; [STMNT]) OP | break; | goto LABEL; | read(ID); | write (STMNT <, STMNT>);
 * Statement operator		OP_STMNT 	--> STMNT | ID = STMNT;
 * Statement				STMNT    	--> ADD | ADD = STMNT | ADD [==|<|>|<=|>=|!=] ADD 
 * Additive state			ADD		 	--> MULTI | MULTI [+ | - | or] MULTI
 * Multiplicative state		MULTI	 	--> FIN | FIN [ * | / | and] FIN
 * Final state 				FIN		 	--> ID | LABEL: | ID++ | ID-- | ++ID | --ID | [+ | -] FIN | STR | BOOL | not FIN | STMNT
 */


//___________________________________________REVERSE POLISH NOTATION PARSER____________________________________________
class Parser
{
	vector<Lexeme> sourceCode;											// Source code converted to a vector of lexical tokens
	int count;															// Counter for the vector of lexical tokens
	vector<Lexeme> RPNTable;                                            // Reverse Polish Notation (RPN) table (vectorised)
    
    stack<lexemeType> lexStack;
	stack<int> loopInitPosStack;
    Lexeme currLex;                                                     // Current lexeme
	lexemeType currType;                                                // Type of the current lexeme
	int currVal;                                                        // Value of the current lexeme
	
	int nestedCodeBlocksCount;											// Number of nested code blocks (loops or switch) (-1 : no loop state; 0 : no nested code blocks; >= 1 : >= 1 nested code blocks)
	bool isLvalue;														// Indicator that the current identifier is isLvalue
	
	struct breakStackItem
	{
		int nestedCodeBlockNum;
		int position;
	};
	stack<breakStackItem> breakStack;						    		// Stack for break operators (Stack item consists of label's position in RPN and number of a nested loop containing the break operator)
	
	stack<int> plusStack;												// Stack for postfix '++' operations
	stack<int> minusStack;												// Stack for postfix '--' operations
	stack<int> lvalueUncertainStack;									// Stack of identifiers unconfirmed to be lvalue
	int lvalueUncertainIndex;											// Index of the identifiers unconfirmed to be lvalue in the RPN table
	
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
	void STMNT(bool lvalue = true);										// Statement
	void ADD();															// Additive state
	void MULTI();														// Multiplicative state
	void FIN();															// Final state
	
	// Semantic actions
	void setVar();
	void checkIdDeclared(int idIndex);
	void checkIdInRead();
	void checkOperationType();
	void checkUnaryOperation();
	void checkNot();
	void checkTypeInAssign();
	void checkTypeInCondition();
	void checkBreak();
	void checkGoto();
	void breakControllerOn();
	void breakControllerOff();
	
	// Convert unary operation to RPN (e.g.: x++ => x x 1 + =)
	void unaryOperationToRPN();
	
	// Get the next lexeme
	void getLexeme()
	{
		currLex = sourceCode.at(count);									// Get the current lexeme
		currType = currLex.getType();					                // Get type of the lexeme
		currVal = currLex.getValue();					        		// Get value of the lexeme
		count++;														// Move to the next lexeme
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
			cout << "[SYNTAX ERROR #" << errNumber << "] " << s << "\nLexeme: " << currLex << endl;
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
			cout << "[SEMANTIC WARNING] " << s << endl;
		}
	}
	
public:
	Parser(vector<Lexeme> sc)
	{
		sourceCode = std::move(sc);
		count = 0;
		nestedCodeBlocksCount = -1;
		isLvalue = true;
		isAssignment = false;
	}

    vector<Lexeme> getRPNTable()
    {
        return RPNTable;
    }
	
	void validateProgram();
};


template <class T1, class T2>

// Extract item from stack
void extract(T1& stack, T2& item)
{
	item = stack.top();
	stack.pop();
}


void Parser::validateProgram()
{
	getLexeme();
	HEADER();
	if (currType != LEX_EOF)
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
	if (currType == LEX_PROGRAM)								    	// if first lexeme is program's header:
	{
		getLexeme();													//   get next lexeme
		if (currType == LEX_LEFT_BRACE)									// if next lexeme is left brace:
		{
			getLexeme();												//   get next lexeme
			CODE_BLOCK();												//   analyse code block
			checkGoto();
		}
		else															// else:
		{
			syntaxError(2, "Did you forget '{'?");						// Syntax error #2
		}
	}
	else 																// else:
	{
		syntaxError(3, "Did you forget 'program'?");					// Syntax error #3
	}
}

// Variable descriptions analysis
void Parser::DESCS()
{
	DESC();
	if (currType != LEX_SEMICOLON)						    			// if lexeme following the variable description is not semicolon:
	{
		syntaxError(4, "Did you forget ';'?");							// Syntax error #4
	}
	getLexeme();
}

// Single variable description analysis
void Parser::DESC()
{ 
	lexemeType idType = currType;							    		// save the type of variable to be described further (to assing a value of the same type when required)
	lexStack.push(idType);				    							// push this type to lexemes stack
	getLexeme();
	VAR();																// analyse the variable
	while (currType == LEX_COMMA)			    						// while next lexeme is "," (i.e. while variables of the same type are being declared)
	{
		lexStack.push(idType);		    								//   push said type to lexemes stack once again
		getLexeme();	
		VAR();															//   analyse next variable of the same type
	}
}

// Variable analysis
void Parser::VAR()
{
	if (currType == LEX_ID)										
	{
		double idIndex = currVal;
		setVar();														// assign this lexeme its type (recently saved in lexemes stack)
		RPNTable.push_back(Lexeme(RPN_ADDRESS, currVal));				// add the lexeme to the RPN table
		getLexeme();
		
		if(currType == LEX_ASSIGN)										// if the identifier above is being assigned a constant value
		{
			isAssignment = true;
            getLexeme();
            CONST();													//   analyse the constant value
            RPNTable.push_back(LEX_ASSIGN);
			idTable[idIndex].setAssign();
        }
		else
		{
			RPNTable.pop_back();
		}

		if (
			currType == LEX_COMMA || 									// if the next lexeme is comma
			currType == LEX_SEMICOLON || 								// or semicolon
			currType == LEX_STEP										// or 'step' (required for 'for ... step ... until ... do' operator):
		) {
			lexStack.pop(); 											//   remove variable type from the lexemes stack 
		}
		else
		{
			syntaxError(5, "Illegal deliminator. Only '=' is allowed");	// Syntax error #5
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
	auto constType = lexStack.top();
	if (
		constType == LEX_INT || 
		constType == LEX_REAL || 
		constType == LEX_STRING || 
		constType == LEX_BOOL
	) {
        STMNT(false);
        checkTypeInAssign();
    }
	else
	{
		syntaxError(7, "No matching const type found");					// Syntax error #7
	}
}

// Analysis of multiple operators
void Parser::CODE_BLOCK()
{
	while (currType != LEX_RIGHT_BRACE)
	{
		OP();
		if (currType == LEX_EOF)
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
	
	switch (currType)
	{
		// Identifier descriptions
		case LEX_INT: case LEX_REAL: case LEX_BOOL: case LEX_STRING:
			DESCS();
			break;
			
		// Conditional statement
		case LEX_IF:
			getLexeme();

			if (currType == LEX_LEFT_PAREN)
			{
				getLexeme();
				STMNT();
				checkTypeInCondition();

				pos2 = RPNTable.size();
				RPNTable.push_back(Lexeme());
				RPNTable.push_back(Lexeme(RPN_GO_FALSE));
				
				if (currType == LEX_RIGHT_PAREN)
				{
					getLexeme();
					OP();
					RPNTable[pos2] = Lexeme(RPN_LABEL, RPNTable.size());
					
					if (currType == LEX_ELSE)
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
					syntaxError(										// Syntax error #9
						9,
						"'if' expression: did you forget ')' ?"
					);
				}
			}
			else
			{
				syntaxError(											// Syntax error #10
					10,
					"'if' expression: expected '(' after 'if'"
				);
			}
			break;

		// Switch - case statement:
		case LEX_SWITCH:
		{
			int insertIndex = RPNTable.size(); 
			int insertSize = 0;
			int stackSizeInit = breakStack.size();
			getLexeme();

			if (currType == LEX_LEFT_PAREN)
			{
				getLexeme();
				if (currType == LEX_ID)
				{
					Lexeme idLex = currLex;
					int idIndex = idLex.getValue();
					if (!idTable[idIndex].isAssigned())
					{
						syntaxError(									// Syntax error #49
							49,
							"Variable \"" + idTable[idIndex].getName() + "\" is not assigned a value."
						);
					}
					lexemeType idType = idTable[idIndex].getType();
					
					getLexeme();
					if (currType == LEX_RIGHT_PAREN)
					{
						int switchTableIndex = switchLabelTable.size();
						switchLabelTable.push_back(vector<pair<int, int>>{});
						// Add the switch RPN, containing the index in a switch label table
						RPNTable.push_back(Lexeme(RPN_ADDRESS, idLex.getValue()));
						RPNTable.push_back(Lexeme(RPN_SWITCH, switchTableIndex));
						RPNTable.push_back(Lexeme(RPN_GO));

						getLexeme();
						if (currType == LEX_LEFT_BRACE)
						{
							breakControllerOn();
							getLexeme();
							while (currType == LEX_CASE)
							{
								getLexeme();
								if ((idType == LEX_INT && currType == LEX_INT_NUM) || (idType == LEX_REAL && currType == LEX_REAL_NUM))
								{
									Lexeme valLex = currLex;
									getLexeme();
									if (currType == LEX_COLON)
									{
										switchLabelTable.at(switchTableIndex).emplace_back(valLex.getValue(), RPNTable.size());
										getLexeme();
										OP();
									}
									else
									{
										syntaxError(					// Syntax error #49
											49, 
											"case statement: missing ':'"
										);
									}
								}
								else
								{
									syntaxError(						// Syntax error #48
										48, 
										"case statement: value type mismatch"
									);
								}
							}

							if (currType == LEX_DEFAULT)
							{
								getLexeme();
								if (currType == LEX_COLON)
								{
									switchLabelTable.at(switchTableIndex).insert(
										switchLabelTable.at(switchTableIndex).begin(),
										make_pair(std::nan(""), RPNTable.size())
									);
									getLexeme();
									OP();

									if (currType == LEX_RIGHT_BRACE)
									{
										breakControllerOff();
										getLexeme();
									}
									else
									{
										syntaxError(					// Syntax error #52
											52, 
											"Switch statement: missing '}'"
										);
									}
								}
								else
								{
									syntaxError(						// Syntax error #51
											51, 
											"default case statement: missing ':'"
										);
								}
							}
							else
							{
								syntaxError(							// Syntax error #50
									50,
									"Switch statement: missing default case"
								);
							}
						}
						else
						{
							syntaxError(								// Syntax error #47
								47, 
								"Switch statement: missing '{'"
							);
						}
					}
					else
					{
						syntaxError(46, "Missing ')'");					// Syntax error #46
					}
				}
				else
				{
					syntaxError(										// Syntax error #45
						45, 
						"Expected identifer as an arguement of the 'switch' statement"
					);
				}
			}
			else
			{
				syntaxError(44, "Expected '(' after 'switch'");			// Syntax error #44
			}

			/*
			if (currType == LEX_LEFT_PAREN)
			{
				getLexeme();
				if (currType == LEX_ID)
				{
					Lexeme idLex = currLex;
					int idIndex = idLex.getValue();
					if (!idTable[idIndex].isAssigned())
					{
						syntaxError(									// Syntax error #49
							49,
							"Variable \"" + idTable[idIndex].getName() + "\" is not assigned a value."
						);
					}
					lexemeType idType = idTable[idIndex].getType();
					
					getLexeme();
					if (currType == LEX_RIGHT_PAREN)
					{
						getLexeme();
						if (currType == LEX_LEFT_BRACE)
						{
							breakControllerOn();
							getLexeme();
							while (currType == LEX_CASE)
							{
								getLexeme();
								if ((idType == LEX_INT && currType == LEX_INT_NUM) || (idType == LEX_REAL && currType == LEX_REAL_NUM))
								{
									Lexeme valLex = currLex;
									getLexeme();
									if (currType == LEX_COLON)
									{
										// Insert the RPN_GO conditions BEFORE the start of switch statement
										int currSize = RPNTable.size();
										RPNTable.insert(RPNTable.begin() + insertIndex, {
											idLex,
											valLex,
											Lexeme(LEX_NOT_EQ),
											Lexeme(RPN_LABEL, currSize),
											Lexeme(RPN_GO_FALSE)
										});
										insertSize += 5;

										// Shift each label lexeme by 5 (since 5 new lexemes were added before it)
										transform(RPNTable.begin() + insertIndex, RPNTable.end(), RPNTable.begin() + insertIndex, [](Lexeme l){
											if (l.getType() == RPN_LABEL)
											{
												return Lexeme(RPN_LABEL, l.getValue() + 5);
											}
											return l;
										});

										// Shift each break stack item position by 5
										stack<breakStackItem> tempStack;
										breakStackItem item;
										while (breakStack.size() != stackSizeInit)
										{
											extract(breakStack, item);
											item.position += 5;
											tempStack.push(item);
										}
										while (!tempStack.empty())
										{
											extract(tempStack, item);
											breakStack.push(item);
										}

										getLexeme();
										OP();
									}
									else
									{
										syntaxError(					// Syntax error #49
											49, 
											"case statement: missing ':'"
										);
									}
								}
								else
								{
									syntaxError(						// Syntax error #48
										48, 
										"case statement: value type mismatch"
									);
								}
							}

							if (currType == LEX_DEFAULT)
							{
								getLexeme();
								if (currType == LEX_COLON)
								{
									int currSize = RPNTable.size();
									RPNTable.insert(RPNTable.begin() + insertIndex + insertSize, {
										Lexeme(RPN_LABEL, currSize),
										Lexeme(RPN_GO)
									});
									transform(RPNTable.begin() + insertIndex, RPNTable.end(), RPNTable.begin() + insertIndex, [](Lexeme l){
										if (l.getType() == RPN_LABEL)
										{
											return Lexeme(RPN_LABEL, l.getValue() + 2);
										}
										return l;
									});
									stack<breakStackItem> tempStack;
									breakStackItem item;
									while (breakStack.size() != stackSizeInit)
									{
										extract(breakStack, item);
										item.position += 2;
											tempStack.push(item);
									}
									while (!tempStack.empty())
									{
										extract(tempStack, item);
										breakStack.push(item);
									}

									getLexeme();
									OP();

									if (currType == LEX_RIGHT_BRACE)
									{
										breakControllerOff();
										getLexeme();
									}
									else
									{
										syntaxError(					// Syntax error #52
											52, 
											"Switch statement: missing '}'"
										);
									}
								}
								else
								{
									syntaxError(						// Syntax error #51
											51, 
											"default case statement: missing ':'"
										);
								}
							}
							else
							{
								syntaxError(							// Syntax error #50
									50,
									"Switch statement: missing default case"
								);
							}
						}
						else
						{
							syntaxError(								// Syntax error #47
								47, 
								"Switch statement: missing '{'"
							);
						}
					}
					else
					{
						syntaxError(46, "Missing ')'");					// Syntax error #46
					}
				}
				else
				{
					syntaxError(										// Syntax error #45
						45, 
						"Expected identifer as an arguement of the 'switch' statement"
					);
				}
			}
			else
			{
				syntaxError(44, "Expected '(' after 'switch'");			// Syntax error #44
			}
			*/
			break;
		}
		
		// 'while()' loop
		case LEX_WHILE:
			pos0 = RPNTable.size();
			getLexeme();

			if (currType == LEX_LEFT_PAREN)
			{
				getLexeme();
				STMNT();
				checkTypeInCondition();
				if (currType == LEX_RIGHT_PAREN)
				{
					pos1 = RPNTable.size(); 
					RPNTable.push_back(Lexeme());
					RPNTable.push_back(Lexeme(RPN_GO_FALSE));
					
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
					syntaxError(										// Syntax error #11
						11,
						"'while' expression: did you forget ')' ?"
					);
				}
			}
			else
			{
				syntaxError(											// Syntax error #12
					12,
					"'while' expression: expected '(' after 'while'"
				);
			}
			break;
		
		// 'do { ... } while();' loop
		case LEX_DO:
			pos0 = RPNTable.size();
			breakControllerOn();	
			getLexeme();
			OP();
            breakControllerOff();

			if (currType == LEX_WHILE)
			{
				getLexeme();
				if (currType == LEX_LEFT_PAREN)
				{
					getLexeme();
					STMNT();
					checkTypeInCondition();	
					if (currType == LEX_RIGHT_PAREN)
					{
						getLexeme();
						if (currType == LEX_SEMICOLON)
						{
							pos1 = RPNTable.size(); 
							RPNTable.push_back(Lexeme());
							RPNTable.push_back(Lexeme(RPN_GO_FALSE));
							RPNTable.push_back(Lexeme(RPN_LABEL, pos0));
							RPNTable.push_back(Lexeme(RPN_GO));
							RPNTable[pos1] = Lexeme(RPN_LABEL, RPNTable.size());
							getLexeme();
						}
						else
						{
							syntaxError(								// Syntax error #13
								13,
								"'do-while' expression: did you forget ';' ?"
							);
						}
					}
					else
					{
						syntaxError(									// Syntax error #14
							14,
							"'while' expression: did you forget ')' ?"
						);
					}
				}
				else
				{
					syntaxError(										// Syntax error #15
						15,
						"'do-while' expression: expected '(' after 'while'"
					);
				}
			}
			else
			{
				syntaxError(											// Syntax error #16
					16,
					"'do-while' expression: expected 'while' after the code block"
				);
			}
			break;

		// 'for(...; ...; ..)' loop or 'for ... step ... until ... do' loop
		case LEX_FOR:
			getLexeme();

			// 'for(...; ...; ..)' loop
			if (currType == LEX_LEFT_PAREN)
			{
				getLexeme();

				// for(<analysing this part>; ...; ...)
				if (currType != LEX_SEMICOLON)
				{
					if (
						currType == LEX_INT || 
						currType == LEX_REAL || 
						currType == LEX_BOOL || 
						currType == LEX_STRING
					) {
						DESC();											// the first part of 'for' loop initialisation can be either a variable declaration (always assigning it a cretain value)
						if (currType == LEX_SEMICOLON)
						{
							getLexeme();
						}
						else
						{
							syntaxError(								// Syntax error #17
								17, 
								"'for' expression: ';' between first two statements is missing"
							);
						}
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
				if (currType != LEX_SEMICOLON)	
				{
					STMNT();
					checkTypeInCondition();
					if (currType != LEX_SEMICOLON)
					{
						syntaxError(									// Syntax error #18
							18, 
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
				RPNTable.push_back(Lexeme(RPN_GO_FALSE));
				
				pos2 = RPNTable.size();
				RPNTable.push_back(Lexeme());
				RPNTable.push_back(Lexeme(RPN_GO));
				pos4 = RPNTable.size();
				
				// for(...; ...; <analysing this part>)
				getLexeme();
				if (currType != LEX_RIGHT_PAREN)
				{
					loopInitPosStack.push(RPNTable.size());				// Push the initial position of the loop in the RPN table to the stack (for 'continue')
					isAssignment = false;
					STMNT();
					if (currType == LEX_RIGHT_PAREN)
					{
						RPNTable.push_back(Lexeme(RPN_LABEL, pos3));
						RPNTable.push_back(Lexeme(RPN_GO));
					}
					else
					{
						syntaxError(									// Syntax error #19
							19, 
							"'for' expression: did you forget ')' ?"
						);
					}
				}
				getLexeme();
				RPNTable[pos2] = Lexeme(RPN_LABEL, RPNTable.size());

				breakControllerOn();
				OP();
				
				RPNTable.push_back(Lexeme(RPN_LABEL, pos4));
				RPNTable.push_back(Lexeme(RPN_GO));
				RPNTable[pos1] = Lexeme(RPN_LABEL, RPNTable.size());
				
				breakControllerOff();
				loopInitPosStack.pop();
			}

			// 'for ... step ... until ... do' loop
			else
			{
				// for <analysing this part> step ... until ... do
				if (currType != LEX_STEP)
				{
					if (currType == LEX_INT || currType == LEX_REAL || currType == LEX_BOOL || currType == LEX_STRING)
					{
						DESC();											// the first part of a 'for' loop initialisation can be either a variable declaration
					}													// (always assigning it a cretain value)
					else
					{
						STMNT();										// or a statement
					}
					if (currType == LEX_STEP)
					{
						getLexeme();
					}
					else
					{
						syntaxError(									// Syntax error #20
							20, 
							"'for' expression: did you forget 'step' ?"
						);
					}
				}
				pos0 = RPNTable.size();
				RPNTable.push_back(Lexeme());
				RPNTable.push_back(Lexeme(RPN_GO));
				pos1 = RPNTable.size();
				loopInitPosStack.push(pos1);

				// for ... step <analysing this part> until ... do
				if (currType == LEX_UNTIL)	
				{
					getLexeme();
				} 
				else
				{
					isAssignment = false;
					STMNT();
					if (currType == LEX_UNTIL)
					{
						getLexeme();
					}
					else
					{
						syntaxError(									// Syntax error #21
							21, 
							"'for' expression: 'until' between last two statements is missing"
						);
					}
				}
				
				// for ... step ... until <analysing this part> do
				if (currType == LEX_DO)
				{
					RPNTable.push_back(Lexeme(LEX_TRUE, 1));
					getLexeme();
				}
				else
				{
					isAssignment = false;
					STMNT();
					checkTypeInCondition();
					if (currType == LEX_DO)
					{
						getLexeme();
					}
					else
					{
						syntaxError(									// Syntax error #22
							22, 
							"'for' expression: did you forget 'do' ?"
						);
					}
				}

				pos2 = RPNTable.size();
				RPNTable.push_back(Lexeme());
				RPNTable.push_back(Lexeme(RPN_GO_FALSE));
				
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
				loopInitPosStack.pop();
			}
			break;
		
		case LEX_BREAK:													// break operator
			checkBreak();
			getLexeme();
			if (currType == LEX_SEMICOLON)
			{
				getLexeme();
			}
			else
			{
				syntaxError(23, "did you forget ';' ?");				// Syntax error #23
			}
			break;

		case LEX_CONTINUE:
			if (!loopInitPosStack.empty())
			{
				int loopInitPos = loopInitPosStack.top();
				RPNTable.push_back(Lexeme(RPN_LABEL, loopInitPos));
				RPNTable.push_back(Lexeme(RPN_GO));
				getLexeme();
				if (currType == LEX_SEMICOLON)
				{
					getLexeme();
				}
				else
				{
					syntaxError(54, "did you forget ';' ?");			// Syntax error #54
				}
			}
			else
			{
				syntaxError(											// Syntax error #53
					53,
					"continue statement not within a 'for' loop"
				);
			}
			break;
		
		case LEX_GOTO:													// goto operator
            getLexeme();
			if (currType == LEX_ID)
			{
				int idIndex = currVal;
				if (!idTable[idIndex].isLabel())				        // if the identifier is not declared as label:
				{
					if (!idTable[idIndex].isDeclared())	    			//   if the identifier is not declared at all:
					{
						idTable[idIndex].setAsLabel();					//      set it as a label (implying this label was not present before in the code)
						idTable[idIndex].setAddress(RPNTable.size());
						RPNTable.push_back(Lexeme());
						RPNTable.push_back(Lexeme(RPN_GO));
					}
					else 												//   else: the identifier is already declared as a variable => error
					{
						syntaxError(									// Syntax error #24
							24, 
							"the identifier has already been declared"
						);
					}
				}
				else 													// else: the identifier has already been declared as a label
				{														// i.e. this label was present in the code before
					double labelValue = idTable[idIndex].getValue();
					RPNTable.push_back(Lexeme(RPN_LABEL, labelValue));
					RPNTable.push_back(Lexeme(RPN_GO));
				}
				getLexeme();
				if (currType == LEX_SEMICOLON)
				{
					getLexeme();
				}
				else
				{
					syntaxError(										// Syntax error #25
						25, 
						"\"goto\" operator: did you forget ';' ?"
					);
				}
			}
			else
			{
				syntaxError(											// Syntax error #26
					26, 
					"expected label after \"goto\" operator"
				);
			}
			break;
		
		case LEX_READ:													// read() operator
			getLexeme();
			if (currType == LEX_LEFT_PAREN)
			{
				getLexeme();
				if (currType == LEX_ID)
				{
					checkIdInRead();
					int idIndex = currVal;
					RPNTable.push_back(Lexeme(RPN_ADDRESS, currVal));
					getLexeme();
					if (currType == LEX_RIGHT_PAREN)
					{
						getLexeme();
						RPNTable.push_back(Lexeme(LEX_READ));
						idTable[idIndex].setAssign();
						if (currType == LEX_SEMICOLON)
						{
							getLexeme();
						}
						else
						{
							syntaxError(27, "Did you forget ';' ?");	// Syntax error #27
						}
					}
					else
					{
						syntaxError(									// Syntax error #28
							28, 
							"'read' expression: did you forget ')' ?"
						);
					}
				}
				else
				{
					syntaxError(										// Syntax error #29
						29, 
						"'read' expression: identifier not found"
					);
				}
			}
			else
			{
				syntaxError(											// Syntax error #30
					30, 
					"'read' expression: expected '(' after 'read'"
				);
			}
			break;
		
		case LEX_WRITE:	case LEX_WRITELINE:								// write() and writeline() operators
		{
			lexemeType writeMode = currType;
			getLexeme();
			if (currType == LEX_LEFT_PAREN)
			{
				getLexeme();
				if (currType != LEX_RIGHT_PAREN)
				{
					STMNT(false);
					while (currType == LEX_COMMA)
					{
						getLexeme();
						STMNT(false);
					}

					if (currType == LEX_RIGHT_PAREN)
					{
						getLexeme();
						RPNTable.push_back(Lexeme(writeMode));
						if (currType == LEX_SEMICOLON)
						{
							getLexeme();
						}
						else
						{
							syntaxError(								// Syntax error #31
								31,
								"Did you forget ';' ?"
							);
						}
					}
					else
					{
						syntaxError(									// Syntax error #32
							32,
							"'write' expression: did you forget ')' ?"
						);
					}
				}
				else
				{
					syntaxError(										// Syntax error #33
						33,
						"'write' expression: identifier not found"
					);
				}
			}
			else
			{
				syntaxError(											// Syntax error #34
					34, 
					"'write' expression: expected '(' after 'write'"
				);
			}
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
	if (currType == LEX_SEMICOLON || currType == LEX_COLON)
	{
		getLexeme();
	}
	else
	{
		syntaxError(35, "Did you forget ';' ?");						// Syntax error #35
	}
}

// Statement analysis
void Parser::STMNT(bool lvalue)
{
	isLvalue = lvalue;
	lexemeType assignedType = currType;  								// save the type of lvalue lexeme (in case of assigning variable of a different type)
	double assignedVal = currVal;										// save the value of lvalue lexeme 
	ADD();

	if (currType == LEX_ASSIGN)											// if assignment takes place:
	{
		if (assignedType == LEX_ID && isLvalue)							//   check that before assignment was lvalue statement identifier
		{
			isAssignment = true;
			int lvalueUncertain;
			extract(lvalueUncertainStack, lvalueUncertain);
			RPNTable[lvalueUncertainIndex] = Lexeme(RPN_ADDRESS, lvalueUncertain);
			getLexeme();
			STMNT();
			checkTypeInAssign();
			RPNTable.push_back(LEX_ASSIGN);
			idTable[assignedVal].setAssign();
		}
		else
		{
			syntaxError(												// Syntax error #36
				36, 
				"Lvalue required as a left operand of assignment"
			);
		}
	}
	else if (currType >= LEX_EQ && currType <= LEX_NOT_EQ)
	{
		lexemeType compareType = currType;
		
		isLvalue = false;
		lexStack.push(currType); 
		getLexeme();
		ADD();
		checkOperationType();
		RPNTable.push_back(Lexeme(compareType));
	}
	
	unaryOperationToRPN();
	if (!lvalueUncertainStack.empty())
	{
		int lvalueUncertain;
		extract(lvalueUncertainStack, lvalueUncertain);
		RPNTable[lvalueUncertainIndex] = isLvalue ? 
			Lexeme(RPN_ADDRESS, lvalueUncertain) : 
			Lexeme(LEX_ID, lvalueUncertain);
	}
}

// Additive state analysis
void Parser::ADD()
{
	MULTI();
	while (currType == LEX_PLUS || currType == LEX_MINUS || currType == LEX_OR)
	{
		lexemeType additionType = currType;
		isLvalue = false;
		lexStack.push(currType);
		getLexeme();
		MULTI();
		checkOperationType();
		RPNTable.push_back(Lexeme(additionType));
	}
}

// Multiplicative state analysis
void Parser::MULTI()
{
	FIN();
	while (currType == LEX_TIMES || currType == LEX_SLASH || currType == LEX_PERCENT || currType == LEX_AND)
    {
		lexemeType multiplicationType = currType;
		
		isLvalue = false;
		lexStack.push(currType);
		getLexeme();
		FIN();
        checkOperationType();
		RPNTable.push_back(Lexeme(multiplicationType));
	}
}

// Final state analysis
void Parser::FIN()
{
	switch (currType)
	{
		case LEX_ID:
		{
			if (isLvalue)
			{
				lvalueUncertainStack.push(currVal);
				lvalueUncertainIndex = RPNTable.size();
				RPNTable.push_back(Lexeme());
			}
			else
			{
				RPNTable.push_back(Lexeme(LEX_ID, currVal));
			}

			int idIndex = currVal;
            getLexeme();
			if (currType == LEX_COLON)					    			// if ':' goes after the identifier:
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
						if (idTable[idIndex].getValue() == -1)			//	   if this label has already been assigned a value:
						{
							idTable[idIndex].setValue(RPNTable.size());	//     assign the location the label will lead to
							idTable[idIndex].setAssign();				//     confirm that label has been assigned a value
							RPNTable[pos] = Lexeme(RPN_LABEL, RPNTable.size());
						}
						else											//   else:
						{												//     the label was placed twice within the code => error
							syntaxError(								// Syntax error #37
								37, 
								"Label \"" + idTable[idIndex].getName() + "\" is declared twice"
							);
						}
					}
					else if (!idTable[idIndex].isDeclared())			//     if an identifier was not declared as label:		
					{
						idTable[idIndex].setAsLabel();					//       declare the identifier as label
						idTable[idIndex].setValue(RPNTable.size());		//       assign the location where label will lead to
						idTable[idIndex].setAssign();					//       confirm that label has been assigned a value
					}
					else
					{	
						syntaxError(									// Syntax error #38
							38,
							"Label \"" + idTable[idIndex].getName() + "\" is already declared as an identifier and cannot be used"
						);
					}
				}
				else
				{
					syntaxError(										// Syntax error #39
						39,
						"Wrong usage of label \"" + idTable[idIndex].getName() + "\""
					);
				}
			}
			else if (currType == LEX_PLUS_PLUS || currType == LEX_MINUS_MINUS)
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
				currType == LEX_PLUS_PLUS ? plusStack.push(idIndex) : minusStack.push(idIndex);
				getLexeme();
			}
			checkIdDeclared(idIndex);
			break;
		}	
		case LEX_INT_NUM:
			lexStack.push(LEX_INT);
			RPNTable.push_back(currLex);
			getLexeme();
			break;
		
		case LEX_REAL_NUM:
			lexStack.push(LEX_REAL);
			RPNTable.push_back(currLex);
			getLexeme();
			break;
		
		case LEX_PLUS:
			isLvalue = false;
			getLexeme();
			FIN();
			checkUnaryOperation();
			break;
		
		case LEX_MINUS:
			isLvalue = false;
			getLexeme();
			FIN();
			checkUnaryOperation();
			RPNTable.push_back(Lexeme(LEX_UNARY_MINUS));
			break;
			
		case LEX_PLUS_PLUS: case LEX_MINUS_MINUS:
			isLvalue = false;
			lexemeType unaryOpType;
			unaryOpType = currType == LEX_PLUS_PLUS ? LEX_PP_PRE : LEX_MM_PRE;// save unary operation's type to add it to RPN table
			
			getLexeme();
			if (currType == LEX_ID)
			{
				checkIdDeclared(currVal);
				checkUnaryOperation();
				if (!isAssignment)
				{
					RPNTable.push_back(Lexeme(RPN_ADDRESS, currVal));
					RPNTable.push_back(Lexeme(LEX_ID, currVal));
					RPNTable.push_back(Lexeme(LEX_INT_NUM, 1));
					
					unaryOpType == LEX_PP_PRE ?
						RPNTable.push_back(Lexeme(LEX_PLUS)) :
						RPNTable.push_back(Lexeme(LEX_MINUS));
					
					RPNTable.push_back(Lexeme(LEX_ASSIGN));
				}
				else
				{
					RPNTable.push_back(Lexeme(RPN_ADDRESS, currVal));
					RPNTable.push_back(Lexeme(unaryOpType));
				}
				getLexeme();
			}
			else
			{
				syntaxError(											// Syntax error #40
					40, 
					"Lvalue requied as an increment operand"
				);
			}
			break;
		
		case LEX_QUOTE:
			getLexeme();
			if (currType == LEX_STR_CONST)
			{
				lexStack.push(LEX_STRING);
				RPNTable.push_back(currLex);
				getLexeme();											// get the finishing quote (if it is missing lexical error will be triggered)
				getLexeme();
			}
			else
			{
				syntaxError(41, "No string constant found");			// Syntax error #41
			}
			break;
		
		case LEX_TRUE: case LEX_FALSE:
			lexStack.push(LEX_BOOL);									// true and false are bool => put bool in the lexemes stack
			currType == LEX_TRUE ? 
				RPNTable.push_back(Lexeme(LEX_TRUE, 1)) :
				RPNTable.push_back(Lexeme(LEX_FALSE, 0));
			getLexeme();
			break;
		
		case LEX_NOT:
			isLvalue = false;
			getLexeme();
			FIN();
			checkNot();
			RPNTable.push_back(Lexeme(LEX_NOT));
			break;
		
		case LEX_LEFT_PAREN:
			getLexeme();
			STMNT(false);
			if (currType != LEX_RIGHT_PAREN)
			{
				syntaxError(42, "Did you forget ')' ?");				// Syntax error #42
			}
			getLexeme();
			break;
		
		default:
			//cout << "LEXEME: " << lex << "\n";
			syntaxError(43, "No matching operand found");				// Syntax error #43
			break;
	}
}


//.........................SEMANTIC ANALYSIS
// Set the variable's type and check its declaration status
void Parser::setVar()
{
	int idIndex = currVal;
	if (!idTable[idIndex].isDeclared())							    	// if the variable has not been declared before:
	{
		idTable[idIndex].setType(lexStack.top());		    			//   assign the variable its type (which is kept in the end of the lexemes stack)
		idTable[idIndex].setDeclare();				    				//   confirm the variable has been declared
	}
	else																// else:
	{
		semanticError(													//   semantic error
			"Variable \"" + idTable[idIndex].getName() + "\" is declared twice"
		);
	}
}

// Check whether identifier was declared or not
void Parser::checkIdDeclared(int idIndex)
{
	if (idTable[idIndex].isDeclared())									// if the identifier has been declared:
	{
		lexStack.push(idTable[idIndex].getType());						//   add it to the lexemes stack
	}	
	else																// else:
	{
		semanticError(													//   semantic error
			"Identifier \"" + idTable[idIndex].getName() + "\" has not been declared"
		);
	}
}

// Check declaration of an identifier in read()
void Parser::checkIdInRead()
{
	int idIndex = currVal;
	if (!idTable[idIndex].isDeclared())									// if the variable has not been declared before:
	{
		semanticError(													//   semantic error
			"in 'read()' function: Variable \"" + idTable[idIndex].getName() + "\" has not been declared"
		);
	}
}

// Single operation check
void Parser::checkOperationType()
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
		semanticError("Variable types in the operation do not match");
	}
	else
	{
		lexStack.push(resType);
	}
}

// Unary operation check
void Parser::checkUnaryOperation()
{
	int opType = lexStack.top();										// operand's type is kept at the end of the lexemes stack
	if(opType != LEX_INT)												// if operand's type is not integer:
	{
		semanticError("Wrong type for unary operation");				//   semantic error
	}
}

// 'not' operator check
void Parser::checkNot()
{
	lexemeType opType = lexStack.top();
	if(opType != LEX_BOOL)
	{
		semanticError("Wrong type in 'not' statement");
	}
}

// Check of equality of variable type and statement type before assignment
void Parser::checkTypeInAssign()
{
	lexemeType typeRight;												// statement (or right variable) type 
	extract(lexStack, typeRight);										// extract it from the lexemes stack
	if (																// NOTE: it is allowed to assign integer values to bool variables
		lexStack.top() != typeRight &&
		(lexStack.top() != LEX_BOOL || typeRight != LEX_INT) &&
		(lexStack.top() != LEX_REAL || typeRight != LEX_INT)
	) {
		semanticError("The types " + to_string(lexStack.top()) + " and " + to_string(typeRight) + " do not match ");
	}
}

// Check of statement type in conditions of if() / while() / for(;;) / do-while()
void Parser::checkTypeInCondition()
{
	if(lexStack.top() == LEX_BOOL)										// must be bool
	{
		lexStack.pop();
	}
	else
	{
		semanticError("The condition expression is not boolean");
	}
}

// Checking break
void Parser::checkBreak()
{
	if (nestedCodeBlocksCount > -1)										// if the code is in a nested code block:
	{
		int pos = RPNTable.size();
		breakStackItem newItem {nestedCodeBlocksCount, pos};
		breakStack.push(newItem);										//   push break's position in RPN into stack
		RPNTable.push_back(Lexeme());									//   add empty lexeme (will be assigned transfer location later) to RPN table
		RPNTable.push_back(Lexeme(RPN_GO));								//   add transfer lexeme to RPN table
	}
	else																// else:
	{																	//    semantic error
		semanticError("'break' can only be used inside loops");
	}
}

// Checking goto operation
void Parser::checkGoto()
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

// Enable break controller
void Parser::breakControllerOn()
{
    nestedCodeBlocksCount++;											//   increase number of nested code blocks
}

// Disable break controller
void Parser::breakControllerOff()
{
	breakStackItem item;												// if a loop (standard or nested) has a break operator in it, then break stack keeps the nested
	while (!breakStack.empty())											// loop number, from where break was called, and a position of its label in the RPN table
	{																	// thus, if break stack is not empty:
		extract(breakStack, item);										//   extract the number of nested loop and label's postion in RPN table
		if (item.nestedCodeBlockNum == nestedCodeBlocksCount)
		{
			RPNTable[item.position] = Lexeme(RPN_LABEL, RPNTable.size());//	 assign end of the loop as a transfer location for this label
		}
		else
		{
            breakStack.push(item);
            break;
        }
	}
	nestedCodeBlocksCount--;
}

// Converting unary operation to RPN
void Parser::unaryOperationToRPN()
{
	int value;
	while (!plusStack.empty())											// while there are saved operations in the postfix '++' stack:
	{
		extract(plusStack, value);										//   extract the identifier used in the operation
	    RPNTable.push_back(Lexeme(RPN_ADDRESS, value));					//   add the operation to the RPN table
		RPNTable.push_back(Lexeme(LEX_ID, value));
		RPNTable.push_back(Lexeme(LEX_INT_NUM, 1));
		RPNTable.push_back(Lexeme(LEX_PLUS));
		RPNTable.push_back(Lexeme(LEX_ASSIGN));
	}
	while (!minusStack.empty())											// While there are saved operations in the postfix '--' stack:
	{
		extract(minusStack, value);										//   extract the identifier used in the operation
		RPNTable.push_back(Lexeme(RPN_ADDRESS, value));					//   add the operation to the RPN table
		RPNTable.push_back(Lexeme(LEX_ID, value));
		RPNTable.push_back(Lexeme(LEX_INT_NUM, 1));
		RPNTable.push_back(Lexeme(LEX_MINUS));
		RPNTable.push_back(Lexeme(LEX_ASSIGN));
	}
}