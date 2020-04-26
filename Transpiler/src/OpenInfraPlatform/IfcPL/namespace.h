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

#pragma once
#ifndef OpenInfraPlatform_IfcPL_namespace_9f72caf0_1c8e_4686_8e41_7ca7c7e469a0_h
#define OpenInfraPlatform_IfcPL_namespace_9f72caf0_1c8e_4686_8e41_7ca7c7e469a0_h

#define OPENINFRAPLATFORM_IFCPL_NAMESPACE_BEGIN namespace OpenInfraPlatform { namespace IfcPL {
#define OPENINFRAPLATFORM_IFCPL_NAMESPACE_END }}

#define OPENINFRAPLATFORM_IFCPL_EMBED_INTO_LUX_NAMESPACE(TYPE)	\
namespace ipl			    									\
{																\
	using OpenInfraPlatform::IfcPL::TYPE;						\
}

#endif // end define OpenInfraPlatform_IfcPL_namespace_9f72caf0_1c8e_4686_8e41_7ca7c7e469a0_h