#include <iostream>
#include "Lexeme.hpp"

using namespace std;

Lexeme::Lexeme(lexemeType t, double v) : type(t), value(v) {}

lexemeType Lexeme::getType() const
{
	return type;
}
	
double Lexeme::getValue() const
{
	return value;
}

// DEBUG
ostream& operator << (ostream &out, Lexeme lex)
{
	out << '(' << lex.type << ',' << lex.value << "); ";
	return out;
}