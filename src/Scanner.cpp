#include <iostream>
#include "Scanner.hpp"

using namespace std;


//_______________________________________________________SCANNER_______________________________________________________
string Scanner::wordTable[] =
{
	"and",
	"bool",
	"break",
	"case",
	"continue",
	"default",
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
	"real",
	"step",
	"string",
	"switch",
	"true",
	"until",
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
	LEX_EOF,															// 29
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

void Scanner::clearBuffer()
{
	buffer.clear();
	bufferTop = 0;
}
	
void Scanner::addToBuffer(char newChar)
{
	buffer.push_back(newChar);
	bufferTop++;
}
	
int Scanner::checkPresence(const string lexemeTable[])
{
	int i = 0;
	while (!lexemeTable[i].empty())
	{
		if (buffer == lexemeTable[i])								// if a string in buffer is equal to a table item:
		{
			return i + 1;											//   return its position in the list
		}
		i++;														// else: check the next item
	}
	return 0;														// 0 is returned if a string in buffer is not present in the table
}
	
void Scanner::getChar()
{
	currChar = fgetc(f);
}
	
void Scanner::ungetChar()
{
	ungetc(currChar, f);
}
	
void Scanner::lexicalError(string err)
{
	try
	{
		throw err;
	}
	catch (string token)
	{
		int errorNum;
		string errorMsg;
		switch (token[0])
		{
			case '\\':
				errorNum = 1;
				errorMsg = "Unknown escape sequence: " + token;
				break;
				
			case '\"':
				errorNum = 2;
				errorMsg = "Missing terminating \" character.";
				break;
				
			case '!':
				errorNum = 3;
				errorMsg = "Expected initializer before '" + token + "' token.";
				break;
				
			case '\'':
				errorNum = 4;
				errorMsg = "No match for deliminator " + token;
				break;
		}
		cout << "[LEXICAL ERROR #" << errorNum << "] " << errorMsg << endl;
		exit(1);
	}
}

Scanner::Scanner(const string fileName)
{
	f = fopen(fileName.c_str(), "r");
	currState = INIT;
	getChar();
}
	
Scanner::~Scanner()
{
	fclose(f);
}

Lexeme Scanner::getLexeme()
{
	clearBuffer();
	int number = 0;														// a number, encountered in the code
	int decimal = 0;													// a value after decimal, encountered in the code
	int currLexVal;														// a value of the current lexeme
	
	do
	{
		switch(currState)
		{
			case INIT:													// Initial state:
				if (
					currChar == ' ' || 									//   if the character is space or end of the line or new line:
					currChar == '\n' || 
					currChar == '\r' || 
					currChar == '\t'
				) {
					getChar();											//     get the next character
				}
				else if (isalpha(currChar))								//   if the character is an identifier:									
				{
					clearBuffer();
					addToBuffer(currChar);
					currState = IDENT;
					getChar();
				}
				else if (isdigit(currChar))								//   if the character is a digit:
				{
					number = currChar - '0';
					currState = NUMBER;
					getChar();
				}
				else if (currChar == '.')
				{
					currState = DECIMAL;
					getChar();
				}
				else if (currChar == '\"')
				{
					addToBuffer(currChar);
					currState = STRING;
					getChar();
					currLexVal = checkPresence(delimTable);
					return Lexeme(LEX_QUOTE, currLexVal);
				}     
				else if (currChar == '/')                               //   if the character is start of a comment:
				{
					clearBuffer();
					addToBuffer(currChar);
					getChar();
					switch (currChar)
					{
						case '/':                                       //     in case of one-line comment:
							clearBuffer();
							currState = COMMENT_SINGLE;
							getChar();
							break;

						case '*':                                       //     in case of multiple-line comment:
							clearBuffer();
							currState = COMMENT_MULTI;
							getChar();
							break;
						
						default:										//     in case of not a comment:
							ungetChar();								//       return the current character back into the input stream
							currState = DELIM;
							currChar = '/';								//       return to analysing '/' as delimeter
							break;
					}
				}
				else if (currChar == '!')								//   if the character is 'not equal' sign:
				{
					clearBuffer();
					addToBuffer(currChar);
					currState = NOT_EQ;
					getChar();
				}
				else if (currChar == EOF)								//   if the character is end of file:
				{
					currState = END_OF_FILE;
				}
				else                                                    //    else: the character is a part of a delimeter
				{
					clearBuffer();
					addToBuffer(currChar);
					currState = DELIM;
				}
				break;
			
			case IDENT:													// Identifier state:
				if (isalpha(currChar) || isdigit(currChar))             //   if the character is alphabetic or a number:
				{
					addToBuffer(currChar);								//     add it to the buffer as a part of an identifier
					getChar();
				}
				else                                                    //   else: the identifier is finished
				{
					currState = INIT;									//     switch back to the initial state
					currLexVal = checkPresence(wordTable);
					if (currLexVal > 0)									//     if a string in buffer has a match in a functional words table:
					{
						return Lexeme((lexemeType) currLexVal, currLexVal);//       return the functional word as a lexeme
					}
					else                                                //     else: the string in bufer is an identifier
					{
						currLexVal = addUniqueId(buffer);				//       add it to the table
						return Lexeme(LEX_ID, currLexVal);
					}
				}
				break;
			
			case NUMBER:                                                // Number state
				if (isdigit(currChar))                                  //   if the character is a digit:
				{
                    int newDigit = currChar - '0';
					number = 10 * number + newDigit;                    //     continue the number
					getChar();
				}
				else if (currChar == '.')								//   if a decimal is encountered:
				{
					currState = DECIMAL;								//     switch to decimal state
					getChar();
				}
				else                                                    //   else: the number is finished
				{
					currState = INIT;
					return Lexeme(LEX_INT_NUM, number);                 //     return the number as a lexeme
				}
				break;

			case DECIMAL:												// Decimal state
				if (isdigit(currChar))                                  //   if the character is a digit:
				{
					int newDigit = currChar - '0';
					decimal = 10 * decimal + newDigit;                  //     continue the number after decimal
					getChar();
				}
				else                                                    //   else: the number after decimal is finished
				{
					currState = INIT;
					int decimalDigitsCount = to_string(decimal).length();
					double fullNumber = number + (double) decimal / pow(10, decimalDigitsCount);// combine the whole value with the value after decimal
					return Lexeme(LEX_REAL_NUM, fullNumber);			//     return the number as a lexeme
				}
				break;
				
			case STRING:												// String state
				clearBuffer();
				if (currChar != '\"')									// if the character is NOT a finishing quote:
				{
					while (currChar != '\"')							//   consider each next character a part of string until a finishing quote is met
					{
						if (currChar == '\\')							//   if the character is a control character:
						{
							getChar();
							switch(currChar)							//     check the next character
							{
								case 'n':								//      new line character
									addToBuffer('\n');
									break;
								
								case '0':								//		end of line character
									addToBuffer('\0');
									break;
								
								case 'r':								//      carriage return
									addToBuffer('\r');
									break;
								
								case 't':								//      horizontal tab character
									addToBuffer('\t');
									break;
								
								case '\\': case '\'': case '\"': case '\?': case '\%':// standard characters
									addToBuffer(currChar);
									break;
								
								case '\n':								//	    string's continuation in the next line of the code
									getChar();
									while (currChar == ' ')				//		while the character is not horizontal tab used for code allignment:	
									{
										getChar();						//        get next character
									}
									ungetChar();						//		unget character that is not a tab
									break;
								
								default:								//      wrong control character
									string wrongToken = "\\" + string(1, currChar);
									lexicalError(wrongToken);
									break;
							}
						}
						else if (currChar == '\n')						//   else: if the character is an end of line:
						{
							lexicalError("\"");							//     lexical error: no finishing quote
						}
						else											//    else: the character is a part of string
						{
							addToBuffer(currChar);						//      so add it to the buffer
						}
						getChar();
					}													//   when a finishing quote is met, the string is complete
					currLexVal = addUniqueStrConst(buffer);				//   add the completed string to the identifiers table
					return Lexeme(LEX_STR_CONST, currLexVal);
				}
				else													// if the character is a finishing quote
				{
					addToBuffer(currChar);
					getChar();
					currState = INIT;									//   go out of the string state
					currLexVal = checkPresence(delimTable);
					return Lexeme(LEX_QUOTE, currLexVal);				//   add finishing quote to the identifiers table
				}
				break;
			
			case COMMENT_SINGLE:                                        // One-line comment state
				while (currChar != '\n' && currChar != EOF)
				{
					getChar();											//   get next characters until the end of line or the end of file is met
				}
				if (currChar == EOF)									//   if the current character is end of file
				{
					currState = END_OF_FILE;							//     change the state to end of file
				}
				else													//   else: return to the initial state and get the next character
				{
					currState = INIT;
					getChar();
				}
				break;

			case COMMENT_MULTI:											// Multi-line comment state
				addToBuffer(currChar);
				if (currChar == '*')
				{
					getChar();
					if (currChar == '/')								//   if the multi-line comment is closed
					{
						addToBuffer(currChar);
						getChar();
						currState = INIT;								//     return to the initial state
					}
					else
					{
						ungetChar();
					}
				}
				if (currChar == EOF)
				{
					currState = END_OF_FILE;
				}
				else
				{
					getChar();
				}	
				break;
			
			case NOT_EQ:                                                // 'not equal' sign state
				if (currChar == '=')                                    //   if the character is '='
				{
					addToBuffer(currChar);
					currState = INIT;
					getChar();
					currLexVal = checkPresence(delimTable);
					return Lexeme(LEX_NOT_EQ, currLexVal);
				}
				else                                                    //   else: lexical error
				{
					lexicalError("!");
				}
				break;
			
			case DELIM:                                                 // Delimeter state:
				char firstChar;
                firstChar = currChar;
				getChar();
                char secondChar;
                secondChar = currChar;
				
				// Composite delimeter analysis ("++", "--", "+=", "-=", "==", ">=", "<=")
				if (													// Check the composite delimeter's first character
					firstChar == '+' ||
					firstChar == '-' ||
					firstChar == '>' ||
					firstChar == '<' ||
					firstChar == '='
				) {
					switch(secondChar)									// Check the composite delimeter's second character
					{
						case '+': case '-':
							if (firstChar == secondChar)				// Check that the composite delimeter is either "++" or "--"
							{
								addToBuffer(secondChar);
								getChar();
							}
							break;

						case '=':
							addToBuffer(secondChar);					// '=' can be placed after any character of a composite delimeter
							getChar();
							break;
						
						default:
							break;
					}
				}
				currState = INIT;
				currLexVal = checkPresence(delimTable);
				if (currLexVal > 0)										// if a string in buffer has a match in a delimeters table:
				{
					return Lexeme((lexemeType)(currLexVal + (int) LEX_EOF), currLexVal);//   return the delimeter as a lexeme
				}
				else
				{
					lexicalError("'" + buffer + "'");					// else: lexical error
				}
				break;
				
			case END_OF_FILE:
				return Lexeme(LEX_EOF);
				break;
		}
	}
	while (true);
};

vector<Lexeme> Scanner::getLexemesList()
{
	vector<Lexeme> lexList;
	Lexeme currLex = getLexeme();
	while (currLex.getType() != LEX_EOF)
	{
		lexList.push_back(currLex);
		currLex = getLexeme();
	}
	lexList.push_back(currLex);
	return lexList;
};