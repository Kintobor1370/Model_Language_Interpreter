#include "Tables.hpp"

using namespace std;


//_____________________________________________________IDENTIFIER______________________________________________________
Identifier::Identifier(const string n): declared(false), assigned(false), label(false), name(n)
{
	value = -1;
	address = -1;
}
	
string Identifier::getName() const
{
	return name;
}
	
void Identifier::setName(const string& newName)
{
	name = newName;
}
	
bool Identifier::isDeclared() const
{
	return declared;
}
	
void Identifier::setDeclare()
{
	declared = true;
}
	
lexemeType Identifier::getType() const
{
	return type;
}
	
void Identifier::setType(lexemeType newType)
{
	type = newType;
}
	
bool Identifier::isAssigned() const
{
	return assigned;
}
	
void Identifier::setAssign()
{
	assigned = true;
}
	
double Identifier::getValue() const
{
	return value;
}
	
void Identifier::setValue(double newValue)
{
	value = newValue;
}

string Identifier::getStringValue() const
{
	return strValue;
}
	
void Identifier::setValue(string newStrValue)
{
	strValue = newStrValue;
}
	
bool Identifier::isLabel() const
{
	return label;
}
	
void Identifier::setAsLabel()
{
	label = true;
	declared = true;
}

int Identifier::getAddress() const
{
	return address;
}

void Identifier::setAddress(int newAddress)
{
	address = newAddress;
}

//_______________________________________________________TABLES________________________________________________________
vector<Identifier> idTable;												// Identifiers table (vectorised)
vector<string> strConstTable;	    									// String constants table (vectorised)
vector<vector<pair<int, int>>> switchLabelTable;						// Table of labels for each switch case (vectorised) (1st int - value that identifier must be equal to, 2nd int - label in the RPN table)

int addUniqueId(const string name)
{
	auto isPresent = [name](Identifier id) { 
		return id.getName() == name; 
	};
    auto it = std::find_if(idTable.begin(), idTable.end(), isPresent);
        
    if (it != idTable.end())                                        	// if an identifier with this name is already present in the table:
	{
        return distance(idTable.begin(), it);                       	// return its position in the table
	}
	idTable.push_back(Identifier(name));				  				// else: add the ID in the end of the table
    return idTable.size() - 1;							    	    	// and return its position
}

int addUniqueStrConst(const string str)
{
	auto it = find(strConstTable.begin(), strConstTable.end(), str);

    if (it != strConstTable.end())            	    		    		// if the current string is already present in the table:
	{
        return distance(strConstTable.begin(), it);   	    	      	// return its position in the table
	}
	strConstTable.push_back(str);             					    	// else: add the string in the end of the table
    return strConstTable.size() - 1;      					        	// and return its position
}

void clearTables()
{
	idTable.clear();
	strConstTable.clear();
}

// Operation rules table
vector<lexemeType> OperationTable::opRealMath = { 
	LEX_PLUS, 
	LEX_MINUS, 
	LEX_TIMES, 
	LEX_SLASH 
};
vector<lexemeType> OperationTable::opIntMath = { 
	LEX_PLUS, 
	LEX_MINUS, 
	LEX_TIMES, 
	LEX_SLASH, 
	LEX_PERCENT 
};
vector<lexemeType> OperationTable::stringConcat = { 
	LEX_PLUS 
};
vector<lexemeType> OperationTable::numCompare = { 
	LEX_GREATER, 
	LEX_LESS, 
	LEX_GREATER_EQ, 
	LEX_LESS_EQ, 
	LEX_EQ, 
	LEX_NOT_EQ 
};
vector<lexemeType> OperationTable::stringCompare = { 
	LEX_GREATER, 
	LEX_LESS, 
	LEX_EQ, 
	LEX_NOT_EQ 
};
vector<lexemeType> OperationTable::opBool = { 
	LEX_OR, 
	LEX_AND 
};

vector<pair<opTableKey, lexemeType>> OperationTable::opTable =
{
	{ { opIntMath, LEX_INT, LEX_INT }, LEX_INT },
	{ { opRealMath, LEX_REAL, LEX_REAL }, LEX_REAL },
	{ { opRealMath, LEX_INT, LEX_REAL }, LEX_REAL },
	{ { opRealMath, LEX_REAL, LEX_INT }, LEX_REAL },
	{ { stringConcat, LEX_STRING, LEX_STRING }, LEX_STRING },
	{ { numCompare, LEX_INT, LEX_INT }, LEX_BOOL },
	{ { numCompare, LEX_REAL, LEX_REAL }, LEX_BOOL },
	{ { numCompare, LEX_INT, LEX_REAL }, LEX_BOOL },
	{ { numCompare, LEX_REAL, LEX_INT }, LEX_BOOL },
	{ { stringCompare, LEX_STRING, LEX_STRING }, LEX_BOOL },
	{ { opBool, LEX_BOOL, LEX_BOOL }, LEX_BOOL }
};

lexemeType OperationTable::getResultType(lexemeType opX, lexemeType opY, lexemeType oper)
{
	auto isPresent = [opX, opY, oper](pair<opTableKey, lexemeType> entry)
	{
		vector<lexemeType> allowedOperators = entry.first.opers;
		auto operIt = find(allowedOperators.begin(), allowedOperators.end(), oper);
		bool operatorIsAllowed = operIt != allowedOperators.end();
		return opX == entry.first.opX && opY == entry.first.opY && operatorIsAllowed;
	};
	auto it = find_if(opTable.begin(), opTable.end(), isPresent);
	if (it != opTable.end())
	{
		lexemeType resType = opTable.at(it - opTable.begin()).second;
		return resType;
	}
	return LEX_NULL;
}