#include <stack>
#include "Scanner.hpp"

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
	void getLexeme();
	
	// Syntax error processing
	void syntaxError(int errNumber, string err);
	
	// Semantic error processing
	void semanticError(string err);
	
	// Semantic warning processing
	void semanticWarning(string err);
	
public:
	Parser(vector<Lexeme> sc);

    vector<Lexeme> getRPNTable();
	
	void validateProgram();
};