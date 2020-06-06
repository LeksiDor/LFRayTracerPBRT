#ifndef CUSTOMRAYGEN_H
#define CUSTOMRAYGEN_H

#include "LFRayTracer.h"


// Simple implementation of perspective pinhole camera.
class CustomRayGen : public lfrt::RayGenerator
{
public:
	using Real = lfrt::Real;
	using Int = lfrt::Int;
	using VEC2 = lfrt::VEC2;
	using VEC3 = lfrt::VEC3;

	CustomRayGen() = default;

	CustomRayGen(
		const Int& width, const Int& height,
		const VEC2& imagePlaneHalfSize = {1.0,1.0},
		const Real& imagePlaneDepth = 1.0 );

	virtual ~CustomRayGen() = default;

	virtual Real GenerateRay(
		const VEC2& raster, const VEC2& secondary,
		VEC3& ori, VEC3& dir ) const override;

	Real ImagePlaneDepth = 1.0;
	VEC2 ImagePlaneHalfSize = { 1.0, 1.0 };
	Int Width = 512;
	Int Height = 512;
};



#endif // CUSTOMRAYGEN_H