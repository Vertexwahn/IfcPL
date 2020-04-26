/*! \verbatim
 *  \copyright      Copyright (c) 2016 Julian Amann. All rights reserved.
 *  \author         Julian Amann <dev@vertexwahn.de> (http://vertexwahn.de/)
 *  \brief          This file is part of the BlueFramework.
 *  \endverbatim
 */

#pragma once

#include "BlueFramework/Core/platform.h"
#include "BlueFramework/Core/compiler.h" // disable some compiler specific warnings

#ifdef IFCPL_EXPORT
#	if BLUE_PLATFORM == BLUE_PLATFORM_WINDOWS
#       define IFCPL_API __declspec(dllexport)
#   else
#       define IFCPL_API
#   endif
#else
#  define IFCPL_API
#endif
