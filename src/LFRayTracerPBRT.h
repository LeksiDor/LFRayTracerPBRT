#ifndef LFRAYTRACERPBRT_H
#define LFRAYTRACERPBRT_H

#include "LFRayTracer.h"


namespace lfrt
{

class LFRayTracer;


class DefaultRayGenerator : public RayGenerator
{
public:
    virtual Real GenerateRay( const VEC2& raster, const VEC2& secondary, VEC3& ori, VEC3& dir ) const override;
};


class DefaultSampleGenerator : public SampleGenerator
{
public:
    virtual SampleGenerator* Clone() const override;
    virtual bool ResetPixel( const Int& x, const Int& y ) override;
    virtual bool NextSample( Real& weight, VEC2& raster, VEC2& secondary, Real& time ) override;
};


class DefaultSampleAccumulator : public SampleAccumulator
{
public:
    virtual bool SetSize( const Int& width, const Int& height ) override;
    virtual bool GetSize( Int& width, Int& height ) const override;
    virtual SampleTile* CreateSampleTile( const Int& startX, const Int& startY, const Int& sizeX, const Int& sizeY ) override;
    virtual bool MergeSampleTile( SampleTile* tile ) override;
    virtual bool DestroySampleTile( SampleTile* tile ) override;
    virtual bool GetColor( const Int& x, const Int& y, Real& r, Real& g, Real& b ) const override;
private:
    Int m_Width  = 0;
    Int m_Height = 0;
};


LFRayTracer* LFRayTracerPBRTInstance();
bool LFRayTRacerPBRTRelease();


} // namespace lfrt


#endif // LFRAYTRACERPBRT_H