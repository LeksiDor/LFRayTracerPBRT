#ifndef CUSTOMSAMPLEGEN_H
#define CUSTOMSAMPLEGEN_H

#include "LFRayTracer.h"


// Simple uniform non-random sampler.
class CustomSampleGen : public lfrt::SampleGenerator
{
public:
    using Int = lfrt::Int;
    using Real = lfrt::Real;
    using VEC2 = lfrt::VEC2;

    CustomSampleGen(const Int numSamplesSqrt = 1) : numSamplesSqrt(numSamplesSqrt) {}

    virtual SampleGenerator* Clone() const override;

    virtual bool ResetPixel( const Int& x, const Int& y ) override;

    virtual Int NumSamplesInPixel() override;

    virtual bool CurrentSample( Real& weight, VEC2& raster, VEC2& secondary, Real& time ) override;

    virtual bool MoveToNextSample() override;

    Int numSamplesSqrt = 1;
    Int currentSampleInd = 0;
    Int x;
    Int y;
};


#endif // CUSTOMSAMPLEGEN_H