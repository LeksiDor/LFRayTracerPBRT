#include "LFRayTracerPBRT.h"

#include "LFRayTracer.h"

#include "api.h"


using namespace pbrt;


namespace lfrt
{


class LFRayTracerPBRTImpl : public LFRayTracer
{
public:
    LFRayTracerPBRTImpl() = default;

    virtual ~LFRayTracerPBRTImpl()
    {
        //pbrtCleanup();
    }


public:

    virtual bool LoadScene( const std::string& filepath ) override
    {
        FLAGS_stderrthreshold = 1; // Warning and above.
        Options options;
        //options.quiet = true;
        //options.quickRender = false;
        //options.toPly = false;
        //options.cat = true;
        //fflush(stdout);
        pbrtInit( options );
        pbrtParseFile( filepath );
        pbrtCleanup();
        return true;
    }

    virtual RayGenerator* CreateDefaultRayGenerator(
        const Int& width, const Int& height ) const override
    {
        return nullptr;
    }

    virtual SampleGenerator* CreateDefaultSampleGenerator(
        const Int& widht, const Int& height ) const override
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



static std::shared_ptr<LFRayTracerPBRTImpl> raytracerInstance;


LFRayTracer* LFRayTracerPBRTInstance()
{
    if ( raytracerInstance == nullptr )
    {
        raytracerInstance = std::make_shared<LFRayTracerPBRTImpl>();
        google::InitGoogleLogging(""); // To actually enable this, call it from main.cpp with 'argv[0]' argument.
        FLAGS_stderrthreshold = 1;  // Warning and above.
    }
    return raytracerInstance.get();
}


bool LFRayTRacerPBRTRelease()
{
    raytracerInstance = nullptr;
    return true;
}


}  // namespace lfrt