#include "CustomSampleGen.h"

using namespace lfrt;


SampleGenerator* CustomSampleGen::Clone() const
{
    return new CustomSampleGen( numSamplesSqrt );
}


bool CustomSampleGen::ResetPixel( const Int& x, const Int& y )
{
    this->x = x;
    this->y = y;
    currentSampleInd = 0;
    return true;
}


Int CustomSampleGen::NumSamplesInPixel()
{
    return numSamplesSqrt * numSamplesSqrt;
}


bool CustomSampleGen::CurrentSample( Real& weight, VEC2& raster, VEC2& secondary, Real& time )
{
    const Real dx = Real(currentSampleInd%numSamplesSqrt) / Real(numSamplesSqrt+1);
    const Real dy = Real(currentSampleInd/numSamplesSqrt) / Real(numSamplesSqrt+1);
    weight = 1.0;
    raster = { Real(x)+dx, Real(y)+dy };
    secondary = { 0.5, 0.5 };
    time = 0.0;
    return true;
}


bool CustomSampleGen::MoveToNextSample()
{
    ++currentSampleInd;
    return currentSampleInd < numSamplesSqrt*numSamplesSqrt;
}
