#include "LFRayTracerPBRT.h"

#include "LFRayTracer.h"

#include "api.h"

#include "Film.h"


using namespace pbrt;


namespace lfrt
{





class LFRayTracerPBRTImpl : public LFRayTracer
{
public:
    LFRayTracerPBRTImpl()
    {
        google::InitGoogleLogging(""); // To actually enable this, call it from main.cpp with 'argv[0]' argument.
        FLAGS_stderrthreshold = 1;  // Warning and above.
        Options options;
        pbrtInit( options );
    }

    virtual ~LFRayTracerPBRTImpl()
    {
        pbrtCleanup();
    }


public:

    virtual bool LoadScene( const std::string& filepath ) override
    {
        pbrtParseFile( filepath );
        return true;
    }

    virtual RayGenerator* CreateDefaultRayGenerator(
        const Int& width, const Int& height ) const override
    {
        return new DefaultRayGenerator();
    }

    virtual SampleGenerator* CreateDefaultSampleGenerator(
        const Int& width, const Int& height ) const override
    {
        return new DefaultSampleGenerator();
    }

    virtual SampleAccumulator* CreateDefaultSampleAccumulator(
        const Int& width, const Int& height ) const override
    {
        return theFilm();
    }

    virtual bool Render( const RayGenerator& raygen, SampleGenerator& sampleGen,
                        SampleAccumulator& sampleAccum ) const override
    {
        pbrtRenderScene();
        return true;
    }

public:

};



static std::shared_ptr<LFRayTracerPBRTImpl> raytracerInstance = nullptr;


LFRayTracer* LFRayTracerPBRTInstance()
{
    if ( raytracerInstance == nullptr )
    {
        raytracerInstance = std::make_shared<LFRayTracerPBRTImpl>();
    }
    return raytracerInstance.get();
}


bool LFRayTRacerPBRTRelease()
{
    raytracerInstance = nullptr;
    return true;
}



Real DefaultRayGenerator::GenerateRay( const VEC2& raster, const VEC2& secondary, VEC3& ori, VEC3& dir ) const
{
    return Real();
}


SampleGenerator* DefaultSampleGenerator::Clone() const
{
    return new DefaultSampleGenerator();
}


bool DefaultSampleGenerator::ResetPixel( const Int& x, const Int& y )
{
    return false;
}


bool DefaultSampleGenerator::NextSample( Real& weight, VEC2& raster, VEC2& secondary, Real& time )
{
    return false;
}

}  // namespace lfrt