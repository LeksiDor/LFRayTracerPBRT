#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif


#ifndef PBRT_CAMERAS_LFCAMERA_H
#define PBRT_CAMERAS_LFCAMERA_H

// cameras/perspective.h*
#include "camera.h"
#include "film.h"
#include "pbrt.h"


namespace lfrt { class RayGenerator; }


namespace pbrt
{

// LFCamera Declarations.
class LFCamera : public Camera
{
public:
    LFCamera(
        const AnimatedTransform& CameraToWorld,
        Float shutterOpen,
        Float shutterClose,
        Film* film,
        const Medium* medium,
        const Float& scale,
        lfrt::RayGenerator* raygen = nullptr );

      virtual ~LFCamera();

      virtual Float GenerateRay(
          const CameraSample& sample,
          Ray* ray ) const override;

      virtual Float GenerateRayDifferential(
          const CameraSample& sample,
          RayDifferential* ray ) const override;

public:
    Float Scale = 1.0;
    lfrt::RayGenerator* RayGen = nullptr;
};

LFCamera* CreateLFCamera(
    const ParamSet& params,
    const AnimatedTransform& cam2world,
    Film* film,
    const Medium* medium,
    const std::string& mode,
    lfrt::RayGenerator* raygen = nullptr );

}  // namespace pbrt

#endif  // PBRT_CAMERAS_LFCAMERA_H