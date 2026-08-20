#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include "Lexeme.hpp"


template <class T1, class T2>

// Extract item from stack
void extract(T1& stack, T2& item)
{
	item = stack.top();
	stack.pop();
}


//_____________________________________________________IDENTIFIER______________________________________________________
class Identifier
{
	std::string name;
	lexemeType type;
	
	bool declared;														// identificator that identifier is already declared
	bool assigned;														// identificator that identifier is already assigned a value
	double value;
	std::string strValue;
	
	bool label;															// identificator that identifier is a label
	int address;

public:
	Identifier(const std::string n);
	
	std::string getName() const;
	void setName(const std::string& newName);
	
	bool isDeclared() const;
	void setDeclare();
	
	lexemeType getType() const;
	void setType(lexemeType newType);
	
	bool isAssigned() const;
	void setAssign();
	
	double getValue() const;
	void setValue(double newValue);
	
    std::string getStringValue() const;
	void setValue(std::string newStrValue);
	
	bool isLabel() const;
	void setAsLabel();

	int getAddress() const;
	void setAddress(int newAddress);
};

//_______________________________________________________TABLES________________________________________________________
extern std::vector<Identifier> idTable;									// Identifiers table (vectorised)
extern std::vector<string> strConstTable;	    						// String constants table (vectorised)
extern std::vector<vector<pair<int, int>>> switchLabelTable;			// Table of labels for each switch case (vectorised) (1st int - value that identifier must be equal to, 2nd int - label in the RPN table)

// Add a unique identifier to the identifiers table
int addUniqueId(const std::string name);

// Add a unique string constant to a string constants table
int addUniqueStrConst(const std::string str);

// Clear both tables
void clearTables();

// Operation table key
struct opTableKey
{
	std::vector<lexemeType> opers;
	lexemeType opX;
	lexemeType opY;
};

// Operation rules table
struct OperationTable
{
	static std::vector<lexemeType> opRealMath;
	static std::vector<lexemeType> opIntMath;
	static std::vector<lexemeType> stringConcat;
	static std::vector<lexemeType> numCompare;
	static std::vector<lexemeType> stringCompare;
	static std::vector<lexemeType> opBool;
	static std::vector<std::pair<opTableKey, lexemeType>> opTable;

public:
    // Return type of the result of the operation (returns LEX_NULL if the operation is illegal)
	lexemeType getResultType(lexemeType opX, lexemeType opY, lexemeType oper);
};