#include "lfsampler.h"
#include "camera.h"

#include "LFRayTracer.h"


namespace pbrt {



LFSampler::LFSampler( SampleGen sampleGen )
    :RandomSampler(1)
    ,sampleGenerator( std::move(sampleGen) )
{

}


void LFSampler::StartPixel( const Point2i& p )
{
    sampleGenerator->ResetPixel( p.x, p.y );
    samplesPerPixel = sampleGenerator->NumSamplesInPixel();
    RandomSampler::StartPixel(p);
}


CameraSample LFSampler::GetCameraSample( const Point2i& pRaster )
{
    lfrt::Real weight;
    lfrt::VEC2 raster;
    lfrt::VEC2 secondary;
    lfrt::Real time;
    sampleGenerator->CurrentSample( weight, raster, secondary, time );
    CameraSample res;
    res.pFilm = Point2f( raster.x, raster.y );
    res.pLens = Point2f( secondary.x, secondary.y );
    res.time = time;
    this->weight = weight;
    return res;
}


bool LFSampler::StartNextSample()
{
    const bool res1 = sampleGenerator->MoveToNextSample();
    const bool res2 = RandomSampler::StartNextSample();
    return res1 && res2;
}


std::unique_ptr<Sampler> LFSampler::Clone( int seed )
{
    LFSampler::SampleGen sampleGen( sampleGenerator->Clone() );
    LFSampler* sampler = new LFSampler( std::move(sampleGen) );
    return std::unique_ptr<Sampler>( sampler );
}


Sampler* CreateLFSampler( const lfrt::SampleGenerator* sampleGen )
{
    LFSampler::SampleGen sampler( sampleGen->Clone() );
    return new LFSampler( std::move(sampler) );
}

}  // namespace pbrt