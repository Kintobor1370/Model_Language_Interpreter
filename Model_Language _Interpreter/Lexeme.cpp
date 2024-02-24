#include <iostream>

using namespace std;


//____________________________________________________LEXEME TYPES_____________________________________________________
enum lexemeType
{
	LEX_NULL,															// 0
	
	// Lexemes for lexical analysis
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
	LEX_STRING,															// 19
	LEX_TRUE,															// 20
	LEX_WHILE,															// 21
	LEX_WRITE,															// 22
	LEX_WRITELINE,														// 23
	LEX_FIN,															// 24
	
	LEX_LEFT_BRACE,														// 25   1
	LEX_RIGHT_BRACE,													// 26   2
	LEX_QUOTE,															// 27   3
	LEX_SEMICOLON,														// 28   4
	LEX_COMMA,															// 29   5
	LEX_COLON,															// 30   6
	LEX_ASSIGN,															// 31   7
	LEX_PLUS,															// 32   8
	LEX_MINUS,															// 33   9
	LEX_TIMES,															// 34   10
	LEX_SLASH,															// 35   11
	LEX_PERCENT,														// 36   12
	LEX_PLUS_PLUS,														// 37   13
	LEX_MINUS_MINUS,													// 38   14
	LEX_PLUS_ASSIGN,													// 39   15
	LEX_MINUS_ASSIGN,													// 40   16
	LEX_LEFT_PAREN,														// 41   17
	LEX_RIGHT_PAREN,													// 42   18
	LEX_EQ,																// 43   19
	LEX_GREATER,														// 44   20
	LEX_LESS,															// 45   21
	LEX_GREATER_EQ,														// 46   22
	LEX_LESS_EQ,														// 47   23
	LEX_NOT_EQ,															// 48   24

	LEX_ID,																// 49
	LEX_NUM,															// 50
	LEX_STR_CONST,														// 51

	// Parsing Tokens
	LEX_UNARY_MINUS,													// 52 - Unary minus (detected by parsing)
	LEX_PP_PRE,															// 53 - prefix '++'
	LEX_PP_POST,														// 54 - postfix '++'
	LEX_MM_PRE,															// 55 - prefix '--'
	LEX_MM_POST,														// 56 - postfix '--'
	
	// Reverse Polish Notation (RPN) tokens
    RPN_GO, 															// 57
	RPN_FGO,															// 58
	RPN_LABEL,  														// 59
	RPN_ADDRESS 														// 60
};


//____________________________________________________LEXEME CLASS_____________________________________________________
class Lexeme
{
	lexemeType type;
	int value;
	
public:
	Lexeme(lexemeType type=LEX_NULL, int value=0)
    {
        this->type = type;
        this->value = value;
    }

	lexemeType getType() const
	{
		return type;
	}
	
	int getValue() const
	{
		return value;
	}

	friend ostream& operator << (ostream &out, Lexeme l)
	{
		out << '(' << l.type << ',' << l.value << "); ";
		return out;
	}
};