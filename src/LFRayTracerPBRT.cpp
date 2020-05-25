#include "LFRayTracerPBRT.h"

#include "LFRayTracer.h"

#include "api.h"


using namespace pbrt;


namespace lfrt
{


class DefaultRayGenerator : public RayGenerator
{
public:
    virtual Real GenerateRay(const VEC2& raster, const VEC2& secondary, VEC3& ori, VEC3& dir) const override
    {
        return Real();
    }
};


class DefaultSampleGenerator : public SampleGenerator
{
public:
    virtual SampleGenerator* Clone() const override { return new DefaultSampleGenerator(); }
    virtual bool ResetPixel( const Int& x, const Int& y ) override { return false; }
    virtual bool NextSample( Real& weight, VEC2& raster, VEC2& secondary, Real& time ) override
    {
        return false;
    }
};


class DefaultSampleAccumulator : public SampleAccumulator
{
public:
    virtual bool SetSize( const Int& width, const Int& y ) override { return false; }
    virtual bool GetSize( Int& x, Int& y ) const override { return false; }
    virtual SampleTile* CreateSampleTile(
        const Int& startX, const Int& startY, const Int& sizeX, const Int& sizeY ) override
    {
        return nullptr;
    }
    virtual bool MergeSampleTile( SampleTile* tile ) override { return false; }
    virtual bool DestroySampleTile( SampleTile* tile ) override { return false; }
    virtual bool GetColor( const Int& x, const Int& y, Real& r, Real& g, Real& b ) const override
    {
        return false;
    }
};


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
        return new DefaultSampleAccumulator();
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


}  // namespace lfrt