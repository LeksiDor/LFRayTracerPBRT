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
namespace lfrt { class SampleAccumulator; }


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
        lfrt::SampleAccumulator* film,
        const Medium* medium,
        const Float& scale,
        const lfrt::RayGenerator* raygen );

      virtual ~LFCamera();

      virtual Float GenerateRay(
          const CameraSample& sample,
          Ray* ray ) const override;

      virtual Float GenerateRayDifferential(
          const CameraSample& sample,
          RayDifferential* ray ) const override;

public:
    Float Scale = 1.0;
    const lfrt::RayGenerator* RayGen = nullptr;
};

LFCamera* CreateLFCamera(
    const ParamSet& params,
    const AnimatedTransform& cam2world,
    lfrt::SampleAccumulator* film,
    const Medium* medium,
    const lfrt::RayGenerator* raygen );

}  // namespace pbrt

#endif  // PBRT_CAMERAS_LFCAMERA_H