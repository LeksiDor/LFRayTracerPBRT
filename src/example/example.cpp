#include "LFRayTracer.h"
#include "LFRayTracerPBRT.h"
#include <iostream>

#include <glog/logging.h>


using namespace lfrt;


int main( int argc, char *argv[] )
{
    if ( argc != 2 )
    {
        std::cout << "You should pass filepath as the argument, nothing more, nothing less." << std::endl;
        return 1;
    }

    LFRayTracer* raytracer = LFRayTracerPBRTInstance();

    raytracer->LoadScene( argv[1] );

    return 0;
}