#include <cmath>
#include "Tables.hpp"

using namespace std;


//_______________________________________________________SCANNER_______________________________________________________
class Scanner
{
	FILE *f;															// program file descriptor
	
	enum state
	{
		INIT,															// initial state
		IDENT,
		NUMBER,
		DECIMAL,
		STRING,
		COMMENT_SINGLE,													// one-line comment
		COMMENT_MULTI,													// multi-line comment 
		DELIM,															// delimeter
		NOT_EQ,															// not equal
		END_OF_FILE														// end of file
	};
	
    // Current state
	state currState;
	
	static string wordTable[];											// functional words table
    static lexemeType words[];
	
	static string delimTable[];											// delimeters table
	static lexemeType delims[];
	
	char currChar;														// the current character 
	string buffer;														// buffer for the string being entered
	int bufferTop;														// position of the last non-empty character in buffer
	
private:
	// Clear buffer
	void clearBuffer();
	
	// Add new character to buffer
	void addToBuffer(char newChar);
	
	// Check if a string in buffer is present in the lexemes table
	int checkPresence(const string lexemeTable[]);
	
	// Reading the next character of a model language program
	void getChar();
	
	// Pushing the current character back into the input stream
	void ungetChar();
	
	// Lexical error processing
	void lexicalError(string err);

public:
	Scanner(const string fileName);
	
	~Scanner();

	Lexeme getLexeme();

	vector<Lexeme> getLexemesList();
};