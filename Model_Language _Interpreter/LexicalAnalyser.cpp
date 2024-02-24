#include <iostream>
#include "Tables.cpp"

using namespace std;


//_______________________________________________________SCANNER_______________________________________________________
class Scanner
{
	FILE *f;															// file descriptor of a model language program
	
	enum state
	{
		INIT,															// initial state
		IDENT,
		NUMBER,
		STRING,
		COMMENT,														// comment
		COMMENT_STRING,													// one-line comment
		DELIM,															// delimeter
		NOT_EQ,															// not equal
		FIN																// final state
	};
	
	state currentState;
	
	static string wordTable[];											// functional words table
	static lexemeType words[];
	
	static string delimTable[];											// delimeters table
	static lexemeType delims[];
	
	char c;																// the current character 
	string buf;															// buffer for the string being entered
	int bufTop;															// position of the last non-empty character in buffer
	
private:
	// Open model language program file for reading
	void openFile(const string fileName)
	{
		f = fopen(fileName.c_str(), "r");
		return;
	}
	
	// Clear buffer
	void clearBuffer()
	{
		bufTop = 0;
		buf.clear();
	}
	
	// Add new character in buffer
	void addChar(char c)
	{
		buf.push_back(c);
		bufTop++;
	}
	
	// Check if a string in buffer is present in the lexemes list
	int check(const string &buf, const string lexemesList[])
	{
		int i = 0;
		while (!lexemesList[i].empty())
		{
			if (buf == lexemesList[i])									// if a string in buffer is equal to an item of the lexemes list,
				return i + 1;											// return its position in the list
			i++;														// else check the next item
		}
		return 0;														// 0 is returned if a string in buffer is not present in the lexemes list
	}
	
	// Reading the next character of a model language program
	void getChar()
	{
		c = fgetc(f);
		return;
	}
	
	// Pushing the current character back into the input stream
	void ungetChar()
	{
		ungetc(c, f);
		return;
	}
	
	// Lexical error processing
	void lexicalError(string err)
	{
		try
		{
			throw err;
		}
		catch (string token)
		{
			string errorMessage;
			switch (token[0])
			{
				case '\\':
					errorMessage = "1: Unknown escape sequence: " + token;
					break;
				
				case '\"':
					errorMessage = "2: Missing terminating \" character.";
					break;
				
				case '!':
					errorMessage = "3: Expected initializer before '" + token + "' token.";
					break;
				
				case '\'':
					errorMessage = "4: No match for deliminator " + token;
					break;
			}
			cerr << "LEXICAL ERROR #" << errorMessage << endl;
			exit(1);
		}
	}

public:
	Scanner(const string fileName)
	{
		openFile(fileName);
		currentState = INIT;
		getChar();
	}
	
	~Scanner()
	{
		fclose(f);
	}
	
	Lexeme getLexeme();
};


string Scanner::wordTable[] =
{
	"and",
	"bool",
	"break",
	"case",
	"continue",
	"do",
	"else",
	"end",
	"false",
	"for",
	"goto",
	"if",
	"int",
	"not",
	"of",
	"or",
	"program",
	"read",
	"string",
	"true",
	"while",
	"write",
	"writeline",
};

lexemeType Scanner::words[] =
{
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
};

string Scanner::delimTable[] =
{
	"{",
	"}",
	"\"",
	";",
	",",
	":",
	"=",
	"+",
	"-",
	"*",
	"/",
	"%",
	"++",
	"--",
	"+=",
	"-=",	
	"(",
	")",
	"==",
	">",
	"<",
	">=",
	"<=",
	"!=",
};

lexemeType Scanner::delims[] =
{
	LEX_LEFT_BRACE,														// 1
	LEX_RIGHT_BRACE,													// 2
	LEX_QUOTE,															// 3
	LEX_SEMICOLON,														// 4
	LEX_COMMA,															// 5
	LEX_COLON,															// 6
	LEX_ASSIGN,															// 7
	LEX_PLUS,															// 8
	LEX_MINUS,															// 9
	LEX_TIMES,															// 10
	LEX_SLASH,															// 11
	LEX_PERCENT,														// 12
	LEX_PLUS_PLUS,														// 13
	LEX_MINUS_MINUS,													// 14
	LEX_PLUS_ASSIGN,													// 15
	LEX_MINUS_ASSIGN,													// 16
	LEX_LEFT_PAREN,														// 17
	LEX_RIGHT_PAREN,													// 18
	LEX_EQ,																// 19
	LEX_GREATER,														// 20
	LEX_LESS,															// 21
	LEX_GREATER_EQ,														// 22
	LEX_LESS_EQ,														// 23
	LEX_NOT_EQ,															// 24
};

Lexeme Scanner::getLexeme()
{
	clearBuffer();
	int number;															// a number, encountered in the model language code
	int lex;															// value of the current lexeme
	
	do
	{
		switch(currentState)
		{
			case INIT:													// Initial state:
				if (c == ' ' || c == '\n' || c == '\r' || c == '\t')	//   if the character is space/end of the line/new line:
					getChar();
				else if (isalpha(c))									//   if the character is an identifier:									
				{
					clearBuffer();
					addChar(c);
					currentState = IDENT;
					getChar();
				}
				else if (isdigit(c))									//   if the character is a number:
				{
					number = c - '0';
					currentState = NUMBER;
					getChar();
				}
				else if (c == '\"')
				{
					addChar(c);
					currentState = STRING;
					getChar();
					lex = check(buf, delimTable);
					return Lexeme(LEX_QUOTE, lex);
				}     
				else if (c == '/')                                      //   if the character is start of a comment:
				{
					clearBuffer();
					addChar(c);
					getChar();
					switch(c)
					{
						case '*':                                       //     in case of multiple-line comment:
							clearBuffer();
							currentState = COMMENT;
							getChar();
							break;

						case '/':                                       //     in case of one-line comment:
							clearBuffer();
							currentState = COMMENT_STRING;
							getChar();
							break;
						
						default:										//     in case of not a comment:
							ungetChar();								//       return the current character back into the input stream
							currentState = DELIM;
							c = '/';									//       return to analysing '/' as delimeter
							break;
					}
				}
				else if (c == '!')                                      //   if the character is 'not equal' sign:
				{
					clearBuffer();
					addChar(c);
					currentState = NOT_EQ;
					getChar();
				}
				else if (c == EOF)                                      //   if the character is end of file:
				{
					currentState = FIN;
				}
				else                                                    //    else: delimeter
				{
					clearBuffer();
					addChar(c);
					currentState = DELIM;
				}
				break;
			
			case IDENT:													// Identifier state:
				lex = check(buf, wordTable);
				if (isalpha(c) || isdigit(c))                           //   if the character is alphabetic or a number:
				{
					addChar(c);                                         //     add it to the buffer as a part of an identifier
					getChar();
				}
				else                                                    //   else: the identifier is finalised 
				{
					currentState = INIT;								//     switch back to the initial state
					if (lex)											//     if identifier in buffer has a match in a functional words table:
						return Lexeme((lexemeType) lex, lex);			//       return its lexeme
					else                                                //     else:
					{
						lex = addUniqueIdent(buf);                      //       add it to the table
						return Lexeme(LEX_ID, lex);
					}
				}
				break;
			
			case NUMBER:                                                // Number state
				if (isdigit(c))                                         //   if the character is a digit:
				{
                    int newDigit = c - '0';
					number = 10 * number + newDigit;                    //     make it continue the number
					getChar();
				}
				else                                                    //   else: the number is finalised
				{
					currentState = INIT;
					return Lexeme(LEX_NUM, number);                     //     return the number as a lexeme
				}
				break;
				
			case STRING:												// String state
				clearBuffer();
				if (c != '\"')											// if the character is NOT a finishing quote:
				{
					while (c != '\"')									//   consider each next character a part of string until a finishing quote is met
					{
						if (c == '\\')									//   if the character is a control character:
						{
							getChar();
							switch(c)									//     check the next character
							{
								case 'n':								//      new line character
									addChar('\n');
									break;
								
								case '0':								//		end of line character
									addChar('\0');
									break;
								
								case 'r':								//      carriage return
									addChar('\r');
									break;
								
								case 't':								//      horizontal tab character
									addChar('\t');
									break;
								
								case '\\': case '\'': case '\"': case '\?': case '\%':// standard characters
									addChar(c);
									break;
								
								case '\n':								//	    string's continuation in the next line of the code
									getChar();
									while (c == '\t')					//		while the character is not horizontal tab used for code allignment:	
										getChar();						//        get next character
									ungetChar();						//		unget character that is not a tab
									break;
								
								default:								//      wrong control character
									string wrongToken = "\\" + string(1, c);
									lexicalError(wrongToken);
									break;
							}
						}
						else if (c == '\n')								//   else: if the character is an end of line:
							lexicalError("\"");							//     lexical error: no finishing quote
						else											//    else: the character is a part of string
							addChar(c);									//      so add it to the buffer
						getChar();
					}													//   when a finishing quote is met, the string is complete
					lex = addUniqueStrConst(buf);				    	//   add the completed string to the identifiers table
					return Lexeme(LEX_STR_CONST, lex);
				}
				else													// if the character is a finishing quote
				{
					addChar(c);
					getChar();
					currentState = INIT;								//   go out of the string state
					lex = check(buf, delimTable);
					return Lexeme(LEX_QUOTE, lex);						//   add finishing quote to the identifiers table
				}
				break;
				
			case COMMENT:                                               // Multiple-line comment state
				addChar(c);
				if (c == '*')
				{
					getChar();
					if (c == '/')
					{
						addChar(c);
						getChar();
						currentState = INIT;
					}
					else
						ungetChar();
				}
				if (c == EOF)
					currentState = FIN;
				else
					getChar();
				break;
			
			case COMMENT_STRING:                                        // One-line comment state
				while (c != '\n' && c != EOF)
					getChar();
				if (c == EOF)
					currentState = FIN;
				else
				{
					getChar();
					currentState = INIT;
				}
				break;
			
			case NOT_EQ:                                                // 'not equal' sign state
				if (c == '=')                                           //   if the character is '='
				{
					addChar(c);
					currentState = INIT;
					getChar();
					lex = check(buf, delimTable);
					return Lexeme(LEX_NOT_EQ, lex);
				}
				else                                                    //   else: lexical error
					lexicalError("!");
				break;
			
			case DELIM:                                                 // Delimeter state:
				char first;
                first = c;
				getChar();
                char second;
                second = c;
				
				// Composite delimeter analysis ("++", "--", "+=", "-=", "==", ">=", "<=")
				if (													// Check the composite delimeter's first character
					first == '+' ||
					first == '-' ||
					first == '>' ||
					first == '<' ||
					first == '='
				)
					switch(second)										// Check the composite delimeter's second character
					{
						case '+': case '-':
							if (first == second)					    // Check that the composite delimeter is either "++" or "--"
							{
								addChar(second);
								getChar();
							}
							break;

						case '=':
							addChar(second);							// '=' can be placed after any character of a composite delimeter
							getChar();
							break;
						
						default:
							break;
					}
				currentState = INIT;
				lex = check(buf, delimTable);
				if (lex)
					return Lexeme((lexemeType)(lex + (int)LEX_FIN), lex);
				else
					lexicalError("'" + buf + "'");
				break;
				
			case FIN:
				return Lexeme(LEX_FIN);
				break;
		}
	}
	while (true);
};