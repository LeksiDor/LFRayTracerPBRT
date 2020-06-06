#ifndef PBRT_SAMPLERS_LFSAMPLER_H
#define PBRT_SAMPLERS_LFSAMPLER_H

#include "random.h"


namespace lfrt { class SampleGenerator; }

namespace pbrt {

class LFSampler : public RandomSampler
{
public:

	using SampleGen = std::unique_ptr<lfrt::SampleGenerator>;

	LFSampler( SampleGen sampleGen );

	virtual void StartPixel( const Point2i &p ) override;

	virtual CameraSample GetCameraSample( const Point2i& pRaster ) override;

	virtual Float CameraSampleWeight() override { return weight; }

	virtual bool StartNextSample() override;

	virtual std::unique_ptr<Sampler> Clone(int seed) override;

private:
	SampleGen sampleGenerator;
	Float weight = 1;
};


Sampler* CreateLFSampler( const lfrt::SampleGenerator* sampleGen );


}  // namespace pbrt

#endif // PBRT_SAMPLERS_LFSAMPLER_H