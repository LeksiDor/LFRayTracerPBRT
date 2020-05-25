#ifndef LFRAYTRACER_H
#define LFRAYTRACER_H

#include <string>


namespace lfrt
{


class SampleGenerator;
class SampleTile;
class SampleAccumulator;
class LFRayTracer;
class RayGenerator;

using Real = double;
using Int = int;


struct VEC2i { Int x; Int y; };
struct VEC2 { Real x; Real y; };
struct VEC3 { Real x; Real y; Real z; };



// Generates sequence of samples for each pixel.
// Should be called sequentially for each pixel.
// If another pixel is set, it should reset itself.
// Is not thread-safe.
// For multi-threading, SampleGenerator should be cloned for each thread.
class SampleGenerator
{
public:
    virtual ~SampleGenerator() = default;
	virtual SampleGenerator* Clone() const = 0;
    virtual bool ResetPixel( const Int& x, const Int& y ) = 0;
	virtual bool NextSample( Real& weight, VEC2& raster, VEC2& secondary, Real& time ) = 0;
};


// Container for all accumulated samples within a local rectangular pixel area.
// May use memory allocated by SampleAccumulator.
// Should be created and destroyed by SampleAccumulator.
// Should be used inside one thread.
class SampleTile
{
public:
	virtual ~SampleTile() = default;
	virtual bool AddSample(
		const VEC2& raster,
		const VEC2& secondary,
		const Real& sampleWeight,
		const Real& rayWeight,
		const Real& r, const Real& g, const Real& b
	) = 0;
};


class SampleAccumulator
{
public:
	virtual ~SampleAccumulator() = default;
	virtual bool SetSize( const Int& width, const Int& y ) = 0;
    virtual bool GetSize( Int& x, Int& y ) const = 0;
	virtual SampleTile* CreateSampleTile( const Int& startX, const Int& startY, const Int& sizeX, const Int& sizeY ) = 0;
    virtual bool MergeSampleTile(SampleTile* tile) = 0;
	virtual bool DestroySampleTile( SampleTile* tile ) = 0;
	virtual bool GetColor( const Int& x, const Int& y, Real& r, Real& g, Real& b ) const = 0;
};


class RayGenerator
{
public:
	virtual ~RayGenerator() = default;

	virtual Real GenerateRay(
		const VEC2& raster, // Coordinates from [0,Width]x[0,Height].
		const VEC2& secondary, // Coordinates from [0,1]x[0,1].
		VEC3& ori, VEC3& dir
	) const = 0;

	virtual Real GenerateRayDifferential(
		const VEC2& raster, // Coordinates from [0,Width]x[0,Height].
		const VEC2& secondary, // Coordinates from [0,1]x[0,1].
		VEC3& ori, VEC3& dir,
		VEC3& oridx, VEC3& dirdx,
		VEC3& oridy, VEC3& dirdy
	) const;
};


// This define should be used in RT-side library only.
#ifdef LFRAYTRACER_IMPLEMENTATION
Real RayGenerator::GenerateRayDifferential(
    const VEC2& raster,     // Coordinates from [0,Width]x[0,Height].
    const VEC2& secondary,  // Coordinates from [0,1]x[0,1].
    VEC3& ori, VEC3& dir, VEC3& oridx, VEC3& dirdx, VEC3& oridy, VEC3& dirdy ) const
{
    const Real epsilon = 0.01;
    const Real weight = GenerateRay( raster, secondary, ori, dir );
	if ( weight <= 0 )
	{
        oridx = dirdx = oridy = dirdy = { 0, 0, 0 };
        return weight;
	}
    VEC3 ori1 = ori;
	VEC3 dir1 = dir;
    VEC2 raster1;
	raster1.x = raster.x + epsilon;
    raster1.y = raster.y;
    Real weight1 = GenerateRay( raster1, secondary, ori1, dir1 );
	oridx.x = (ori1.x - ori.x) / epsilon;
    oridx.y = (ori1.y - ori.y) / epsilon;
	oridx.z = (ori1.z - ori.z) / epsilon;
    dirdx.x = (dir1.x - dir.x) / epsilon;
    dirdx.y = (dir1.y - dir.y) / epsilon;
    dirdx.z = (dir1.z - dir.z) / epsilon;
    if ( weight1 <= 0 ) oridx = dirdx = { 0, 0, 0 };
    raster1.x = raster.x;
    raster1.y = raster.y + epsilon;
	weight1 = GenerateRay( raster1, secondary, ori1, dir1 );
	oridy.x = (ori1.x - ori.x) / epsilon;
    oridy.y = (ori1.y - ori.y) / epsilon;
	oridy.z = (ori1.z - ori.z) / epsilon;
    dirdy.x = (dir1.x - dir.x) / epsilon;
    dirdy.y = (dir1.y - dir.y) / epsilon;
    dirdy.z = (dir1.z - dir.z) / epsilon;
    if ( weight1 <= 0 ) oridx = dirdx = { 0, 0, 0 };
    return weight;
}
#endif // LFRAYTRACER_IMPLEMENTATION




class LFRayTracer
{
public:
	virtual ~LFRayTracer() = default;
	virtual bool LoadScene( const std::string& filepath ) = 0;
	virtual RayGenerator* CreateDefaultRayGenerator( const Int& width, const Int& height ) const = 0;
	virtual SampleGenerator* CreateDefaultSampleGenerator( const Int& width, const Int& height ) const = 0;
	virtual SampleAccumulator* CreateDefaultSampleAccumulator( const Int& width, const Int& height ) const = 0;
	virtual bool Render( const RayGenerator& raygen, SampleGenerator& sampleGen, SampleAccumulator& sampleAccum ) const = 0;
};


} // namespace lfrt

#endif // LFRAYTRACER_H