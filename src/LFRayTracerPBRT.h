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
    virtual Int NumSamplesInPixel() override;
    virtual bool NextSample( Real& weight, VEC2& raster, VEC2& secondary, Real& time ) override;
};



LFRayTracer* LFRayTracerPBRTInstance();
bool LFRayTRacerPBRTRelease();


} // namespace lfrt


#endif // LFRAYTRACERPBRT_H