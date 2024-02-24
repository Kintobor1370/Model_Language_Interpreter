#include <iostream>
#include <stack>
#include "LexicalAnalyser.cpp"

using namespace std;


/*____________________________________SYNTAX STRUCTURE OF A MODEL LANGUAGE PROGRAM_____________________________________
 * Legend:
 *	1) (A | B) = (A OR B)
 *  2) (A; [ B | C | D ]) = (A; B) OR (A; C) OR (A; D)
 * 	3) (A <; B>) = (A) OR (A; B)
 *  4) ({ A; B }) = code block of operations A and B
 *
 * 
 * Program's header:		P   	-->  program { OPSs }
 *
 * Descriptions:			DESCS	-->  DESC; DESCS | DESC; | eps
 * Description				DESC	--> [int | string | bool] VAR <, VAR>			
 * Variable					VAR	    --> LEX_ID | LEX_ID = CONST														
 * Constant parameter		CONST   --> INT | STR | BOOL
 *
 * Operations				OPS      --> <OP>
 * Operation				OP       --> DES | OP_STMNT | { OPS } | if (STMNT) OP <else OP> | while (STMNT) OP | 
 * 										 for ([STMNT]; [STMNT]; [STMNT]) OP | break; | goto LABEL; | read(ID); | write (STMNT <, STMNT>);
 * Statement operator		OP_STMNT --> STMNT;
 * Statement				STMNT    --> ADD | ADD = STMNT | ADD [==|<|>|<=|>=|!=] ADD 
 * Additive state			ADD		 --> MULTI | MULTI [+ | - | or] MULTI
 * Multiplicative state		MULTI	 --> FIN | FIN [ * | / | and] FIN
 * Final state 				FIN		 --> ID | LABEL: | ID++ | ID-- | ++ID | --ID | [+ | -] FIN | STR | BOOL | not FIN | STMNT
 */


//___________________________________________REVERSE POLISH NOTATION PARSER____________________________________________
class Parser
{
    Scanner scanner;                                                    // Lexical scanner
	vector<Lexeme> RPNs;                                                // Reverse Polish Notation (RPN) table (vectorised)
    
    stack<lexemeType> lexStack;
    Lexeme lex;                                                         // Current lexeme
	lexemeType type;                                                    // Current lexeme's type
	int val;                                                            // Current lexeme's value
	
	bool loopState;														// Indicator that the program iscurrently in loop state
	int nestedLoopsCount;												// Number of nested loops (-1 : no loop state; 0 : no nested loops; >= 1 : >= 1 nested loops)
	bool lvalue;
	
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
	
	bool pp_id;
	
	// Syntax actions
	void P();															// Program's header

	void DESCS();														// Descriptions
	void DESC();														// Description		
	void VAR();															// Variable													
	void CONST();														// Constant parameter

	void OPS();															// Operators
	void OP();															// Operator
	void OP_STMNT();													// Statement operator
	void STMNT(int x=1);												// Statement
	void ADD();															// Additive state
	void MULTI();														// Multiplicative state
	void FIN();															// Final state
	
	// Semantic actions
	void setVar();
	void identCheck(int value);
	void identReadCheck();
	void operationCheck();
	void unaryOperationCheck();
	void notCheck();
	void assignEqualTypeCheck();
	void conditionEqualTypeCheck();
	void breakController(bool mode);
	void breakCheck();
	void gotoCheck();
	
	// Convertion to RPN
	void unaryOperationToRPN();
	
	// Get the next lexeme
	void getLexeme()
	{
		lex = scanner.getLexeme();          							// The scanner gets a lexeme
		type = lex.getType();					                		// Get the lexeme's type
		val = lex.getValue();					        		        // Get the lexeme's value
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
			cerr << "SYNTAX ERROR #" << errNumber << ": " << s << "\nLexeme: " << lex << endl;
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
			cerr << "ERROR: " << s << endl;
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
			cerr << "WARNING: " << s << endl << endl;
		}
	}
	
public:
	Parser(const string fileName): scanner(fileName)
	{
		loopState = 0;
		nestedLoopsCount = -1;
		lvalue = 1;
		pp_id = 0;
	}

    vector<Lexeme> getRPNs()
    {
        return RPNs;
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
	RPNs.clear();

	getLexeme();
	P();
	if (type != LEX_FIN)
		syntaxError(													// Syntax error #1
			1,
			"No final state found...how is this even possible?"
		);
	cout << "No lexical, syntax or semantic issues. Your program is flawless." << '\n';
}

//.........................SYNTAX ANALYSIS
// Analyse code starting from the program's header
void Parser::P()
{
	if (type == LEX_PROGRAM)								    		// if first lexeme is program's header:
		getLexeme();													//   get next lexeme
	else 																// else:
		syntaxError(2, "Did you forget 'program'?");					// Syntax error #2
	if (type == LEX_LEFT_BRACE)											// if next lexeme is left brace:
		getLexeme();													//   get next lexeme
	else																// else:
		syntaxError(3, "Did you forget '{'?");							// Syntax error #3
	
	OPS();																// operators analysis
	gotoCheck();
}

// Descriptions analysis
void Parser::DESCS()
{
	DESC();
	if (type != LEX_SEMICOLON)						    				// if lexeme following the description is not semicolon:
		syntaxError(4, "Did you forget ';'?");							// Syntax error #4
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
		RPNs.push_back(Lexeme(RPN_ADDRESS, val));						// add the lexeme to the RPN table
		getLexeme();
		
		if(type == LEX_ASSIGN)											// if the identifier above is being assigned a constant value
		{
            getLexeme();
            CONST();													//   analyse the constant value
            RPNs.push_back(LEX_ASSIGN);
        }
		else
		{
			RPNs.pop_back();
		}
		if (type == LEX_COMMA || type == LEX_SEMICOLON)		            // if the next lexeme is comma or semicolon:
			lexStack.pop(); 											//		удаление из стека типа переменной и возврат из разбора отдельной переменной
		else
			syntaxError(5, "Unallowed deliminator. Only '=' available");// Syntax error #5
	}
	else																// if the lexeme is NOT an identifier:		
	{
		syntaxError(6, "No identifier found");							// Syntax error #6
	}
}

// Constant value analysis
void Parser::CONST()
{
	auto currentType = lexStack.top();
	if (currentType == LEX_INT || currentType == LEX_STRING || currentType == LEX_BOOL)
	{
        STMNT(0);
        assignEqualTypeCheck();
    }
	else
	{
		syntaxError(7, "No matching const type found");					// Syntax error #7
	}
}

// Analysis of multiple operators
void Parser::OPS()
{
	while (type != LEX_RIGHT_BRACE)
	{
		OP();
		if (type == LEX_FIN)
			syntaxError(8, "Did you forget '}'?");						// Syntax error #8
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
		case LEX_INT: case LEX_BOOL: case LEX_STRING:					// Description of identifiers
			DESCS();
			break;
			
		case LEX_IF:													// if() operator
			getLexeme();
			if (type != LEX_LEFT_PAREN)
				syntaxError(											// Syntax error #9
					9,
					"'if' expression: expected '(' after 'if'"
				);
			getLexeme();
			STMNT();
			conditionEqualTypeCheck();

			pos2 = RPNs.size();
			RPNs.push_back(Lexeme());
			RPNs.push_back(Lexeme(RPN_FGO));
			
			if (type != LEX_RIGHT_PAREN)
				syntaxError(											// Syntax error #10
					10,
					"'if' expression: did you forget ')' ?"
				);
			getLexeme();
			OP();
			RPNs[pos2] = Lexeme(RPN_LABEL, RPNs.size());
			
			if (type == LEX_ELSE)
			{
				pos3 = RPNs.size();
				RPNs.push_back(Lexeme());
				RPNs.push_back(Lexeme(RPN_GO));
				RPNs[pos2] = Lexeme(RPN_LABEL, RPNs.size());
                getLexeme();
				OP();
				RPNs[pos3] = Lexeme(RPN_LABEL, RPNs.size());
			}
			break;
		
		case LEX_WHILE:													// while() loop
			pos0 = RPNs.size();
			getLexeme();
			if (type != LEX_LEFT_PAREN)
				syntaxError(											// Syntax error #11
					11,
					"'while' expression: expected '(' after 'while'"
				);
			getLexeme();
			STMNT();
			conditionEqualTypeCheck();
			pos1 = RPNs.size(); 
			RPNs.push_back(Lexeme());
			RPNs.push_back(Lexeme(RPN_FGO));
			
			if (type != LEX_RIGHT_PAREN)
				syntaxError(											// Syntax error #12
					12,
					"'while' expression: did you forget ')' ?"
				);
			breakController(1);											// break operators processing in RPN table is also done via breakController (0 = off, 1 = on)
			getLexeme();
			OP();
			
			RPNs.push_back(Lexeme(RPN_LABEL, pos0));
            RPNs.push_back(Lexeme(RPN_GO));
            RPNs[pos1] = Lexeme(RPN_LABEL, RPNs.size());
            
            breakController(0);
			break;
		
		case LEX_FOR:													// for(;;) loop
			getLexeme();
			if (type != LEX_LEFT_PAREN)
				syntaxError(											// Syntax error #13
					13,
					"'for' expression: expected '(' after 'for'"
				);
			getLexeme();

			// for(<analysing this part>; ...; ...)
			if (type == LEX_SEMICOLON)
				getLexeme();
			else if (type == LEX_INT || type == LEX_BOOL || type == LEX_STRING)
				DESCS();												// the first part of 'for' loop initialisation can be either a variable declaration (always assigning it a cretain value)
			else
				OP_STMNT();												// or a statement operator
			pos3 = RPNs.size();
			
			// for(...; <analysing this part>; ...)
			if (type == LEX_SEMICOLON)	
			{
				RPNs.push_back(Lexeme(LEX_TRUE, 1));
				getLexeme();
			} 
			else
			{
				STMNT();
				conditionEqualTypeCheck();
				if (type != LEX_SEMICOLON)
					syntaxError(										// Syntax error #14
						14, 
						"'for' expression: ';' between last two statements is missing"
					);
				getLexeme();
			}
			
			pos1 = RPNs.size();
			RPNs.push_back(Lexeme());
			RPNs.push_back(Lexeme(RPN_FGO));
			
			pos2 = RPNs.size();
			RPNs.push_back(Lexeme());
			RPNs.push_back(Lexeme(RPN_GO));
			pos4 = RPNs.size();
			
			//    for(...; ...; <analysing this part>)
			if (type == LEX_RIGHT_PAREN)
				getLexeme();
			else
			{
				STMNT();
			
				RPNs.push_back(Lexeme(RPN_LABEL, pos3));
				RPNs.push_back(Lexeme(RPN_GO));
			
				if (type != LEX_RIGHT_PAREN)
					syntaxError(										// Syntax error #15
						15, 
						"'for' expression: did you forget ')' ?"
					);
				getLexeme();
			}
			RPNs[pos2] = Lexeme(RPN_LABEL, RPNs.size());
			
			breakController(1);
			OP();
			
			RPNs.push_back(Lexeme(RPN_LABEL, pos4));
			RPNs.push_back(Lexeme(RPN_GO));
			RPNs[pos1] = Lexeme(RPN_LABEL, RPNs.size());
			
			breakController(0);
			break;
		
		case LEX_BREAK:													// break operator
			breakCheck();
			getLexeme();
			if (type != LEX_SEMICOLON)
				syntaxError(16, "did you forget ';' ?");				// Syntax error #16
			getLexeme();
			break;
		
		case LEX_GOTO:													// goto operator
            getLexeme();
			if (type != LEX_ID)
				syntaxError(											// Syntax error #17
					17, 
					"expected label after \"goto\" operator"
				);
			if (!identTable[val].isLabel())				        		// if the identifier is not declared as label:
			{
				if (!identTable[val].isDeclared())	    				//   if the identifier is not declared at all:
				{
					identTable[val].setAsLabel();							//      set it as a label (implying this label was not present before in the code)
					identTable[val].setAddress(RPNs.size());
					RPNs.push_back(Lexeme());
					RPNs.push_back(Lexeme(RPN_GO));
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
				int value = identTable[val].getValue();
				RPNs.push_back(Lexeme(RPN_LABEL, value));
				RPNs.push_back(Lexeme(RPN_GO));
			}
			getLexeme();
			if (type != LEX_SEMICOLON)
				syntaxError(											// Syntax error #19
					19, 
					"\"goto\" operator: did you forget ';' ?"
				);
			getLexeme();
			break;
		
		case LEX_READ:													// read() operator
			getLexeme();
			if (type != LEX_LEFT_PAREN)
				syntaxError(											// Syntax error #20
					20, 
					"'read' expression: expected '(' after 'read'"
				);
			getLexeme();
			if (type != LEX_ID)
				syntaxError(											// Syntax error #21
					21, 
					"'read' expression: identifier not found"
				);
			identReadCheck();
			RPNs.push_back(Lexeme(RPN_ADDRESS, val));
			getLexeme();
			
			if (type != LEX_RIGHT_PAREN)
			    syntaxError(											// Syntax error #22
					22, 
					"'read' expression: did you forget ')' ?"
				);
			getLexeme();
			RPNs.push_back(Lexeme(LEX_READ));
				
			if (type != LEX_SEMICOLON)
				syntaxError(23, "Did you forget ';' ?");				// Syntax error #23
			getLexeme();
			break;
		
		case LEX_WRITE:	case LEX_WRITELINE:								// write() and writeline() operators
		{
			lexemeType writeMode = type;
			getLexeme();
			if (type != LEX_LEFT_PAREN)
				syntaxError(											// Syntax error #24
					24, 
					"'write' expression: expected '(' after 'write'"
				);
			getLexeme();
			if (type == LEX_RIGHT_PAREN)
				syntaxError(											// Syntax error #25
					25,
					"'write' expression: identifier not found"
				);
			STMNT(0);
			while (type == LEX_COMMA)
			{
				getLexeme();
				STMNT(0);
			}

			if (type != LEX_RIGHT_PAREN)
				syntaxError(											// Syntax error #26
					26,
					"'write' expression: did you forget ')' ?"
				);
			getLexeme();
			RPNs.push_back(Lexeme(writeMode));
			if (type != LEX_SEMICOLON)
				syntaxError(											// Syntax error #27
					27,
					"Did you forget ';' ?"
				);
			getLexeme();
			break;
		}
		case LEX_LEFT_BRACE:											// Composite operator
			getLexeme();
			OPS();
			break;
		
		default:														// Statement operator
			OP_STMNT();
			break;
	}
}

// Statement operator analysis
void Parser::OP_STMNT()
{
	pp_id = 0;
	STMNT(1);
	if (type == LEX_SEMICOLON || type == LEX_COLON)
		getLexeme();
	else
		syntaxError(28, "Did you forget ';' ?");						// Syntax error #28
}

// Statement analysis
void Parser::STMNT(int operand)
{
	lvalue = operand;
	lexemeType assignedType = type;  									// save the type of lvalue lexeme (in case of assigning variable of a different type)
	ADD();
	
	if (type == LEX_ASSIGN)												// if assignment takes place:
	{
		if (assignedType == LEX_ID && lvalue)							//   check that before assignment was lvalue statement identifier
		{
			pp_id = 1;
			int lvalueUncertain;
			extract(lvalueUncertainStack, lvalueUncertain);
			RPNs[num] = Lexeme(RPN_ADDRESS, lvalueUncertain);
			getLexeme();
			STMNT();
			assignEqualTypeCheck();
			RPNs.push_back(LEX_ASSIGN);
			unaryOperationToRPN();
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
		
		lvalue = 0;
		lexStack.push(type); 
		getLexeme();
		ADD();
		operationCheck();
		RPNs.push_back(Lexeme(compareType));
	}
	
	if (!lvalueUncertainStack.empty())
	{
		int lvalueUncertain;
		extract(lvalueUncertainStack, lvalueUncertain);
		RPNs[num] = lvalue ? Lexeme(RPN_ADDRESS, lvalueUncertain) : Lexeme(LEX_ID, lvalueUncertain);
	}
	
	if (!pp_id)
		unaryOperationToRPN();
}

void Parser::ADD()
{
	MULTI();
	while (type == LEX_PLUS || type == LEX_MINUS || type == LEX_OR)
	{
		lexemeType additionType = type;
		
		lvalue = 0;
		lexStack.push(type);
		getLexeme();
		MULTI();
		operationCheck();
		RPNs.push_back(Lexeme(additionType));
	}
}

void Parser::MULTI()
{
	FIN();
	while (type == LEX_TIMES || type == LEX_SLASH || type == LEX_PERCENT || type == LEX_AND)
    {
		lexemeType multiplicationType = type;
		
		lvalue = 0;
		lexStack.push(type);
		getLexeme();
		FIN();
        operationCheck();
		RPNs.push_back(Lexeme(multiplicationType));
	}
}

void Parser::FIN()
{
	switch (type)
	{
		case LEX_ID:
		{
			if (lvalue)
			{
				lvalueUncertainStack.push(val);
				num = RPNs.size();
				RPNs.push_back(Lexeme());
			}
			else
			{
				RPNs.push_back(Lexeme(LEX_ID, val));
			}
			int idValue = val;
            getLexeme();
			if (type == LEX_COLON)					    				// if ':' goes after the identifier:
				if (!pp_id)
				{														//   it means that identifier is a label
					if (lvalue)
						lvalueUncertainStack.pop();
					RPNs.pop_back();
					
					if (identTable[idValue].isLabel())					//   if an identifier was declared as label before:					
					{
						int pos = identTable[idValue].getAddress();		//     pos - label's address in the code
						if (identTable[idValue].getValue() != -1)		//	   if this label has already been assigned a value:
						{												//       the label was placed twice within the code => error
							syntaxError(								// Syntax error #30
								30, 
								"Label \"" + identTable[idValue].getName() + "\" is declared twice"
							);
						}
						identTable[idValue].setValue(RPNs.size());		//     assign the location the label will lead to
						identTable[idValue].setAssign();				//     confirm that label has been assigned a value
						RPNs[pos] = Lexeme(RPN_LABEL, RPNs.size());
					}
					else if (!identTable[idValue].isDeclared())			//     if an identifier was not declared as label:		
					{
						identTable[idValue].setAsLabel();				//       declare the identifier as label
						identTable[idValue].setValue(RPNs.size());		//       assign the location where label will lead to
						identTable[idValue].setAssign();				//       confirm that label has been assigned a value
					}
					else
					{	
						syntaxError(									// Syntax error #31
							31,
							"Label \"" + identTable[idValue].getName() + "\" is already declared as an identifier and cannot be used"
						);
					}
				}
				else
				{
					syntaxError(										// Syntax error #32
						32,
						"Wrong usage of label \"" + identTable[idValue].getName() + "\""
					);
				}
			else if (type == LEX_PLUS_PLUS || type == LEX_MINUS_MINUS)
			{
				if (!pp_id)
					RPNs.pop_back();
				lvalue = 0;
				type == LEX_PLUS_PLUS ? plusStack.push(idValue) : minusStack.push(idValue);
				getLexeme();
			}
			identCheck(idValue);
			break;
		}	
		case LEX_NUM:		
			lexStack.push(LEX_INT);							    		// the only available data type is 'int'. Put it in the lexemes stack
			RPNs.push_back(lex);
			getLexeme();
			break;
		
		case LEX_PLUS:
			lvalue = 0;
			getLexeme();
			FIN();
			unaryOperationCheck();
			break;
		
		case LEX_MINUS:
			lvalue = 0;
			getLexeme();
			FIN();
			unaryOperationCheck();
			RPNs.push_back(Lexeme(LEX_UNARY_MINUS));
			break;
			
		case LEX_PLUS_PLUS: case LEX_MINUS_MINUS:
			lvalue = 0;
			lexemeType unaryOpType;
			unaryOpType = type == LEX_PLUS_PLUS ? LEX_PP_PRE : LEX_MM_PRE;// save unary operation's type to add it to RPN table
			
			getLexeme();
			if (type != LEX_ID)
				syntaxError(											// Syntax error #33
					33, 
					"Lvalue requied as an increment operand"
				);
			identCheck(val);
			operationCheck();
			if (!pp_id)
			{
				RPNs.push_back(Lexeme(RPN_ADDRESS, val));
				RPNs.push_back(Lexeme(LEX_ID, val));
				RPNs.push_back(Lexeme(LEX_NUM, 1));
				unaryOpType == LEX_PP_PRE ?
					RPNs.push_back(Lexeme(LEX_PLUS)) :
					RPNs.push_back(Lexeme(LEX_MINUS));
				RPNs.push_back(Lexeme(LEX_ASSIGN));
			}
			else
			{
				RPNs.push_back(Lexeme(RPN_ADDRESS, val));
				RPNs.push_back(Lexeme(unaryOpType));
			}
			getLexeme();
			break;
		
		case LEX_QUOTE:
			getLexeme();
			lexStack.push(LEX_STRING);
            RPNs.push_back(lex);
			if (type != LEX_STR_CONST)
				syntaxError(34, "No string constant found");			// Syntax error #34
			getLexeme();												// get the finishing quote (if it is missing lexical error will be triggered)
			getLexeme();
			break;
		
		case LEX_TRUE: case LEX_FALSE:
			lexStack.push(LEX_BOOL);									// true and false are bool => put bool in the lexemes stack
			type == LEX_TRUE ? 
				RPNs.push_back(Lexeme(LEX_TRUE, 1)) :
				RPNs.push_back(Lexeme(LEX_FALSE, 0));
			getLexeme();
			break;
		
		case LEX_NOT:
			lvalue = 0;
			getLexeme();
			FIN();
			notCheck();
			RPNs.push_back(Lexeme(LEX_NOT));
			break;
		
		case LEX_LEFT_PAREN:
			getLexeme();
			STMNT(0);
			if (type != LEX_RIGHT_PAREN)
				syntaxError(35, "Did you forget ')' ?");				// Syntax error #35
			getLexeme();
			break;
		
		default:
			syntaxError(36, "No matching operand found");				// Syntax error #36
			break;
	}
}


//.........................SEMANTIC ANALYSIS
// Set the variable's type and check its declaration status
void Parser::setVar()
{
	if (identTable[val].isDeclared())							    	// if the variable has already been declared before:
	{
		semanticError(													//   semantic error
			"Variable \"" + identTable[val].getName() + "\" is declared twice"
		);
	}
	else 																// else:
	{
		identTable[val].setType(lexStack.top());		    			//   assign the variable its type (which is kept in the end of the lexemes stack)
		identTable[val].setDeclare();				    				//   confirm the variable has been declared
	}
}

// Check whether identifier was declared or not
void Parser::identCheck(int value)										// Проверка идентификатора (объявлен или нет)
{
	if(identTable[value].isDeclared())									// if declared:
		lexStack.push(identTable[value].getType());						//   add it to the lexemes stack
	else																// else:
		semanticError(													//   semantic error
			"Variable \"" + identTable[value].getName() + "\" has not been declared"
		);
}

// Check the identifier's declaration in read()
void Parser::identReadCheck()
{
	if(!identTable[val].isDeclared())
		semanticError(
			"in 'read()' function: Variable \"" + identTable[val].getName() + "\" has not been declared"
		);
}

// Single operation check
void Parser::operationCheck()
{
	lexemeType opLeft;													// left operand 
	lexemeType opRight;													// right operand
	lexemeType oper;													// operator
	
	lexemeType opType;													// operation's type
	lexemeType resType;													// operation result's type

	extract(lexStack, opRight);
	extract(lexStack, oper);
	extract(lexStack, opLeft);
	
	if (opLeft == LEX_STRING && opLeft == opRight)
	{
		opType = LEX_STRING;
		if(oper == LEX_PLUS)
			resType = LEX_STRING;
		else if (
			oper == LEX_EQ ||
			oper == LEX_NOT_EQ ||
			oper == LEX_GREATER ||
			oper == LEX_LESS
		)
			resType = LEX_BOOL;
		else
			semanticError("Unallowed operator for variables of type \"string\"");
	}
	else
	{
		if (oper >= LEX_EQ && oper <= LEX_NOT_EQ)
		{
			opType = LEX_INT;
			resType = LEX_BOOL;
		}
		else if (oper >= LEX_PLUS && oper <= LEX_PERCENT)
		{
			opType = LEX_INT;
			resType = LEX_INT;
		}
		else if (oper == LEX_OR || oper == LEX_AND)
		{
			opType = LEX_BOOL;
			resType = LEX_BOOL;
		}
	}
	if (
		opLeft == opRight && opLeft == opType ||
		opType == LEX_BOOL && opLeft != LEX_STRING && opRight != LEX_STRING
	)
		lexStack.push(resType);
	else
		semanticError("Variable types in the operation do not match");
}

void Parser::unaryOperationCheck()
{
	int opType = lexStack.top();										// operand's type is kept at the end of the lexemes stack
	if(opType != LEX_INT)												// if operand's type is not integer:
		semanticError("Wrong type for unary operation");				//   semantic error
}

// 'not' operator check
void Parser::notCheck()
{
	lexemeType opType = lexStack.top();
	if(opType != LEX_BOOL)
		semanticError("Wrong type in 'not' statement");
}

// Check of equality of variable type and statement type before assignment
void Parser::assignEqualTypeCheck()
{
	lexemeType typeRight;												// statement (or right variable) type 
	extract(lexStack, typeRight);										// extract it from the lexemes stack
	if (																// NOTE: it is allowed to assign integer values to bool variables
		lexStack.top() != typeRight &&
		(lexStack.top() != LEX_BOOL || typeRight != LEX_INT)
	)
		semanticError("The types do not match");
}

// Check of statement type in conditions of if() / while() / for(;;) / do-while()
void Parser::conditionEqualTypeCheck()
{
	if(lexStack.top() == LEX_BOOL)										// must be bool
		lexStack.pop();
	else
		semanticError("The expression is not boolean");
}

// Break controller
void Parser::breakController(bool turnedOn)
{
	if(turnedOn)														// if turned on:
	{
        loopState = 1;													//   the code is in loop state
        nestedLoopsCount++;												//   number of nested loops may also increase
    }
	else
	{
		if(!nestedLoopsCount)											// if a standard loop was finished and not a nested one:
			loopState = 0;											    //   the code is out of loop state
		
		breakStackItem item;											// if a loop (standard or nested) has a break operator in it, then break stack keeps the nested
		while(!breakStack.empty())										// loop number, from where break was called, and a position of its label in the RPN table
		{																// thus, if break stack is not empty:
			extract(breakStack, item);									//   extract the number of nested loop and label's postion in RPN table
			if(item.nestedLoopNumber == nestedLoopsCount)
			{
				RPNs[item.position] = Lexeme(RPN_LABEL, RPNs.size());	//	 assign end of the loop as a transfer location for this label
			}
			else
			{
                breakStack.push(item);
                break;
            }
		}
		nestedLoopsCount--;
	}
}

// Checking break
void Parser::breakCheck()
{
	if(loopState)														// if the code is in loop state:
	{
		int pos = RPNs.size();
		breakStackItem newItem {nestedLoopsCount, pos};
		breakStack.push(newItem);										//   push break's position in RPN into stack
		RPNs.push_back(Lexeme());										//   add empty lexeme (will be assigned transfer location later) to RPN table
		RPNs.push_back(Lexeme(RPN_GO));								    //   add transfer lexeme to RPN table
	}
	else																// else:
	{																	//    semantic error
		semanticError("'break' can only be used in cycles");
	}
}

// Checking goto operation
void Parser::gotoCheck()
{
	vector<Identifier>::iterator it;
	for(it = identTable.begin(); it != identTable.end(); ++it)
	{
		if(it->isLabel())
		{
			if(it->isAssigned() && it->getAddress() == -1)
				semanticWarning("label \""+ it->getName() + "\" declared, but not used");
			if(!it->isAssigned() && it->getAddress() != -1)
				semanticError("label \""+ it->getName() + "\" used, but not declared");
		}
	}
}

// Converting unary operation to RPN
void Parser::unaryOperationToRPN()
{
	int value;
	while(!plusStack.empty())
	{
		extract(plusStack, value);
	    RPNs.push_back(Lexeme(RPN_ADDRESS, value));
		RPNs.push_back(Lexeme(LEX_ID, value));
		RPNs.push_back(Lexeme(LEX_NUM, 1));
		RPNs.push_back(Lexeme(LEX_PLUS));
		RPNs.push_back(Lexeme(LEX_ASSIGN));
	}
	while(!minusStack.empty())
	{
		extract(minusStack, value);
		RPNs.push_back(Lexeme(RPN_ADDRESS, value));
		RPNs.push_back(Lexeme(LEX_ID, value));
		RPNs.push_back(Lexeme(LEX_NUM, 1));
		RPNs.push_back(Lexeme(LEX_MINUS));
		RPNs.push_back(Lexeme(LEX_ASSIGN));
	}
}