#include "Core.h"
#include <iostream>
#include <cstdarg>
#include <fstream>
#include <cassert>

bool exists (IfcPLString str) {
    std::ifstream f(str.toCStr());
    return f.good();
}

void print(const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr, format, argptr);
	va_end(argptr);
}

void print(const std::string& str) {
	std::cout << str.c_str() << std::endl;
}

void print(int value) {
	std::cout << value << std::endl;
}

void print(IfcPLString str) {
	std::cout << str.toCStr() << std::endl;
}

void print(std::shared_ptr<IfcPLString> str) {
	assert(false);
	// should never be called, since a string should be mapped to IfcPLString
	// and not to std::shared_ptr<IfcPLString>
	std::cout << str->toCStr() << std::endl;
}