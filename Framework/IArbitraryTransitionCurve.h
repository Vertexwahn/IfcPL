#pragma once

#include "buw.h"
using namespace BlueFramework::Core::Math;

namespace IArbitraryTransitionCurve {
	class IArbitraryTransitionCurve
	{
	public:
		virtual double getLength() const = 0;
		
		virtual vector2d getPostion(const double lerpParamter) const = 0;
	
		virtual double getCurvature(const double lerpParamter) const = 0;
	};	
}

using namespace IArbitraryTransitionCurve;
