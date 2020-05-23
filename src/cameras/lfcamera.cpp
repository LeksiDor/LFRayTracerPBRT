#include "cameras/lfcamera.h"
#include "light.h"
#include "paramset.h"
#include "sampler.h"
#include "sampling.h"
#include "stats.h"

#include "LFRayTracer.h"


namespace pbrt
{




LFCamera::LFCamera(
    const AnimatedTransform& CameraToWorld,
    Float shutterOpen,
    Float shutterClose,
    Film* film,
    const Medium* medium,
    const Float& scale,
    lfrt::RayGenerator* raygen )
    :Camera( CameraToWorld, shutterOpen, shutterClose, film, medium )
    ,Scale(scale)
    ,RayGen(raygen)
{
}


LFCamera::~LFCamera() {}


Float LFCamera::GenerateRay( const CameraSample& sample, Ray* ray ) const
{
    // sample.pFilm in [0,imageRes.x]x[0,imageRes.y].
    // sample.pLens in [0,1]x[0,1].
    // sample.time  in [0,1]x[0,1].
    return Float();
}


LFCamera* CreateLFCamera(
    const ParamSet& params,
    const AnimatedTransform& cam2world,
    Film* film,
    const Medium* medium, const std::string& mode,
    lfrt::RayGenerator* raygen )
{
    return nullptr;
}

}  // namespace pbrt