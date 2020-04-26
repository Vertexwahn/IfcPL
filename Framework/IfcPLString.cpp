#include "IfcPLString.h"

IfcPLString operator+(const int c, IfcPLString str) {
	std::stringstream ss;
	ss << c;
	ss << str->toCStr();
	return IfcPLString(ss.str());
}

IfcPLString operator+(const double c, IfcPLString str) {
	std::stringstream ss;
	ss << c;
	ss << str->toCStr();
	return IfcPLString(ss.str());
}

IfcPLString operator+(IfcPLString str, const double c) {
	std::stringstream ss;
	ss << str->toCStr();
	ss << c;
	return IfcPLString(ss.str());
}

bool operator==(IfcPLString const& lhs, IfcPLString const& rhs) {
	return lhs.toStdString() == rhs.toStdString();
}