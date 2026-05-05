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
	LEX_DO,																// 6
	LEX_ELSE,															// 7
	LEX_END,															// 8
	LEX_FALSE,															// 9
	LEX_FOR,															// 10
	LEX_GOTO,															// 11
	LEX_IF,																// 12
	LEX_INT,															// 13
	LEX_NOT,															// 14
	LEX_OF,																// 15
	LEX_OR,																// 16
	LEX_PROGRAM,														// 17
	LEX_READ,															// 18
	LEX_REAL,															// 19
	LEX_STEP,															// 20
	LEX_STRING,															// 21
	LEX_TRUE,															// 22
	LEX_UNTIL,															// 23
	LEX_WHILE,															// 24
	LEX_WRITE,															// 25
	LEX_WRITELINE,														// 26
	
	// End of file lexeme
	LEX_EOF,															// 27
	
	// Lexemes for delimeters
	LEX_LEFT_BRACE,														// 28   1
	LEX_RIGHT_BRACE,													// 29   2
	LEX_QUOTE,															// 30   3
	LEX_SEMICOLON,														// 31   4
	LEX_COMMA,															// 32   5
	LEX_COLON,															// 33   6
	LEX_ASSIGN,															// 34   7
	LEX_PLUS,															// 35   8
	LEX_MINUS,															// 36   9
	LEX_TIMES,															// 37   10
	LEX_SLASH,															// 38   11
	LEX_PERCENT,														// 39   12
	LEX_PLUS_PLUS,														// 40   13
	LEX_MINUS_MINUS,													// 41   14
	LEX_PLUS_ASSIGN,													// 42   15
	LEX_MINUS_ASSIGN,													// 43   16
	LEX_LEFT_PAREN,														// 44   17
	LEX_RIGHT_PAREN,													// 45   18
	LEX_EQ,																// 46   19
	LEX_GREATER,														// 47   20
	LEX_LESS,															// 48   21
	LEX_GREATER_EQ,														// 49   22
	LEX_LESS_EQ,														// 50   23
	LEX_NOT_EQ,															// 51   24

	LEX_ID,																// 52
	LEX_INT_NUM,														// 53
	LEX_REAL_NUM,														// 54
	LEX_STR_CONST,														// 55

	// Parsing Tokens
	LEX_UNARY_MINUS,													// 56 - Unary minus (detected by parsing)
	LEX_PP_PRE,															// 57 - Prefix '++'
	LEX_PP_POST,														// 58 - Postfix '++'
	LEX_MM_PRE,															// 59 - Prefix '--'
	LEX_MM_POST,														// 60 - Postfix '--'
	
	// Reverse Polish Notation (RPN) tokens
    RPN_GO, 															// 61 - RPN goto operator
	RPN_GO_FALSE,														// 62 - RPN false goto operator (works only if the prior boolean value is false)
	RPN_LABEL,  														// 63 - RPN goto label
	RPN_ADDRESS, 														// 64 - Identifier table address
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