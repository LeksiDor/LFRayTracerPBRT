#include "LFRayTracerPBRT.h"

#include "LFRayTracer.h"

#include "api.h"


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
        return nullptr;
    }

    virtual SampleGenerator* CreateDefaultSampleGenerator(
        const Int& width, const Int& height ) const override
    {
        return nullptr;
    }

    virtual SampleAccumulator* CreateDefaultSampleAccumulator(
        const Int& width, const Int& height ) const override
    {
        return nullptr;
    }

    virtual bool Render( const RayGenerator& raygen, SampleGenerator& sampleGen,
                        SampleAccumulator& sampleAccum ) const override
    {
        return false;
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


}  // namespace lfrt