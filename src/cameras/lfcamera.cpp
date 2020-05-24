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
    // sample.pFilm in [0,Width]x[0,Height].
    // sample.pLens in [0,1]x[0,1].
    // sample.time  in [0,1]x[0,1].
    ProfilePhase prof( Prof::GenerateCameraRay );
    if ( RayGen == nullptr )
    {
        return 0;
    }

    // Evaluate ray origin and direction depending on LF model.
    lfrt::VEC3 ori;
    lfrt::VEC3 dir;
    const Float weight = RayGen->GenerateRay(
        {sample.pFilm.x,sample.pFilm.y},
        {sample.pLens.x,sample.pLens.y},
        ori, dir );
    ray->o = Scale * Point3f( ori.x, ori.y, ori.z );
    ray->d = Vector3f( dir.x, dir.y, dir.z );
    const Float raydirLength = ray->d.Length();
    if ( raydirLength == 0 )
    {
        return 0;
    }
    ray->d = ray->d / raydirLength;

    // Apply PBRT-related ray parameters.
    ray->time = Lerp( sample.time, shutterOpen, shutterClose );
    ray->medium = medium;
    *ray = CameraToWorld(*ray);
    return weight;
}


Float LFCamera::GenerateRayDifferential( const CameraSample& sample, RayDifferential* ray ) const
{
    // sample.pFilm in [0,Width]x[0,Height].
    // sample.pLens in [0,1]x[0,1].
    // sample.time  in [0,1]x[0,1].
    ProfilePhase prof( Prof::GenerateCameraRay );
    if ( RayGen == nullptr )
    {
        return 0;
    }

    // Evaluate ray origin and direction depending on LF model.
    lfrt::VEC3 ori;
    lfrt::VEC3 dir;
    lfrt::VEC3 oridx;
    lfrt::VEC3 dirdx;
    lfrt::VEC3 oridy;
    lfrt::VEC3 dirdy;
    const Float weight = RayGen->GenerateRayDifferential(
        {sample.pFilm.x,sample.pFilm.y},
        {sample.pLens.x,sample.pLens.y},
        ori, dir, oridx, dirdx, oridy, dirdy );
    ray->o = Scale * Point3f( ori.x, ori.y, ori.z );
    ray->d = Vector3f( dir.x, dir.y, dir.z );
    const Float raydirLength = ray->d.Length();
    if ( raydirLength == 0 )
    {
        return 0;
    }
    ray->d = ray->d / raydirLength;

    ray->rxOrigin = ray->o + Scale * Point3f( oridx.x, oridx.y, oridx.z );
    ray->ryOrigin = ray->o + Scale * Point3f( oridy.x, oridy.y, oridy.z );

    ray->rxDirection = Normalize( Vector3f( dir.x + dirdx.x, dir.y + dirdx.y, dir.z + dirdx.z ) );
    ray->ryDirection = Normalize( Vector3f( dir.x + dirdy.x, dir.y + dirdy.y, dir.z + dirdy.z ) );

    // Apply PBRT-related ray parameters.
    ray->time = Lerp( sample.time, shutterOpen, shutterClose );
    ray->medium = medium;
    *ray = CameraToWorld(*ray);
    ray->hasDifferentials = true;
    return weight;
}


LFCamera* CreateLFCamera(
    const ParamSet& params,
    const AnimatedTransform& cam2world,
    Film* film,
    const Medium* medium, const std::string& mode,
    lfrt::RayGenerator* raygen )
{
    // Extract common camera parameters from _ParamSet_
    Float shutteropen = params.FindOneFloat( "shutteropen", 0 );
    Float shutterclose = params.FindOneFloat( "shutterclose", 1 );
    const Float scale = params.FindOneFloat( "scale", 1 );

    if (shutterclose < shutteropen) {
        Warning("Shutter close time [%f] < shutter open [%f].  Swapping them.",
                shutterclose, shutteropen);
        std::swap(shutterclose, shutteropen);
    }

    return new LFCamera( cam2world, shutteropen, shutterclose, film, medium, scale, raygen );
}

}  // namespace pbrt