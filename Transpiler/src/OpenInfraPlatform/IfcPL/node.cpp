/*
    Copyright (c) 2017 Technical University of Munich
    Chair of Computational Modeling and Simulation.

    TUM Open Infra Platform is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    TUM Open Infra Platform is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "node.h"

OPENINFRAPLATFORM_IFCPL_NAMESPACE_BEGIN

eAccessModfier strToAccessModfier(const char* str) {
    if (std::string(str) == "public") {
        return eAccessModfier::Public;
    }

    if (std::string(str) == "protected") {
        return eAccessModfier::Protected;
    }

    if (std::string(str) == "private") {
        return eAccessModfier::Private;
    }

    throw std::runtime_error("Unknown access modifier.");
}

std::string accessModfierToStr(const eAccessModfier am) {
    if (eAccessModfier::Public == am)
        return "public";
    if (eAccessModfier::Protected == am)
        return "protected";
    if (eAccessModfier::Private == am)
        return "private";

    throw std::runtime_error("Unknown access modifier.");
}

eNodeType Identifier::getType() const
{
    return eNodeType::Identifier;
}

OpenInfraPlatform::IfcPL::eNodeType MethodOrFunctionCall::getType() const
{
    return eNodeType::MethodCall;
}

OPENINFRAPLATFORM_IFCPL_NAMESPACE_END