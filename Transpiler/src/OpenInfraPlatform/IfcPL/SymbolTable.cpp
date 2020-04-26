#include "SymbolTable.h"

symbolDescription::symbolDescription(std::string id, std::string typeName, bool isExpressType) : id(id), typeName(typeName), isExpressType(isExpressType)
{

}

void SymbolTable::add(std::string id, std::string typeName, bool isExpressType)
{
	symbols.push_back(symbolDescription(id, typeName, isExpressType));
}

void SymbolTable::add(const symbolDescription& symbol)
{
	symbols.push_back(symbol);
}

bool SymbolTable::isExpressType(std::string id) const
{
	for (int i = 0; i < symbols.size(); i++) {
		if (symbols[i].id == id && symbols[i].isExpressType) {
			return true;
		}
	}
	return false;
}

bool SymbolTable::isStringArrayType(std::string id) const
{
	if (id == "args")
		return true; // hack - hope that nobody tries to use an args variable

	for (int i = 0; i < symbols.size(); i++) {
		if (symbols[i].id == id && symbols[i].isArrayType && symbols[i].typeName == "string") {
			return true;
		}
	}
	return false;
}

bool SymbolTable::isArrayType(std::string id) const
{
	for (int i = 0; i < symbols.size(); i++) {
		if (symbols[i].id == id && symbols[i].isArrayType) {
			return true;
		}
	}
	return false;
}
