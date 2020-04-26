#pragma once

#include <string>
#include <vector>

class symbolDescription {
public:
	symbolDescription() {

	}

	symbolDescription(std::string id, std::string typeName, bool isExpressType);

	std::string id = "";
	std::string typeName = "";
	bool isExpressType = false;
	bool isArrayType = false;
};

class SymbolTable {
public:
	void add(std::string id, std::string typeName, bool isExpressType);
	void add(const symbolDescription& symbol);

	bool isStringArrayType(std::string id) const;
	bool isExpressType(std::string id) const;
	bool isArrayType(std::string id) const;

private:
	std::vector<symbolDescription> symbols;
};