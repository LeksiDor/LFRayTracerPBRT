#include "LFRayTracerPBRT.h"
#include <iostream>

#include <glog/logging.h>

#include "imageio.h"


#include "CustomFilm.h"
#include "CustomRayGen.h"


using namespace lfrt;


void WriteImage( const SampleAccumulator &sampleAccum, const std::string& filepath )
{
    const Int width  = sampleAccum.Width();
    const Int height = sampleAccum.Height();
    std::unique_ptr<Real[]> rgb(new Real[3 * width * height]);
    for (Int x = 0; x < width; ++x) {
        for (Int y = 0; y < height; ++y) {
            const Int offset = 3 * (x + y * width);
            Real& r = rgb[offset + 0];
            Real& g = rgb[offset + 1];
            Real& b = rgb[offset + 2];
            sampleAccum.GetColor(x, y, r, g, b);
        }
    }
    const pbrt::Point2i fullResolution( width, height );
    const pbrt::Bounds2i croppedPixelBounds( pbrt::Point2i(0,0), fullResolution );
    pbrt::WriteImage( filepath, &rgb[0], croppedPixelBounds, fullResolution );
}


int main( int argc, char *argv[] )
{
    if ( argc != 2 )
    {
        std::cout << "You should pass filepath as the argument, nothing more, nothing less." << std::endl;
        return 1;
    }

    LFRayTracer* raytracer = LFRayTracerPBRTInstance();

    raytracer->LoadScene( argv[1] );

    const Int width = 800;
    const Int height = 600;

    const Real ratio = Real(width) / Real(height);

    //std::shared_ptr<const RayGenerator> raygen( raytracer->CreateDefaultRayGenerator( width, height ) );
    std::shared_ptr<const RayGenerator> raygen( new CustomRayGen( width, height, { 1.0, 1.0/ratio } ) );

    std::shared_ptr<SampleGenerator> sampleGen( raytracer->CreateDefaultSampleGenerator( width, height ) );

    //std::shared_ptr<SampleAccumulator> sampleAccum( raytracer->CreateDefaultSampleAccumulator( width, height ) );
    std::shared_ptr<SampleAccumulator> sampleAccum( new CustomFilm( width, height ) );

    raytracer->Render( *raygen, *sampleGen, *sampleAccum );

    WriteImage( *sampleAccum, "result.exr" );

    LFRayTRacerPBRTRelease();

    return 0;
}