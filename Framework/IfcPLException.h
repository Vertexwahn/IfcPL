#pragma once

#include "IfcPLString.h"
#include <stdexcept>

class Exception : std::runtime_error {
public:
	Exception(const std::string& message) : runtime_error(message.c_str()) {

	}

	Exception(std::shared_ptr<IfcPLString> message) : runtime_error(message->toCStr()) {

	}

	Exception(IfcPLString message) : runtime_error(message.toCStr()) {

	}
};