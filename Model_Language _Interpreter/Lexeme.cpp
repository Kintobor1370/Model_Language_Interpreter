#include <iostream>

using namespace std;


//____________________________________________________LEXEME TYPES_____________________________________________________
enum lexemeType
{
	LEX_NULL,															// 0
	
	// Lexemes for functional words
	LEX_AND,															// 1
	LEX_BOOL,															// 2
	LEX_BREAK,															// 3
	LEX_CASE,															// 4
	LEX_CONTINUE,														// 5
	LEX_DEFAULT,														// 6
	LEX_DO,																// 7
	LEX_ELSE,															// 8
	LEX_END,															// 9
	LEX_FALSE,															// 10
	LEX_FOR,															// 11
	LEX_GOTO,															// 12
	LEX_IF,																// 13
	LEX_INT,															// 14
	LEX_NOT,															// 15
	LEX_OF,																// 16
	LEX_OR,																// 17
	LEX_PROGRAM,														// 18
	LEX_READ,															// 19
	LEX_REAL,															// 20
	LEX_STEP,															// 21
	LEX_STRING,															// 22
	LEX_SWITCH,															// 23
	LEX_TRUE,															// 24
	LEX_UNTIL,															// 25
	LEX_WHILE,															// 26
	LEX_WRITE,															// 27
	LEX_WRITELINE,														// 28
	
	// End of file lexeme
	LEX_EOF,															// 29
	
	// Lexemes for delimeters
	LEX_LEFT_BRACE,														// 30   1
	LEX_RIGHT_BRACE,													// 31   2
	LEX_QUOTE,															// 32   3
	LEX_SEMICOLON,														// 33   4
	LEX_COMMA,															// 34   5
	LEX_COLON,															// 35   6
	LEX_ASSIGN,															// 36   7
	LEX_PLUS,															// 37   8
	LEX_MINUS,															// 38   9
	LEX_TIMES,															// 39   10
	LEX_SLASH,															// 40   11
	LEX_PERCENT,														// 41   12
	LEX_PLUS_PLUS,														// 42   13
	LEX_MINUS_MINUS,													// 43   14
	LEX_PLUS_ASSIGN,													// 44   15
	LEX_MINUS_ASSIGN,													// 45   16
	LEX_LEFT_PAREN,														// 46   17
	LEX_RIGHT_PAREN,													// 47   18
	LEX_EQ,																// 48   19
	LEX_GREATER,														// 49   20
	LEX_LESS,															// 50   21
	LEX_GREATER_EQ,														// 51   22
	LEX_LESS_EQ,														// 52   23
	LEX_NOT_EQ,															// 53   24

	LEX_ID,																// 54
	LEX_INT_NUM,														// 55
	LEX_REAL_NUM,														// 56
	LEX_STR_CONST,														// 57

	// Parsing Tokens
	LEX_UNARY_MINUS,													// 58 - Unary minus (detected by parsing)
	LEX_PP_PRE,															// 59 - Prefix '++'
	LEX_PP_POST,														// 60 - Postfix '++'
	LEX_MM_PRE,															// 61 - Prefix '--'
	LEX_MM_POST,														// 62 - Postfix '--'
	
	// Reverse Polish Notation (RPN) tokens
    RPN_GO, 															// 63 - RPN goto operator
	RPN_GO_FALSE,														// 64 - RPN false goto operator (works only if the prior boolean value is false)
	RPN_LABEL,  														// 65 - RPN goto label
	RPN_ADDRESS, 														// 66 - Identifier table address
	RPN_SWITCH,															// 67 - RPN switch operator
};


//____________________________________________________LEXEME CLASS_____________________________________________________
class Lexeme
{
	lexemeType type;
	double value;
	
public:
	Lexeme(lexemeType t = LEX_NULL, double v = 0) : type(t), value(v) {}

	lexemeType getType() const
	{
		return type;
	}
	
	double getValue() const
	{
		return value;
	}

	// DEBUG
	friend ostream& operator << (ostream &out, Lexeme lex)
	{
		out << '(' << lex.type << ',' << lex.value << "); ";
		return out;
	}
};