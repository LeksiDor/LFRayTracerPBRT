#include "LFRayTracerPBRT.h"

#include "LFRayTracer.h"

#include "api.h"
#include "Film.h"
#include "renderoptions.h"


using namespace pbrt;

#include "paramset.h"

#include "filters/box.h"
#include "filters/gaussian.h"
#include "filters/mitchell.h"
#include "filters/sinc.h"
#include "filters/triangle.h"
std::unique_ptr<Filter> MakeFilter( const std::string &name, const ParamSet &paramSet)
{
    Filter *filter = nullptr;
    if (name == "box")
        filter = CreateBoxFilter(paramSet);
    else if (name == "gaussian")
        filter = CreateGaussianFilter(paramSet);
    else if (name == "mitchell")
        filter = CreateMitchellFilter(paramSet);
    else if (name == "sinc")
        filter = CreateSincFilter(paramSet);
    else if (name == "triangle")
        filter = CreateTriangleFilter(paramSet);
    else {
        Error("Filter \"%s\" unknown.", name.c_str());
        exit(1);
    }
    paramSet.ReportUnused();
    return std::unique_ptr<Filter>(filter);
}



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
        const RenderOptions& options = theRenderOptions();
        if (options.FilmName != "image") {
            Error("Unable to create film.");
            return nullptr;
        }
        std::unique_ptr<Filter> filter = MakeFilter(options.FilterName, options.FilterParams);
        Film* film = new Film();
        film->Initialize( options.FilmParams, std::move(filter) );

        return film;
    }

    virtual bool Render( const RayGenerator& raygen, SampleGenerator& sampleGen,
                        SampleAccumulator& sampleAccum ) const override
    {
        return pbrtRenderScene( raygen, sampleGen, sampleAccum );
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