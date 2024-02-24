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
	int value;
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
	
	int getValue()
	{
		return value;
	}
	
	string getStringValue()
	{
		return strValue;
	}
	
	void setValue(int newValue)
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
vector <Identifier> identTable;											// Identifiers table (vectorised)
vector <string> strConstTable;	    									// String constants table (vectorised)

// Filling the identifier table with unique entries
int addUniqueIdent(const string name)
{
	auto isPresent = [name](Identifier id) { return id.getName() == name; };
    auto it = std::find_if(identTable.begin(), identTable.end(), isPresent);
        
    if (it != identTable.end())                                         // if an identifier with this name is already present in the table:
        return distance(identTable.begin(), it);                        // return its position in the table
    identTable.push_back(Identifier(name));				  				// else: add the ID in the end of the table
    return identTable.size() - 1;							    	    // and return its position
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
	identTable.clear();
	strConstTable.clear();
}