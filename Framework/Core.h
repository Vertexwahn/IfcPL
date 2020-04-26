#pragma once

#include "IfcPLString.h"
#include <memory>
#include <string>

namespace Debug {
}

bool exists (IfcPLString str);
void print(const char* format, ...);
void print(const std::string& str);
void print(int value);
void print(IfcPLString str);
void print(std::shared_ptr<IfcPLString> str);