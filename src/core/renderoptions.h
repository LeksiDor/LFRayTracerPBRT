#ifndef PBRT_RENDEROPTIONS_H
#define PBRT_RENDEROPTIONS_H

#include "transform.h"
#include "paramset.h"


namespace pbrt
{


PBRT_CONSTEXPR int MaxTransforms = 2;
PBRT_CONSTEXPR int StartTransformBits = 1 << 0;
PBRT_CONSTEXPR int EndTransformBits = 1 << 1;
PBRT_CONSTEXPR int AllTransformsBits = (1 << MaxTransforms) - 1;
struct TransformSet {
    // TransformSet Public Methods
    Transform &operator[](int i) {
        CHECK_GE(i, 0);
        CHECK_LT(i, MaxTransforms);
        return t[i];
    }
    const Transform &operator[](int i) const {
        CHECK_GE(i, 0);
        CHECK_LT(i, MaxTransforms);
        return t[i];
    }
    friend TransformSet Inverse(const TransformSet &ts) {
        TransformSet tInv;
        for (int i = 0; i < MaxTransforms; ++i) tInv.t[i] = Inverse(ts.t[i]);
        return tInv;
    }
    bool IsAnimated() const {
        for (int i = 0; i < MaxTransforms - 1; ++i)
            if (t[i] != t[i + 1]) return true;
        return false;
    }

  private:
    Transform t[MaxTransforms];
};


struct RenderOptions
{
    Float transformStartTime = 0, transformEndTime = 1;
    std::string FilterName = "box";
    ParamSet FilterParams;
    std::string FilmName = "image";
    ParamSet FilmParams;
    std::string SamplerName = "halton";
    ParamSet SamplerParams;
    std::string AcceleratorName = "bvh";
    ParamSet AcceleratorParams;
    std::string IntegratorName = "path";
    ParamSet IntegratorParams;
    std::string CameraName = "perspective";
    ParamSet CameraParams;
    TransformSet CameraToWorld;
};


RenderOptions& theRenderOptions();

} // namespace pbrt

#endif  // PBRT_RENDEROPTIONS_H