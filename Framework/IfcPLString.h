#pragma once

#include <sstream>
#include <string>

class IfcPLString
{
public:
	IfcPLString() {

	}

	IfcPLString(const std::string& str) : str_(str) {

	}

	std::string toStdString() const {
		return str_;
	}
	
	operator std::string() const { return str_; }

	const char* toCStr() const {
		return str_.c_str();
	}

	char& operator[](int index)	{
		return str_.at(index);
	}

	char const& operator[](int index) const	{
		return str_.at(index);
	}

	IfcPLString operator+(const int c) const {
		std::stringstream ss;
		ss << str_;
		ss << c;
		return IfcPLString(ss.str());
	}
	
	IfcPLString operator+(const char c) const {
		std::stringstream ss;
		ss << str_;
		ss << c;
		return IfcPLString(ss.str());
	}
		
	IfcPLString operator+(IfcPLString str) const {
		std::stringstream ss;
		ss << str_;
		ss << str.str_;
		return IfcPLString(ss.str());
	}
		
	IfcPLString* operator->() {
		return this;
	}
	
	int Length() {
		return str_.size();
	}
	
private:
	std::string str_;
};

bool operator==(IfcPLString const& lhs, IfcPLString const& rhs);

IfcPLString operator+(IfcPLString str, const double c);
IfcPLString operator+(const double c, IfcPLString str);
IfcPLString operator+(const int c, IfcPLString str);