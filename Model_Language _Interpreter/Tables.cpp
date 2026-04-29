#include <vector>
#include <algorithm>
#include "Lexeme.cpp"

using namespace std;


//_____________________________________________________IDENTIFIER______________________________________________________
class Identifier
{
	string name;
	lexemeType type;
	
	bool declared;														// identificator that identifier is already declared
	bool assigned;														// identificator that identifier is already assigned a value
	double value;
	string strValue;
	
	bool label;															// identificator that identifier is a label
	int address;

public:
	Identifier(const string n): declared(false), assigned(false), label(false), name(n)
	{
		value = -1;
		address = -1;
	}
	
	string getName()
	{
		return name;
	}
	
	void setName(const string newName)
	{
		name = newName;
	}
	
	bool isDeclared()
	{
		return declared;
	}
	
	void setDeclare()
	{
		declared = true;
	}
	
	lexemeType getType()
	{
		return type;
	}
	
	void setType(lexemeType newType)
	{
		type = newType;
	}
	
	bool isAssigned()
	{
		return assigned;
	}
	
	void setAssign()
	{
		assigned = true;
	}
	
	double getValue()
	{
		return value;
	}
	
	string getStringValue()
	{
		return strValue;
	}
	
	void setValue(double newValue)
	{
		value = newValue;
	}
	
	void setValue(string newStrValue)
	{
		strValue = newStrValue;
	}
	
	bool isLabel()
	{
		return label;
	}
	
	void setAsLabel()
	{
		label = true;
		declared = true;
	}

	int getAddress()
	{
		return address;
	}

	void setAddress(int newAddress)
	{
		address = newAddress;
	}
};

//_______________________________________________________TABLES________________________________________________________
vector<Identifier> idTable;												// Identifiers table (vectorised)
vector<string> strConstTable;	    									// String constants table (vectorised)

// Filling the identifier table with unique entries
int addUniqueIdent(const string name)
{
	auto isPresent = [name](Identifier id) { return id.getName() == name; };
    auto it = std::find_if(idTable.begin(), idTable.end(), isPresent);
        
    if (it != idTable.end())                                        	// if an identifier with this name is already present in the table:
        return distance(idTable.begin(), it);                       	// return its position in the table
    idTable.push_back(Identifier(name));				  				// else: add the ID in the end of the table
    return idTable.size() - 1;							    	    	// and return its position
}

// Filling the sting constants table with unique entries
int addUniqueStrConst(const string str)
{
	auto it = find(strConstTable.begin(), strConstTable.end(), str);

    if (it != strConstTable.end())            	    		    		// if the current string is already present in the table:
        return distance(strConstTable.begin(), it);   	    	      	// return its position in the table
    strConstTable.push_back(str);             					    	// else: add the string in the end of the table
    return strConstTable.size() - 1;      					        	// and return its position
}

// Clear both tables
void clearTables()
{
	idTable.clear();
	strConstTable.clear();
}

// Operation rules table
class OperationTable
{
	vector<lexemeType> opRealMath = { LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH };
	vector<lexemeType> opIntMath = { LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_PERCENT };
	vector<lexemeType> stringConcat = { LEX_PLUS };
	vector<lexemeType> numCompare = { LEX_GREATER, LEX_LESS, LEX_GREATER_EQ, LEX_LESS_EQ, LEX_EQ, LEX_NOT_EQ };
	vector<lexemeType> stringCompare = { LEX_GREATER, LEX_LESS, LEX_EQ, LEX_NOT_EQ };
	vector<lexemeType> opBool = { LEX_OR, LEX_AND };

	struct opTableKey
	{
		vector<lexemeType> opers;
		lexemeType opX;
		lexemeType opY;
	};

	vector<pair<opTableKey, lexemeType>> opTable =
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

public:
	OperationTable() {}
	lexemeType getResultType(lexemeType opX, lexemeType opY, lexemeType oper);
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